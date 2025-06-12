/*********************************************************/
/*********************** FOOTER **************************/
/*********************************************************/

import React from 'react';

function Footer() {
  return (
    <footer className="footer">
      <div className="footer-left">
        <img
          src="/assets/Logos/Logos_Claro_-05.png"
          alt="Logo Claro"
          className="footer-logo"
        />
        <p>© 2024 Claro. Todos los derechos reservados.</p>
      </div>
      <div className="footer-right">
        <span className="tooltip">
          CONTACTO:{' '}
          <a
            href="mailto:vittorio.durigutti@claro.com.ar?subject=Claro-Home"
            target="_blank"
            rel="noopener noreferrer"
          >
            Durigutti Vittorio
          </a>
          <span className="tooltiptext">
            Presiona en Vitto para enviarme un correo
          </span>
        </span>
      </div>
    </footer>
  );
}

export default Footer;

