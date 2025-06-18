const db = require('../models/db');

/**
 * GET /api/etiquetas
 * Retorna una lista plana de etiquetas ordenadas por nombre.
 */
exports.getEtiquetas = (req, res) => {
  const query = 'SELECT * FROM items ORDER BY name';
  db.query(query, (err, results) => {
    if (err) {
      console.error('Error al obtener etiquetas:', err);
      return res.status(500).json({ error: 'Error al obtener etiquetas', details: err.message });
    }
    res.json(results);
  });
};
