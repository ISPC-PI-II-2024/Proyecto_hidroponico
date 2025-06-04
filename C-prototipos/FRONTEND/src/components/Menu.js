/*********************************************************/
/********************* NAV MENU **************************/
/*********************************************************/

import React from 'react';
import { Link } from 'react-router-dom';
import DarkModeToggle from './subComponents/4-modoOscuro';

function Menu() {
  return (
    <nav className="menu">
      <div className="menu-inner">
        <ul className="menu-links">
          <li>
            <Link to="/ispc">ISPC</Link>
          </li>
        </ul>
      <DarkModeToggle />
      </div>
    </nav>
  );
}

export default Menu;
