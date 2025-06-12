'use strict';

const express = require('express');
const cors = require('cors');
const bodyParser = require('body-parser');
const path = require('path');
if (process.env.NODE_ENV === 'development') {
    require('dotenv').config({ path: path.join(__dirname, '..', '.env.local') });
} else {
    require('dotenv').config();
}
  

// Forzamos `NODE_ENV` si fuera necesario (o podrías dejar que venga del entorno)
process.env.NODE_ENV = process.env.NODE_ENV || 'development';


// Importamos nuestra configuración centralizada
const config = require('./models/config'); 
const { HOST, PORT, BASE_URL } = config;

const app = express();

console.log(" Iniciando servidor...");
console.log(` Entorno: ${process.env.NODE_ENV}`);
console.log(` BASE_URL: ${BASE_URL}`);

// Middlewares globales
app.use(cors());
app.use(express.json());
app.use(bodyParser.urlencoded({ extended: true }));

console.log(" Importando rutas...");

// Declaración de variables para rutas
let imagenesRoutes, cumpleanosRoutes, grafanasRoutes, gestoresPowerRoutes, sesionesRoutes, etiquetasRoutes;

try {
    imagenesRoutes = require('./routes/imagenes');
    //...

    console.log("Rutas importadas correctamente.");
} catch (error) {
    console.error("Error al importar rutas:", error);
}

// Verificar si las rutas importadas son válidas
console.log("🔍 Verificando módulos de rutas antes de registrar en `app`...");
console.log("imagenesRoutes:", typeof imagenesRoutes);
//...

try {
    if (imagenesRoutes) {
        app.use('/api/imagenes', imagenesRoutes);
        console.log("Ruta '/api/imagenes' registrada correctamente en `app`.");
    }
    if (cumpleanosRoutes) {
        app.use('/api/cumpleanos', cumpleanosRoutes);
        console.log("Ruta '/api/cumpleanos' registrada correctamente en `app`.");
    } // ...
} catch (error) {
    console.error("Error al registrar rutas en `app`:", error);
}

// Verificación de rutas en `app` después de registrar
console.log("Rutas en Express después de agregar rutas:");
console.log(app._router?.stack.map(r => r.route && r.route.path).filter(Boolean));

// Servir imágenes desde "uploads"
app.use('/uploads', express.static(path.join(__dirname, 'uploads')));
// (Opcional) Servir subcarpeta específica
app.use('/uploads/gestores_power', express.static(path.join(__dirname, 'uploads/gestores_power')));

// Middleware de error 404
app.use((req, res, next) => {
    console.log("Middleware de error ejecutado para:", req.originalUrl);
    res.status(404).json({ error: 'Ruta no encontrada' });
});

console.log("Rutas en Express antes de iniciar el servidor:");
console.log(app._router?.stack.map(r => r.route && r.route.path).filter(Boolean));

// Escuchar en el host y puerto configurados
app.listen(PORT, HOST, () => {
    console.log(`Servidor corriendo en ${BASE_URL}`);
});
