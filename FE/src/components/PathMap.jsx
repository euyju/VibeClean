import React from 'react';
import './PathMap.css';

function PathMap({ path, className }) {
  return (
    <div className={`${className} pathmap-container`}>
      <h2>주행 경로</h2>

      <div className="map-canvas">
        {Array.isArray(path) && path.map((point, index) => (
          <div 
            key={index}
            className="pin"
            style={{
              left: `${point.x}px`, 
              top: `${point.y}px`
            }}
          > 
          </div>
        ))}
      </div>
    </div>
  );
}

export default PathMap;