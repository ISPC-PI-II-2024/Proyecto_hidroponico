//=============================================
// HOOK GESTOR DE KPIs 
//=============================================

import { useState, useEffect, useCallback } from 'react';
import axios from 'axios';

/**
 * Hook para traer datos históricos de KPI de un dispositivo.
 *
 * @param {string} deviceId                         Id del dispositivo. A modificar cuando tenga la variable correcta
 * @param {string[]} metrics                        Claves de métricas a traer.
 * @param {number} [rangeDays=5]                    Días hacia atrás para el histórico (máx. 5 días).
 * @param {number|null} [refreshInterval=null]  
 *        Si != null, hará polling cada X milisegundos.
 *
 * @returns {{
 *   data: Array<{ key: string, data: Array<{ timestamp: string, value: number }> }>,
 *   loading: boolean,
 *   error: Error | null
 * }}
 */
export default function useKpiData(
  deviceId,
  metrics,
  rangeDays = 5,
  refreshInterval = null
) {
  const [data, setData]     = useState([]);
  const [loading, setLoading] = useState(false);
  const [error, setError]     = useState(null);

  const fetchData = useCallback(async () => {
    if (!deviceId || !metrics?.length) return;

    setLoading(true);
    setError(null);
    try {
      const now = new Date();
      const fromDate = new Date(now.getTime() - rangeDays * 24 * 60 * 60 * 1000);

      const params = {
        deviceId,
        metrics: metrics.join(','),
        from:  fromDate.toISOString(),
        to:    now.toISOString(),
      };

      const resp = await axios.get('/api/kpi', { params });
      const payload = resp.data;

      let series;
      // Si la API ya retorna { series: [ { key, data:[{timestamp,value},…] } ] }
      if (Array.isArray(payload.series)) {
        series = payload.series;
      } else if (Array.isArray(payload)) {
        // Supongamos payload = [{ metric, timestamp, value }, …]
        const grouped = metrics.reduce((acc, key) => {
          acc[key] = [];
          return acc;
        }, {});
        payload.forEach(({ metric, timestamp, value }) => {
          if (grouped[metric]) {
            grouped[metric].push({ timestamp, value });
          }
        });
        series = metrics.map((key) => ({
          key,
          data: grouped[key],
        }));
      } else {
        // otro formato no soportado
        throw new Error('Formato de datos de KPI inesperado');
      }

      setData(series);
    } catch (err) {
      console.error('Error en useKpiData:', err);
      setError(err);
    } finally {
      setLoading(false);
    }
  }, [deviceId, metrics, rangeDays]);

  useEffect(() => {
    fetchData();

    let timerId = null;
    if (refreshInterval && refreshInterval > 0) {
      timerId = setInterval(fetchData, refreshInterval);
    }
    return () => {
      if (timerId) clearInterval(timerId);
    };
  }, [fetchData, refreshInterval]);

  return { data, loading, error };
}
