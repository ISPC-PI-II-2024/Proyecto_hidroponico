//=============================================
// Control de KPIs
//=============================================

import React, { useState, useEffect } from 'react';
import { Listbox } from '@headlessui/react';
import { SelectorIcon, CheckIcon } from '@heroicons/react/24/solid';

export default function KPIControls({
  devices = [],
  metricsOptions = [],
  rangeOptions = [],
  onChange
}) {
  const [device, setDevice]   = useState(devices[0]?.id || null);
  const [metrics, setMetrics] = useState(metricsOptions.map(m => m.key));
  const [range, setRange]     = useState(rangeOptions[0]?.value || null);

  useEffect(() => {
    onChange?.({ deviceId: device, metrics, range });
  }, [device, metrics, range, onChange]);

  const toggleMetric = (key) => {
    setMetrics(prev =>
      prev.includes(key)
        ? prev.filter(k => k !== key)
        : [...prev, key]
    );
  };

  return (
    <div className="flex flex-wrap items-center gap-4 p-4 bg-gray-100 rounded-lg">
      {/* Selector de dispositivo */}
      <div className="w-40">
        {/* ... Listbox de dispositivos ... */}
      </div>

      {/* Flags de métricas */}
      <div className="flex flex-wrap gap-2">
        {/* ... botones de métricas ... */}
      </div>

      {/* Selector de rango */}
      <div className="w-32">
        {/* ... Listbox de rango ... */}
      </div>
    </div>
  );
}