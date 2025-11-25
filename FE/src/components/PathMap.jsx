import React from 'react';
import './PathMap.css';

function PathMap({ path, className }) {
  // 공간 범위 설정
  const MAX_WIDTH = 240; //가로
  const MAX_HEIGHT = 200; //세로

  return (
    <div className={`${className} pathmap-container`}>
      <h2>주행 경로</h2>

      <div className="map-canvas">
        {Array.isArray(path) && path.map((point, index) => {
          //로봇이 범위를 넘어가도 PathMap 밖으로 나가지 못하게
          const clampedX = Math.max(0, Math.min(point.x, MAX_WIDTH));
          const clampedY = Math.max(0, Math.min(point.y, MAX_HEIGHT));

          //백분율로 변환 (스케일링)
          const xPercent = (clampedX / MAX_WIDTH) * 100;
          const yPercent = (clampedY / MAX_HEIGHT) * 100;
          
          return (
            <div
              key={index}
              className="pin"
              style={{
                //초기위치
                right: `${xPercent}%`,
                bottom: `${yPercent}%`
              }}
            >
            </div>
          );
        })}

        {/* 스타트 마커 */}
        <div className="start-marker">Start</div>
      </div>
    </div>
  );
}

export default PathMap;