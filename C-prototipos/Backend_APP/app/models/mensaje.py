from datetime import datetime
from typing import List, Optional
from pydantic import BaseModel, Field, validator
from dateutil import parser
from dateutil.parser import ParserError

#========================================
# FORMATO DE LOS MENSAJES
#========================================

#----------------------------------------
# Submodelos
#----------------------------------------
class SensorData(BaseModel):
    temperaturaAgua: float
    temperaturaAire: float
    humedad: float
    nivelAgua: float
    flujoAgua: float
    luz: int
    gas: int
    corriente: float
    voltaje: float
    potencia: float
    phValor: float

    # Timestamp local opcional por sensor, tal como lo envía el dispositivo
    hora: Optional[datetime] = Field(
        None,
        description="Marca de tiempo local del sensor, formato 'YYYY-M-D H:M:S'"
    )

    @validator('hora', pre=True)
    def parse_hora(cls, v):
        if v is None:
            return None
        try:
            # Acepta distintos formatos y parsea
            return parser.parse(v)
        except (ParserError, TypeError, ValueError):
            # En caso de formato inválido, se ignora
            return None

class Controls(BaseModel):
    bomba: bool
    luces: bool
    alarma: bool
    modoAutomatico: bool

#----------------------------------------
# Nodo con datos de cada dispositivo
#----------------------------------------
class Node(BaseModel):
    deviceId: str
    timestamp: datetime
    transmitter: Optional[bool] = Field(None, description="Flag transmisor, si aplica")
    reciber:    Optional[bool] = Field(None, description="Flag receptor, si aplica")
    sensors:    SensorData
    controls:   Controls

    @validator('timestamp', pre=True)
    def parse_timestamp(cls, v):
        try:
            return parser.parse(v)
        except (ParserError, TypeError, ValueError):
            # Si viene mal el timestamp, retorna hora actual UTC
            return datetime.utcnow()

#----------------------------------------
# Modelo principal
#----------------------------------------
class GatewayMessage(BaseModel):
    gatewayId: str
    timestamp: datetime
    nodes: List[Node]

    @validator('timestamp', pre=True)
    def parse_gateway_timestamp(cls, v):
        try:
            return parser.parse(v)
        except (ParserError, TypeError, ValueError):
            # Si formato inválido, usar UTC now
            return datetime.utcnow()
