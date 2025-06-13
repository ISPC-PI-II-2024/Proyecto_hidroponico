# mariadb_serv.py
from datetime import datetime
import sqlalchemy as sa
from sqlalchemy import (
    Table, MetaData, Column, Integer, String, Float, DateTime, ForeignKey,
    select, insert, update
)
from app.utils.logger import get_logger
from app.config import settings
from app.models.mensaje import GatewayMessage, DeviceInfo

logger = get_logger("mariadb_service")

_engine = sa.create_engine(settings.mariadb_url)
_metadata = MetaData()

_sistemas = Table(
    "sistemas", _metadata,
    Column("id_sistema", Integer, primary_key=True, autoincrement=True),
    Column("nombre", String(100), nullable=False, unique=True),
    Column("fecha_instalacion", DateTime, nullable=False),
)

_sensores = Table(
    "sensores", _metadata,
    Column("id_sensores", Integer, primary_key=True, autoincrement=True),
    Column("nombre", String(100), nullable=False),
    Column("tipo", String(50)),
    Column("pin_entrada", Integer),
    Column("id_sistema", Integer, ForeignKey("sistemas.id_sistema")),
)

_lecturas = Table(
    "lecturas", _metadata,
    Column("id_lectura", Integer, primary_key=True, autoincrement=True),
    Column("id_sensor", Integer, ForeignKey("sensores.id_sensores")),
    Column("valor", Float),
    Column("fecha_hora", DateTime),
)

_actuadores = Table(
    "actuadores", _metadata,
    Column("id_actuador", Integer, primary_key=True, autoincrement=True),
    Column("nombre", String(100)),
    Column("tipo", String(50)),
    Column("estado_actual", String(3)),
    Column("pin_salida", Integer),
    Column("id_sistema", Integer, ForeignKey("sistemas.id_sistema")),
)

_eventos = Table(
    "eventos_control", _metadata,
    Column("id_evento", Integer, primary_key=True, autoincrement=True),
    Column("id_actuador", Integer, ForeignKey("actuadores.id_actuador")),
    Column("accion", String(3)),
    Column("fecha_hora", DateTime),
)

_metadata.create_all(_engine)

# Mapeo de nombres para variables de sensores
SENSOR_NAME_MAP = {
    "temperaturaAgua": "tempAgua",
    "temperaturaAire": "tempAire",
    "nivelAgua":      "nivel",
    "flujoAgua":      "flujo",
}


def procesar_info_inicial(info: DeviceInfo) -> None:
    with _engine.begin() as conn:
        # Upsert en sistemas
        res = conn.execute(
            select(_sistemas.c.id_sistema)
            .where(_sistemas.c.nombre == info.gatewayId)
        ).first()
        if res:
            id_sis = res.id_sistema
        else:
            result = conn.execute(
                insert(_sistemas).values(
                    nombre=info.gatewayId,
                    fecha_instalacion=info.timestamp
                )
            )
            id_sis = result.inserted_primary_key[0]

        # Upsert en sensores y actuadores (igual que antes)
        for node in info.nodes:
            for nombre, pininfo in node.sensors.items():
                pin_val = int(pininfo.pin.replace("GPIO", "")) if pininfo.pin else None
                row = conn.execute(
                    select(_sensores.c.id_sensores)
                    .where(( _sensores.c.nombre == nombre ) & (_sensores.c.id_sistema == id_sis))
                ).first()
                if row:
                    conn.execute(
                        update(_sensores)
                        .where(_sensores.c.id_sensores == row.id_sensores)
                        .values(tipo=pininfo.type, pin_entrada=pin_val)
                    )
                else:
                    conn.execute(
                        insert(_sensores).values(
                            nombre=nombre, tipo=pininfo.type,
                            pin_entrada=pin_val, id_sistema=id_sis
                        )
                    )
            for nombre, pininfo in node.controls.items():
                pin_val = int(pininfo.pin.replace("GPIO", "")) if pininfo.pin else None
                row = conn.execute(
                    select(_actuadores.c.id_actuador)
                    .where(( _actuadores.c.nombre == nombre ) & (_actuadores.c.id_sistema == id_sis))
                ).first()
                if row:
                    conn.execute(
                        update(_actuadores)
                        .where(_actuadores.c.id_actuador == row.id_actuador)
                        .values(tipo=pininfo.type, pin_salida=pin_val)
                    )
                else:
                    conn.execute(
                        insert(_actuadores).values(
                            nombre=nombre, tipo=pininfo.type,
                            pin_salida=pin_val, estado_actual=None,
                            id_sistema=id_sis
                        )
                    )


def guardar_datos(msg: GatewayMessage) -> None:
    with _engine.begin() as conn:
        id_sis = conn.execute(
            select(_sistemas.c.id_sistema)
            .where(_sistemas.c.nombre == msg.gatewayId)
        ).scalar_one()

        for node in msg.nodes:
            # -- Procesar valores de sensores --
            for nombre, valor in node.sensors.items():
                if nombre == "hora":
                    continue
                mapped = SENSOR_NAME_MAP.get(nombre, nombre)
                row = conn.execute(
                    select(_sensores.c.id_sensores)
                    .where(( _sensores.c.nombre == mapped ) & (_sensores.c.id_sistema == id_sis))
                ).first()
                if not row:
                    # Crear sensor dinámicamente
                    result = conn.execute(
                        insert(_sensores).values(
                            nombre=mapped, tipo=None,
                            pin_entrada=None, id_sistema=id_sis
                        )
                    )
                    id_sen = result.inserted_primary_key[0]
                    logger.info(f"Sensor '{mapped}' creado dinámicamente con id {id_sen}.")
                else:
                    id_sen = row.id_sensores
                conn.execute(
                    insert(_lecturas).values(
                        id_sensor=id_sen,
                        valor=valor,
                        fecha_hora=node.timestamp
                    )
                )

            # -- Procesar controles/actuadores --
            for nombre, estado in node.controls.items():
                if nombre == "modoAutomatico":
                    continue
                row_act = conn.execute(
                    select(_actuadores.c.id_actuador)
                    .where(( _actuadores.c.nombre == nombre ) & (_actuadores.c.id_sistema == id_sis))
                ).first()
                if not row_act:
                    logger.warning(f"Actuador '{nombre}' no registrado, omitiendo control")
                    continue
                id_act = row_act.id_actuador
                conn.execute(
                    update(_actuadores)
                    .where(_actuadores.c.id_actuador == id_act)
                    .values(estado_actual='ON' if estado else 'OFF')
                )
                conn.execute(
                    insert(_eventos).values(
                        id_actuador=id_act,
                        accion='ON' if estado else 'OFF',
                        fecha_hora=node.timestamp
                    )
                )
