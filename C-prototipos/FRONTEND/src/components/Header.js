/*********************************************************/
/********************** HEADER ***************************/
/*********************************************************/

import React from 'react';
import Tabs from './Tabs';

function Header() {
  return (
    <header className="header">
      <div className="tabs-container">
        <Tabs />
      </div>
    </header>
  );
}

export default Header;
