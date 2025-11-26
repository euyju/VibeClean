import React from "react";
import './Graph2.css';

function Graph2 ({ className, data }) {
    //그래프의 Y축 범위 설정
    const MAX_VAL = 50;
    const MIN_VAL = -50;
    const RANGE = MAX_VAL - MIN_VAL;

    //받은 데이터를 SVG 폴리라인 좌표 문자열로 변환하는 함수
    const getPoints = (key) => {
        if (!data || data.length === 0) return "";

        return data.map((item, index) => {
            const x = (index / (data.length - 1)) * 100; //X좌표: 전체 너비를 데이터의 개수로 나눔
            const value = Math.max(MIN_VAL, Math.min(MAX_VAL, item[key]));
            const y = 100 - ((value - MIN_VAL) / RANGE) * 100;

            return `${x}, ${y}`;
        }).join(" "); // "(0, 50) (10,20) ... 형태의 문자열 반환"
    };

    const latest = data[data.length - 1] || { x:0, y:0, z:0 };

    return (
        <div className={`${className} graph2-container`}>
            <h2>실시간 센서 데이터</h2>

            <div className="charts-wrapper">
                {/* X축 그래프 */}
                <div className="chart-row">
                    <div className="chart-label x-label">X: {latest.x.toFixed(1)}</div>
                    <div className="svg-container">
                        <svg viewBox="0 0 100 100" preserveAspectRatio="none">
                            {/* 기준선 (영점) */}
                            <line x1="0" y1="50" x2="100" y2="50" stroke="rgba(255, 255, 255, 0.1)" strokeWidth="1" />
                            {/* 그래프 */}
                            <polyline
                                points={getPoints('x')}
                                fill="none"
                                stroke="#F87171"
                                strokeWidth="2"
                                vectorEffect="non-scaling-stroke"
                            />
                        </svg>
                    </div>
                </div>

                {/* Y축 그래프 */}
                <div className="chart-row">
                    <div className="chart-label y-label">Y: {latest.y.toFixed(1)}</div>
                    <div className="svg-container">
                        <svg viewBox="0 0 100 100" preserveAspectRatio="none">
                            {/* 기준선 (영점) */}
                            <line x1="0" y1="50" x2="100" y2="50" stroke="rgba(255, 255, 255, 0.1)" strokeWidth="1" />
                            {/* 그래프 */}
                            <polyline
                                points={getPoints('y')}
                                fill="none"
                                stroke="#4ADE80"
                                strokeWidth="2"
                                vectorEffect="non-scaling-stroke"
                            />
                        </svg>
                    </div>
                </div>

                {/* Z축 그래프 */}
                <div className="chart-row">
                    <div className="chart-label z-label">Z: {latest.z.toFixed(1)}</div>
                    <div className="svg-container">
                        <svg viewBox="0 0 100 100" preserveAspectRatio="none">
                            {/* 기준선 (영점) */}
                            <line x1="0" y1="50" x2="100" y2="50" stroke="rgba(255, 255, 255, 0.1)" strokeWidth="1" />
                            {/* 그래프 */}
                            <polyline
                                points={getPoints('z')}
                                fill="none"
                                stroke="#60A5FA"
                                strokeWidth="2"
                                vectorEffect="non-scaling-stroke"
                            />
                        </svg>
                    </div>
                </div>
            </div>
        </div>
    );
}

export default Graph2;