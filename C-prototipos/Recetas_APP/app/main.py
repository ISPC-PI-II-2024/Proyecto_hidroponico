from fastapi import FastAPI
import uvicorn

from app.config import settings
from app.utils.logger import get_logger
from app.mqtt_listener import listener
from app.api.routes import router as api_router

#========================================
# Inicialización de la aplicación FastAPI
#========================================
tmpl_app = FastAPI()
logger = get_logger("main")

#========================================
# INICIALIZACION DE MQTT LISTENER
#========================================
@tmpl_app.on_event("startup")
async def startup_event():
    logger.info("Iniciando MQTT listener...")
    listener.start()

#========================================
# HEAKTCHECK PARA VER QUE LA API SIGUE VIVA
#========================================
@tmpl_app.get("/health")
async def healthcheck():
    return {"status": "ok"}

#========================================
# RUTAS DE LA API
#========================================
tmpl_app.include_router(api_router, prefix="/api")

if __name__ == "__main__":
    uvicorn.run(
        "app.main:tmpl_app",
        host=settings.api_host,
        port=settings.api_port,
        reload=True  # Solo en desarrollo =TRUE; quitar en producción
    )
