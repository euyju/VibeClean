import React from "react";
import './Graph1.css';

function Graph1({ className, data}) {
    //시간 표기 형식 ex ) 00 : 00 변환 함수 formatTime
    const formatTime = (seconds) => {
        const mins = Math.floor(seconds / 60);
        const secs = seconds % 60;
        //padStart를 사용해 10보다 작은 숫자 앞에 0을 붙여줌
        return `${mins.toString().padStart(2, '0')}:${secs.toString().padStart(2, 0)}`;
    }

    const floorTypes = [
        { key: 'Carpet', label: '카펫', color: '#F59E0B' },
        { key: 'Hard', label: '일반 바닥', color: '#3B82F6' },
        { key: 'Dusty', label: '먼지 구간', color: '#EF4444' },
    ];

    return (
        <div className={className}>
            <div className="graph1-header">
                <h2>노면 감지 통계</h2>
                <div className="runtime-badge">
                    <span className="time-label">총 시간</span>
                    <span className="time-value">{formatTime(data.totalRuntimeSeconds)}</span>
                </div>
            </div>

            <div className="chart-container">
                {floorTypes.map((type) => {
                    const percent = data.floorDistribution[type.key] || 0;

                    return (
                        <div key={type.key} className="bar-row">
                            <div className="bar-info">
                                <span className="bar-label">{type.label}</span>
                                <span className="bar-percent">{percent.toFixed(1)}%</span>
                            </div>

                            <div className="progress-bg">
                                <div
                                    className="progress-fill"
                                    style={{
                                        width: `${percent}%`,
                                        backgroundColor: type.color
                                    }}
                                >
                                </div>
                            </div>
                        </div>
                    );
                })}
            </div>
        </div>
    );
}

export default Graph1;