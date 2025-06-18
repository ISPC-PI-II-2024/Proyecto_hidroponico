//=============================================
// HOOK COLUMNAS
//=============================================


import { useState, useEffect } from "react";

/**
 * useColumns
 * Encapsula la lógica para manejar 3 columnas de enlaces:
 * - Carga de valores iniciales desde localStorage.
 * - Guardado automático en localStorage.
 * - Drag & drop entre columnas.
 *
 * @param {Object} defaultValues - Objeto con los valores por defecto de cada columna.
 * @param {Object} storageKeys - Objeto con las llaves de localStorage para cada columna.
 * @returns {Object} { columns, setColumns, handleDragStart, handleDrop }
 */
export default function useColumnas(defaultValues, storageKeys) {
  const getInitialLinks = (key, defaultValue) => {
    const savedLinks = localStorage.getItem(key);
    return savedLinks ? JSON.parse(savedLinks) : defaultValue;
  };

  // Estado para cada columna
  const [columns, setColumnas] = useState({
    col1: getInitialLinks(storageKeys.col1, defaultValues.col1),
    col2: getInitialLinks(storageKeys.col2, defaultValues.col2),
    col3: getInitialLinks(storageKeys.col3, defaultValues.col3),
  });

  // Guardar columnas en localStorage cuando cambien
  useEffect(() => {
    localStorage.setItem(storageKeys.col1, JSON.stringify(columns.col1));
    localStorage.setItem(storageKeys.col2, JSON.stringify(columns.col2));
    localStorage.setItem(storageKeys.col3, JSON.stringify(columns.col3));
  }, [columns, storageKeys]);

  // Drag & Drop
  const handleDragStart = (e, link, originColumn) => {
    e.dataTransfer.setData("link", JSON.stringify({ link, originColumn }));
  };

  const handleDrop = (e, targetColumn) => {
    e.preventDefault();
    const { link, originColumn } = JSON.parse(e.dataTransfer.getData("link"));

    if (originColumn === targetColumn) {
      return;
    }
    // Eliminar el enlace de la columna de origen
    setColumnas((prev) => ({
      ...prev,
      [originColumn]: prev[originColumn].filter((l) => l.url !== link.url),
      [targetColumn]: [...prev[targetColumn], link],
    }));
  };

  return {
    columns,         // { col1: [...], col2: [...], col3: [...] }
    setColumnas,
    handleDragStart,
    handleDrop,
  };
}


