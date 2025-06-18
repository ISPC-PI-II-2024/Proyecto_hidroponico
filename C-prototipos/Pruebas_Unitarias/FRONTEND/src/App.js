import React, { useState, useEffect } from 'react';
import { BrowserRouter, Routes, Route, Navigate } from 'react-router-dom';

import Menu from './components/Menu';
import Header from './components/Header';
import Footer from './components/Footer';

import Pagina0 from './pages/pagina0';
import Pagina1 from './pages/pagina1';

import './styles/main.css';

export default function App() {
  const [loading, setLoading] = useState(true);
  const [exiting, setExiting] = useState(false);

  useEffect(() => {
    // Mostrar loader 2s, luego disparar salida y ocultarlo tras 0.8s
    const loadTimer = setTimeout(() => {
      setExiting(true);            // agrega la clase .exit y lanza la animación
      setTimeout(() => {
        setLoading(false);         // quita el loader del DOM
      }, 800);                     // mismo tiempo que loaderToHeader
    }, 2000);

    return () => clearTimeout(loadTimer);
  }, []);

  return (
    <BrowserRouter>
      {loading ? (
        <div className={`loader ${exiting ? 'exit' : ''}`}>
          <div className="loader-overlay" />
          <div className="spinner" />
        </div>
      ) : (
        <div className="app-container">
          <Menu />

          <Header />

          <main className="content">
            <Routes>
              <Route index element={<Navigate to="/pagina0" replace />} />
              <Route path="/pagina0" element={<Pagina0 />} />
              <Route path="/pagina1" element={<Pagina1 />} />
            </Routes>
          </main>

          <Footer />
        </div>
      )}
    </BrowserRouter>
  );
}
