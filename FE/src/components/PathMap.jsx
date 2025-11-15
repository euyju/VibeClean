import React from 'react';
import './PathMap.css';

function PathMap({ path, className }) {
  return (
    <div className={className} style={{ position: 'relative'}}>
      <h2>주행 경로</h2>

      {Array.isArray(path) && path.map((point, index) => (
        <div 
          key={index}
          className="pin"
          style={{
            left: `${point.x}px`, 
            top: `${point.y}px`
          }}
        ></div>
      ))}
    </div>
  );
}

export default PathMap;