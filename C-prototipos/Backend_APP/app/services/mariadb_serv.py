from datetime import datetime
import sqlalchemy as sa
from sqlalchemy import (
    Table, MetaData, Column, Integer, String, Float, Boolean, DateTime, ForeignKey,
    select, insert, update
)
from app.utils.logger import get_logger
from app.config import settings
from app.models.mensaje import GatewayMessage, DeviceInfo

logger = get_logger("mariadb_service")

#========================================
# Configuración de la base de datos MariaDB
#========================================
_engine = sa.create_engine(settings.mariadb_url)
_metadata = MetaData()

#========================================
# Definición de las tablas
#========================================
_sistemas = Table(
    "sistemas", _metadata,
    Column("id_sistema", Integer, primary_key=True, autoincrement=True),
    Column("nombre", String(64), nullable=False, unique=True),
    Column("fecha_instalacion", DateTime, nullable=False),
)

_sensores = Table(
    "sensores", _metadata,
    Column("id_sensor", Integer, primary_key=True, autoincrement=True),
    Column("nombre", String(64), nullable=False),
    Column("pin", String(16)),
    Column("id_sistema", Integer, ForeignKey("sistemas.id_sistema")),
)

_lecturas = Table(
    "lecturas", _metadata,
    Column("id_lectura", Integer, primary_key=True, autoincrement=True),
    Column("id_sensor", Integer, ForeignKey("sensores.id_sensor")),
    Column("valor", Float),
    Column("fecha_hora", DateTime),
)

_actuadores = Table(
    "actuadores", _metadata,
    Column("id_actuador", Integer, primary_key=True, autoincrement=True),
    Column("nombre", String(64), nullable=False),
    Column("pin", String(16)),
    Column("tipo", String(32)),
    Column("estado_actual", Boolean),
    Column("id_sistema", Integer, ForeignKey("sistemas.id_sistema")),
)

_eventos = Table(
    "eventos_control", _metadata,
    Column("id_evento", Integer, primary_key=True, autoincrement=True),
    Column("id_actuador", Integer, ForeignKey("actuadores.id_actuador")),
    Column("accion", Boolean),
    Column("fecha_hora", DateTime),
)

_metadata.create_all(_engine)


#========================================
# Funciones de procesamiento de mensajes
#========================================
def procesar_info_inicial(info: DeviceInfo) -> None:
    """
    Maneja el payload de 'dispositivo/info':
     - Upsert en 'sistemas'
     - Upsert en 'sensores' con pin
     - Upsert en 'actuadores' con pin y type
    """
    with _engine.begin() as conn:

        # ----------------------------
        #  Sistemas
        # ----------------------------
        res = conn.execute(
            select(_sistemas.c.id_sistema)
             .where(_sistemas.c.nombre == info.gatewayId)
        ).first()

        if res:
            id_sis = res.id_sistema
        else:
            result = conn.execute(
                insert(_sistemas)
                .values(nombre=info.gatewayId, fecha_instalacion=info.timestamp)
            )
            id_sis = result.inserted_primary_key[0]

        # ----------------------------
        # Sensores
        # ----------------------------
        for node in info.nodes:
            for nombre, pininfo in node.sensors.items():
                row = conn.execute(
                    select(_sensores.c.id_sensor)
                     .where(
                         (_sensores.c.nombre == nombre) &
                         (_sensores.c.id_sistema == id_sis)
                     )
                ).first()
                if row:
                    conn.execute(
                        update(_sensores)
                        .where(_sensores.c.id_sensor == row.id_sensor)
                        .values(pin=pininfo.pin)
                    )
                else:
                    conn.execute(
                        insert(_sensores).values(
                            nombre=nombre,
                            pin=pininfo.pin,
                            id_sistema=id_sis
                        )
                    )

        # ----------------------------
        # Actuadores
        # ----------------------------
        for node in info.nodes:
            for nombre, pininfo in node.controls.items():
                row = conn.execute(
                    select(_actuadores.c.id_actuador)
                     .where(
                         (_actuadores.c.nombre == nombre) &
                         (_actuadores.c.id_sistema == id_sis)
                     )
                ).first()
                if row:
                    conn.execute(
                        update(_actuadores)
                        .where(_actuadores.c.id_actuador == row.id_actuador)
                        .values(pin=pininfo.pin, tipo=pininfo.type)
                    )
                else:
                    conn.execute(
                        insert(_actuadores).values(
                            nombre=nombre,
                            pin=pininfo.pin,
                            tipo=pininfo.type,
                            estado_actual=None,
                            id_sistema=id_sis
                        )
                    )

#========================================
# Función para guardar datos de sensores y actuadores
#========================================
def guardar_datos(msg: GatewayMessage) -> None:
    """
    Maneja el payload de 'sensores/datos':
     - Inserta en 'lecturas'
     - Actualiza 'actuadores.estado_actual' y loggea en 'eventos_control'
    """
    with _engine.begin() as conn:
        id_sis = conn.execute(
            select(_sistemas.c.id_sistema)
             .where(_sistemas.c.nombre == msg.gatewayId)
        ).scalar_one()

        for node in msg.nodes:
            for nombre, valor in node.sensors.items():
                id_sen = conn.execute(
                    select(_sensores.c.id_sensor)
                     .where(
                         (_sensores.c.nombre == nombre) &
                         (_sensores.c.id_sistema == id_sis)
                     )
                ).scalar_one()
                conn.execute(
                    insert(_lecturas).values(
                        id_sensor=id_sen,
                        valor=valor,
                        fecha_hora=node.timestamp
                    )
                )

        # ----------------------------
        # Actualización de actuadores
        # ----------------------------
            for nombre, estado in node.controls.items():
                id_act = conn.execute(
                    select(_actuadores.c.id_actuador)
                     .where(
                         (_actuadores.c.nombre == nombre) &
                         (_actuadores.c.id_sistema == id_sis)
                     )
                ).scalar_one()
                # 1) actualizo estado actual
                conn.execute(
                    update(_actuadores)
                    .where(_actuadores.c.id_actuador == id_act)
                    .values(estado_actual=estado)
                )
                # 2) inserto evento de control
                conn.execute(
                    insert(_eventos).values(
                        id_actuador=id_act,
                        accion=estado,
                        fecha_hora=node.timestamp
                    )
                )
