from fastapi import APIRouter

router = APIRouter()

# Importa todos los endpoints para que el router los registre
# De momento no hay endpoints, pero queda a mano por si acaso
default_router = router
from app.api import routes  