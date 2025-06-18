//=============================================
// HOOK DRAG & DROP 
//=============================================

import { useState } from 'react';
import api from '../../../api';

export default function useDragDropSesiones({ category, onMoveSuccess }) {
  const [dragData, setDragData] = useState(null);

  const handleDragStart = (e, itemName, originFilter) => {
    e.dataTransfer.setData("text/plain", "dummy"); 
    setDragData({ itemName, originFilter });
  };

  const handleDragOver = (e) => {
    e.preventDefault(); // Permite el drop
  };

  const handleDrop = async (e, targetFilter) => {
    e.preventDefault();
    if (!dragData) return;

    const { itemName, originFilter } = dragData;

    // Si se suelta en la misma lista, no hacemos nada
    if (originFilter === targetFilter) {
      setDragData(null);
      return;
    }

    // Mostramos la confirmación
    const confirmar = window.confirm(
      `¿Desea confirmar el cambio de '${itemName}' de la sesión '${originFilter}' a '${targetFilter}'?`
    );

    if (!confirmar) {
      // El usuario canceló la operación
      setDragData(null);
      return;
    }

    // Usuario confirmó → llamamos al endpoint
    try {
      await api.put('/sesiones/move-item', {
        category,
        fromFilter: originFilter,
        toFilter: targetFilter,
        itemName,
      });

      // Si la llamada tuvo éxito, refrescamos la data en el componente padre
      if (onMoveSuccess) onMoveSuccess();
    } catch (error) {
      console.error('Error al mover item:', error);
      // Podrías mostrar un alert o toast con el error
    } finally {
      setDragData(null);
    }
  };

  return {
    handleDragStart,
    handleDragOver,
    handleDrop,
  };
}
