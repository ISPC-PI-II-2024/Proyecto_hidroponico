from datetime import datetime
from typing import List, Dict, Optional, Union
from pydantic import BaseModel, Field, validator
from dateutil.parser import parse as _parse_dt

def _parse_datetime(v: str) -> datetime:
    # acepta tanto "2025-5-19 6:24:12" como ISO8601
    try:
        return _parse_dt(v)
    except Exception:
        raise ValueError(f"timestamp inválido: {v}")

#========================================
# FORMATO DE LOS MENSAJES
#========================================

# ----------------------------
# MODELO PARA LECTURAS
# ----------------------------
class ReadingNode(BaseModel):
    deviceId: str
    timestamp: datetime
    transmitter: bool
    receiver: bool
    sensors: Dict[str, float]
    controls: Dict[str, bool]

    _parse_ts = validator("timestamp", pre=True, allow_reuse=True)(_parse_datetime)

class GatewayMessage(BaseModel):
    gatewayId: str
    timestamp: datetime
    nodes: List[ReadingNode]

    _parse_ts = validator("timestamp", pre=True, allow_reuse=True)(_parse_datetime)

# ----------------------------
# MODELO PARA CONFIGURACIÓN
# ----------------------------
class PinInfo(BaseModel):
    pin: str
    type: Optional[str] = None
    source: Optional[str] = None

class ConfigNode(BaseModel):
    deviceId: str
    timestamp: datetime
    transmitter: bool
    receiver: bool
    sensors: Dict[str, PinInfo]
    controls: Dict[str, PinInfo]

    _parse_ts = validator("timestamp", pre=True, allow_reuse=True)(_parse_datetime)

class DeviceInfo(BaseModel):
    gatewayId: str
    timestamp: datetime
    nodes: List[ConfigNode]

    _parse_ts = validator("timestamp", pre=True, allow_reuse=True)(_parse_datetime)