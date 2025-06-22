import logging
from app.config import settings

# De momento solo defino nivel de log en INFO, podemos agregar warningins o errores si nos hace falta.
DEFAULT_LEVEL = getattr(logging, "INFO", logging.INFO)

def configure_root_logger():
    """
    Configura el logger raíz una sola vez, con formato y nivel de log estándar.
    """
    level_name = getattr(settings, "log_level", None)
    if level_name:
        level = getattr(logging, level_name.upper(), DEFAULT_LEVEL)
    else:
        level = DEFAULT_LEVEL


    root = logging.getLogger()
    if not root.handlers:
        handler = logging.StreamHandler()
        fmt = "%(asctime)s [%(levelname)s] %(name)s: %(message)s"
        handler.setFormatter(logging.Formatter(fmt))
        root.setLevel(level)
        root.addHandler(handler)

def get_logger(name: str) -> logging.Logger:
    """
    Devuelve un logger con el nombre indicado, garantizando que el logger raíz
    ya esté configurado.
    """
    configure_root_logger()
    return logging.getLogger(name)
