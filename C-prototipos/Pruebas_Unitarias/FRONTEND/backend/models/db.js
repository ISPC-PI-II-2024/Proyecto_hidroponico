const mysql = require('mysql2');

// Configuración del pool de conexiones
const pool = mysql.createPool({
  host: process.env.DB_HOST,
  port: process.env.DB_PORT || 3306, 
  user: process.env.DB_USER,
  password: process.env.DB_PASSWORD,
  database: process.env.DB_NAME,
  waitForConnections: true,
  connectionLimit: 10,
  queueLimit: 0
});

// Probar la conexión inicial (opcional)
pool.getConnection((err, connection) => {
  if (err) {
    console.error('❌ Error al conectar a MySQL:', err.message);
    process.exit(1);
  }
  console.log('✅ Conexión a MySQL establecida mediante pool.');
  connection.release(); // Liberar la conexión para que se reutilice
});

module.exports = pool;
