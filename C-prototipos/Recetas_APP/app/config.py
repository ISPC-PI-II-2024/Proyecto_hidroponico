from typing import List, Optional
from pydantic import Field
from pydantic_settings import BaseSettings, SettingsConfigDict

class Settings(BaseSettings):
    # MQTT
    mqtt_broker_url: str           = Field(...,         env="MQTT_BROKER_URL")
    mqtt_user: Optional[str]       = Field(None,        env="MQTT_USER")
    mqtt_password: Optional[str]   = Field(None,        env="MQTT_PASSWORD")
    mqtt_topic_info: List[str]     = Field(...,         env="MQTT_TOPIC_INFO")
    mqtt_topic_data: List[str]     = Field(...,         env="MQTT_TOPIC_DATA")


    # MariaDB
    mariadb_url: str               = Field(...,         env="MARIADB_URL")

    # App HTTP
    api_host: str                  = Field("0.0.0.0",   env="API_HOST")
    api_port: int                  = Field(8000,        env="API_PORT")

    model_config = SettingsConfigDict(
        env_file=".env",
        env_file_encoding="utf-8",
        extra="ignore",
    )

settings = Settings()
