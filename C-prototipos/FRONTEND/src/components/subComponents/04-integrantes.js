//=============================================
// Tarjeta de Integrantes del Proyecto
//=============================================

import React from 'react';
import { Linkedin, GitHub } from 'lucide-react';

/**
 * Tarjeta de integrante con foto, nombre y enlaces a LinkedIn/GitHub.
 *
 * @param {{
 *   name: string,               // Nombre y apellido del integrante
 *   photoSrc: string,           // Ruta a la imagen: '/assets/02_integrantes/ejemplofoto.png'
 *   linkedinUrl: string,        // URL al perfil de LinkedIn
 *   githubUrl: string           // URL al perfil de GitHub
 * }} props
 */
export default function IntegranteCard({ name, photoSrc, linkedinUrl, githubUrl }) {
  return (
    <div className="max-w-xs bg-white rounded-2xl shadow-lg overflow-hidden p-6 flex flex-col items-center text-center">
      {/* Foto del integrante */}
      <img
        src={photoSrc}
        alt={`Foto de ${name}`}
        className="w-32 h-32 rounded-full object-cover mb-4"
      />

      {/* Nombre y apellido */}
      <h3 className="text-xl font-semibold mb-3 text-gray-800">{name}</h3>

      {/* Iconos de redes */}
      <div className="flex space-x-6">
        <a
          href={linkedinUrl}
          target="_blank"
          rel="noopener noreferrer"
          className="text-blue-600 hover:text-blue-800 transition-colors"
          aria-label="LinkedIn"
        >
          <Linkedin size={28} />
        </a>

        <a
          href={githubUrl}
          target="_blank"
          rel="noopener noreferrer"
          className="text-gray-800 hover:text-gray-900 transition-colors"
          aria-label="GitHub"
        >
          <GitHub size={28} />
        </a>
      </div>
    </div>
  );
}
