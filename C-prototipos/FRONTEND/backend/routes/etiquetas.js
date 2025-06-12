const express = require('express');
const { getEtiquetas } = require('../controllers/etiquetas');
const router = express.Router();

// Definir el endpoint GET para obtener todas las etiquetas
router.get('/', getEtiquetas);

module.exports = router;
