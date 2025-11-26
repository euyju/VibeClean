import React from 'react';
import './ControlPanel2.css';
import { WindIcon } from './Icons';

function ControlPanel2({ className, fanSpeed, setFanSpeed }) {
    const speeds = [0, 1, 2, 3];
    const indicatorSpeeds = [1, 2, 3];

    return(
        <div className={className}>
            <h2>팬 속도 조절</h2>

            <div className="content-wrapper">
                {/* 팬 아이콘 (회전 반영) */}
                <div className="relative">
                    <div className={`fan-icon-container ${fanSpeed > 0 ? 'active' : '' }`}>
                        <WindIcon 
                        className={`fan-icon speed-${fanSpeed}`} 
                        strokeWidth={2} 
                        />
                    </div>
                </div>

                {/* 상태 구문 */}
                <div className="status-text-container">
                    <p className="status-label1">팬 속도</p>
                    <p className="status-label2">{fanSpeed}단</p>
                </div>

                {/* 속도 조절 버튼 */}
                <div className="speed-button-wrapper">
                    {speeds.map((speed) => (
                        <button
                        key={speed}
                        onClick={() => setFanSpeed(speed)}
                        className={`speed-button ${fanSpeed === speed ? 'active' : ''}`}
                        >
                        {speed}
                        </button>
                    ))}
                </div>
                {/* 속도 상태 표현 바 */}
                <div className="speed-bar-wrapper">
                    {indicatorSpeeds.map((speed) => (
                        <div
                        key={speed}
                        className={`speed-bar ${speed <= fanSpeed ? 'active' : ''}`}
                        />
                    ))}
                </div>
            </div>
        </div>
    );
}

export default ControlPanel2;