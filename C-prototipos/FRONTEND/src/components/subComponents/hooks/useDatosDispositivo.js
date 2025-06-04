//=============================================
// HOOK VISTA GRAL DISPOSITIVO
//=============================================

import { useState, useEffect, useMemo } from 'react';
import axios from 'axios';

export default function useDeviceOverview(deviceId) {
  const [deviceName, setDeviceName] = useState('');
  const [status, setStatus]       = useState('ok');      // 'ok' | 'alarm' | 'error'
  const [metrics, setMetrics]     = useState([]);        // [{ key, label, value, unit }, ...]

  useEffect(() => {
    if (!deviceId) return;

    const load = async () => {
      try {
        const res = await axios.get(`/api/devices/${deviceId}/overview`);
        const { name, status: st, variables } = res.data;
        setDeviceName(name);
        setStatus(st);
        setMetrics(variables);
      } catch (err) {
        console.error('Error cargando overview de dispositivo', err);
        setStatus('error');
      }
    };

    load();
  }, [deviceId]);

  // Clase de color para el indicador de estado
  const statusClass = useMemo(() => {
    switch (status) {
      case 'ok':    return 'bg-green-500';
      case 'alarm': return 'bg-yellow-400';
      case 'error': return 'bg-red-500';
      default:      return 'bg-gray-400';
    }
  }, [status]);

  return { deviceName, status, statusClass, metrics };
}
