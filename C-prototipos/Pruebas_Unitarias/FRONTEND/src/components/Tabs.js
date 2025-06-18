/*********************************************************/
/****************** TABS POR SESIONES ********************/
/*********************************************************/

// src/components/Tabs.js
import React, { useState } from 'react';
import { useNavigate } from 'react-router-dom';

function Tabs() {
  const [activeTab, setActiveTab] = useState('pagina0');
  const navigate = useNavigate();

  const handleTabClick = (tab) => {
    setActiveTab(tab);
    navigate(`/${tab}`);
  };

  return (
    <div className="tabs">
      <button
        className={activeTab === 'pagina0' ? 'active' : ''}
        onClick={() => handleTabClick('pagina0')}
      >
        DISPOSITIVOS
      </button>
      <button
        className={activeTab === 'pagina1' ? 'active' : ''}
        onClick={() => handleTabClick('pagina1')}
      >
        NOSOTROS
      </button>
    </div>
  );
}

export default Tabs;
