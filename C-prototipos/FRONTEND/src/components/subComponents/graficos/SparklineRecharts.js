//=============================================
// Sparkline de línea usando Recharts
//=============================================

import React from 'react';
import {
  LineChart,
  Line,
  ResponsiveContainer,
  Tooltip,
  XAxis,
  YAxis,
} from 'recharts';

/**
 * Mini–gráfico de línea (sparkline) usando Recharts.
 *
 * Props:
 * - data: Array<{ timestamp: string, value: number }>
 * - color: string (hex o rgb) – color de la línea
 * - height: number – altura en pixeles del contenedor
 */
export default function SparklineRecharts({
  data,
  color = '#4ade80',  // verde por defecto
  height = 50,
}) {
  return (
    <ResponsiveContainer width="100%" height={height}>
      <LineChart data={data}>
        {/* Ocultamos ejes para estilo “sparkline” */}
        <XAxis dataKey="timestamp" hide />
        <YAxis hide domain={['auto', 'auto']} />

        {/* Línea principal */}
        <Line
          type="monotone"
          dataKey="value"
          stroke={color}
          strokeWidth={2}
          dot={false}
          isAnimationActive={false}
        />

        {/* Tooltip opcional: descomenta si quieres hover */}
        {/*
        <Tooltip
          wrapperStyle={{ outline: 'none', fontSize: '0.8rem' }}
          labelFormatter={(ts) =>
            new Date(ts).toLocaleString('es-AR', {
              hour: '2-digit',
              minute: '2-digit',
            })
          }
          formatter={(val) => [`${val}`, '']}
        />
        */}
      </LineChart>
    </ResponsiveContainer>
  );
}
