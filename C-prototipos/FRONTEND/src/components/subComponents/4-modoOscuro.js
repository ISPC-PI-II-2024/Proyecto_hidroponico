//=============================================
// Boton para alternar entre modo oscuro y claro
//=============================================

import React, { useEffect, useState } from 'react';
import '../../styles/main.css'; 
import { SunIcon, MoonIcon } from '@heroicons/react/24/solid';


export default function DarkModeToggle() {
  const [darkMode, setDarkMode] = useState(false);

  // Cargar la preferencia del tema
  useEffect(() => {
    const saved = localStorage.getItem('theme');
    if (saved === 'dark') {
      document.body.classList.add('dark-mode');
      setDarkMode(true);
    }
  }, []);

  // Alternar modo oscuro/claro
  const toggleDarkMode = () => {
    const newMode = !darkMode;
    setDarkMode(newMode);
    if (newMode) {
      document.body.classList.add('dark-mode');
      localStorage.setItem('theme', 'dark');
    } else {
      document.body.classList.remove('dark-mode');
      localStorage.setItem('theme', 'light');
    }
  };

  return (
    <button
      onClick={toggleDarkMode}
      className="dark-toggle"
      aria-label={darkMode ? 'Desactivar modo oscuro' : 'Activar modo oscuro'}
    >
      {darkMode ? <SunIcon className="w-6 h-6" /> : <MoonIcon className="w-6 h-6" />}
    </button>
  );
}