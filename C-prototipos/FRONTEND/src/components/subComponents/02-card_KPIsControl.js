//=============================================
// TARJETA: Control de KPIs
//=============================================

import React, { useState } from 'react';
import KPIControls from './03-KPIControls';
import useKpiData from './hooks/useKpiData';
import SparklineRecharts from './graficos/SparklineRecharts';

// Paleta de colores para cada métrica
const COLORS = [
  '#4ade80', // verde
  '#facc15', // amarillo
  '#f87171', // rojo claro
  '#60a5fa', // azul
  '#a78bfa', // morado
];

export default function KPICard({ devicesList, metricsList }) {
  const rangeOptions = [
    { value: 10,    label: '10 seg'  },
    { value: 30,    label: '30 seg'  },
    { value: 60,    label: '1 min'   },
    { value: 300,   label: '5 min'   },
    { value: 600,   label: '10 min'  },
    { value: 1800,  label: '30 min'  },
    { value: 3600,  label: '1 h'     },
  ];

  const [params, setParams] = useState({
    deviceId: devicesList[0]?.id,
    metrics: metricsList.map((m) => m.key),
    range: rangeOptions[2].value,
  });

  const { data, loading, error } = useKpiData(
    params.deviceId,
    params.metrics,
    5,                   // hasta 5 días de histórico
    params.range * 1000  // polling en ms (o null si no quieres refresco)
  );

  return (
    <div className="bg-green-700 text-white rounded-xl overflow-hidden">
      {/* Controles de dispositivo, métricas y rango */}
      <KPIControls
        devices={devicesList}
        metricsOptions={metricsList}
        rangeOptions={rangeOptions}
        onChange={setParams}
      />

      {/* Contenedor de gráficos */}
      <div className="p-4 space-y-6">
        {loading && <p>Cargando KPIs…</p>}
        {error && <p>Error al cargar datos</p>}
        {!loading && !error && data.length === 0 && (
          <p>No hay datos disponibles para este período.</p>
        )}
        {!loading && !error && data.map(({ key, data: points }, idx) => (
          <div key={key}>
            <div className="flex justify-between items-center mb-1">
              <h3 className="text-sm font-semibold uppercase">{key}</h3>
              {/* Valor más reciente */}
              <span className="text-lg">
                {points.length
                  ? points[points.length - 1].value.toFixed(2)
                  : 'N/A'}
              </span>
            </div>
            {/* Sparkline */}
            <SparklineRecharts
              data={points}
              color={COLORS[idx % COLORS.length]}
              height={60}
            />
          </div>
        ))}
      </div>
    </div>
  );
}
