# Utiliza Python 3.11 slim
FROM python:3.11-slim

# Establece el directorio de trabajo
WORKDIR /app

# Copia e instala dependencias
COPY requirements.txt .
RUN pip install --no-cache-dir --upgrade pip \
 && pip install --no-cache-dir -r requirements.txt

# Copia el resto del código
COPY . .

# Expón el puerto de la API
EXPOSE ${API_PORT}

# Comando por defecto: arranca uvicorn
CMD ["uvicorn", "app.main:tmpl_app", "--host", "0.0.0.0", "--port", "8000"]
