const HOST = process.env.HOST || '0.0.0.0';
const PORT = process.env.PORT || 3004;
const PUBLIC_BASE_URL = process.env.PUBLIC_BASE_URL;

const config = {
  HOST,
  PORT,
  get BASE_URL() {
    return PUBLIC_BASE_URL || `http://${HOST}:${PORT}`;
  },
};

module.exports = config;
