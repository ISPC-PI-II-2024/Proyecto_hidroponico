# mensaje.py
from datetime import datetime
from typing import List, Dict, Optional
from pydantic import BaseModel, validator
from dateutil.parser import parse as _parse_dt

def _parse_datetime(v: str) -> datetime:
    try:
        return _parse_dt(v)
    except Exception:
        raise ValueError(f"timestamp inválido: {v}")

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

class PinInfo(BaseModel):
    pin: Optional[str] = None
    type: Optional[str] = None
    source: Optional[str] = None

class ConfigNode(BaseModel):
    deviceId: str
    timestamp: datetime
    transmitter: bool
    receiver: bool
    sensors: Dict[str, PinInfo]
    controls: Dict[str, PinInfo] = {}

    _parse_ts = validator("timestamp", pre=True, allow_reuse=True)(_parse_datetime)

class DeviceInfo(BaseModel):
    gatewayId: str
    timestamp: datetime
    nodes: List[ConfigNode]

    _parse_ts = validator("timestamp", pre=True, allow_reuse=True)(_parse_datetime)