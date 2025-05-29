from typing import Optional, List
from datetime import datetime
from dateutil import parser
from dateutil.parser import ParserError

from app.utils.logger import get_logger
from app.models.mensaje import GatewayMessage
import sqlalchemy as sa
from sqlalchemy import Table, MetaData, Column, String, DateTime, Boolean, Float, Integer, func
from app.config import settings

logger = get_logger("mariadb_service")

#========================================
# Configuración SQLAlchemy
#========================================
_engine = sa.create_engine(settings.mariadb_url)
_metadata = MetaData()

# Tabla de dispositivos
_device_table = Table(
    "Tabla dispostivo", # Hay que cambiarlo cuando tengamos la info de la tabla definida
    _metadata,
    Column("device_id", String(64), primary_key=True),
    Column("name", String(128), nullable=False),
    Column("created_at", DateTime, nullable=False, server_default=func.current_timestamp()),
    Column("transmitter", Boolean, nullable=False, default=False),
    Column("receiver", Boolean, nullable=False, default=False),
    Column("gateway", Boolean, nullable=False, default=False),
)

# Tabla de datos de sensores
_sensor_table = Table(
    "Tbla datos sensor", # Hay que cambiarlo cuando tengamos la info de la tabla definida
    _metadata,
    Column("id", sa.Integer, primary_key=True, autoincrement=True),
    Column("device_id", String(64), nullable=False),
    Column("timestamp", DateTime, nullable=False),
    Column("temperatura_agua", Float),
    Column("temperatura_aire", Float),
    Column("humedad", Float),
    Column("nivel_agua", Float),
    Column("flujo_agua", Float),
    Column("luz", Integer),
    Column("gas", Integer),
    Column("corriente", Float),
    Column("voltaje", Float),
    Column("potencia", Float),
    Column("ph_valor", Float),
    Column("hora_local", DateTime, nullable=True),
)

# Tabla de controles
_control_table = Table(
    "Tabla Controladores", # Hay que cambiarlo cuando tengamos la info de la tabla definida
    _metadata,
    Column("id", sa.Integer, primary_key=True, autoincrement=True),
    Column("device_id", String(64), nullable=False),
    Column("timestamp", DateTime, nullable=False),
    Column("bomba", Boolean),
    Column("luces", Boolean),
    Column("alarma", Boolean),
    Column("modo_automatico", Boolean),
)

_metadata.create_all(_engine)

#========================================
# Función que registra info en MariaDB
#========================================
def guardarDatos(msg: GatewayMessage) -> None:
    logger.debug(f"Guardando información de dispositivos en MariaDB: {msg}")
    with _engine.connect() as conn:
        for node in msg.nodes:
            gateway_flag = node.deviceId == msg.gatewayId

            # Guardar info de dispositivo
            ins_device = sa.insert(_device_table).values(
                device_id=node.deviceId,
                name=node.deviceId,
                transmitter=node.transmitter or False,
                receiver=node.reciber or False,
                gateway=gateway_flag
            ).prefix_with("IGNORE")
            conn.execute(ins_device)

            # Guardar datos de sensores
            sensor = node.sensors
            ins_sensor = sa.insert(_sensor_table).values(
                device_id=node.deviceId,
                timestamp=node.timestamp,
                temperatura_agua=sensor.temperaturaAgua,
                temperatura_aire=sensor.temperaturaAire,
                humedad=sensor.humedad,
                nivel_agua=sensor.nivelAgua,
                flujo_agua=sensor.flujoAgua,
                luz=sensor.luz,
                gas=sensor.gas,
                corriente=sensor.corriente,
                voltaje=sensor.voltaje,
                potencia=sensor.potencia,
                ph_valor=sensor.phValor,
                hora_local=sensor.hora
            )
            conn.execute(ins_sensor)

            # Guardar datos de controles
            control = node.controls
            ins_control = sa.insert(_control_table).values(
                device_id=node.deviceId,
                timestamp=node.timestamp,
                bomba=control.bomba,
                luces=control.luces,
                alarma=control.alarma,
                modo_automatico=control.modoAutomatico
            )
            conn.execute(ins_control)

        conn.commit()
    logger.info("Información de dispositivos, sensores y controles escrita en MariaDB.")
