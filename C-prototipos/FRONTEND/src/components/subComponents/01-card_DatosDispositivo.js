//=============================================
// TARJETA: Dispositivo Vista General
//=============================================

import useDeviceOverview from './hooks/useVistaGralDispositivo';
import { WifiIcon } from '@heroicons/react/24/solid'; 

export default function DeviceOverviewCard({ deviceId }) {
  const { deviceName, statusClass, metrics } = useDeviceOverview(deviceId);

  return (
    <div className="relative rounded-xl p-6 bg-green-700 text-white">
      {/* Header */}
      <div className="flex items-center justify-between">
        <div className="flex items-center space-x-3">
          <h2 className="font-bold text-lg uppercase">{deviceName}</h2>
          {/* Indicador de estado */}
          <span className={`w-4 h-4 rounded-full ${statusClass}`} />
        </div>
        <div className="flex items-center space-x-2">
          <WifiIcon className="w-6 h-6" />
          <div className="w-6 h-6 rounded-full bg-gray-800 flex items-center justify-center">M</div>
          <div className="w-6 h-6 rounded-full bg-gray-800 flex items-center justify-center">L</div>
        </div>
      </div>

      {/* Lista de métricas */}
      <ul className="mt-4 space-y-2">
        {metrics.map(({ key, label, value, unit }) => (
          <li key={key} className="flex items-center">
            <span className="w-2 h-2 bg-black rounded-full mr-2" />
            <span>
              {label}: <strong>{value}{unit}</strong>
            </span>
          </li>
        ))}
      </ul>
    </div>
  );
}
