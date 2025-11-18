import React from "react";
import './ControlPanel3.css';

function ControlPanel3({ className, currentMode, onModeChange, onDirectionChange }) {
    const isManual = currentMode === 'MANUAL';

    //조작키 이벤트 핸들링 함수
    const handlePress = (dir) => {
        if (!isManual) return; //AUTO일경우 무시
        onDirectionChange(dir);
    };

    const handleRelease = () => {
        if (!isManual) return;
        onDirectionChange("STOP");
    };

    return (
        <div className={className}>
            <h2>수동 조작키</h2>

            <div className="cp3-content">
                {/* 모드 전환 토글버튼 */}
                <div className="mode-toggle-container">
                    <button
                        className={`mode-btn ${!isManual ? 'active' : ''}`}
                        onClick={() => onModeChange("AUTO")}
                    >
                        AUTO
                    </button>
                    <button
                        className={`mode-btn ${isManual ? 'active-manual' : ''}`}
                        onClick={() => onModeChange("MANUAL")}
                    >
                        MANUAL
                    </button>
                </div>

                {/* 십자 조작버튼 */}
                <div className={`d-pad-grid ${!isManual ? 'disabled' : ''}`}>
                    {/* 전진 */}
                    <button
                        className="d-btn up"
                        onMouseDown={() => handlePress("FWD")}
                        onMouseUp={handleRelease}
                        onMouseLeave={handleRelease} //버튼 밖으로 마우스가 나가면 멈춤
                    >
                        ▲
                    </button>

                    {/* 후진 */}
                    <button
                        className="d-btn down"
                        onMouseDown={() => handlePress("BACK")}
                        onMouseUp={handleRelease}
                        onMouseLeave={handleRelease} //버튼 밖으로 마우스가 나가면 멈춤
                    >
                        ▼
                    </button>
                    {/* 좌 */}
                    <button
                        className="d-btn left"
                        onMouseDown={() => handlePress("LEFT")}
                        onMouseUp={handleRelease}
                        onMouseLeave={handleRelease} //버튼 밖으로 마우스가 나가면 멈춤
                    >
                        ◀
                    </button>

                    {/* 우 */}
                    <button
                        className="d-btn right"
                        onMouseDown={() => handlePress("RIGHT")}
                        onMouseUp={handleRelease}
                        onMouseLeave={handleRelease} //버튼 밖으로 마우스가 나가면 멈춤
                    >
                        ▶
                    </button>

                    {/* 가운데 */}
                    <div className="d-center"></div>
                </div>
                    
                {/* 현재 상태 출력 */}
                <p className="mode-status">
                    현재 : 
                    <span style={{ color: isManual ? '#67e8f9' : '#4ade80'}}>
                    {currentMode}
                    </span> 
                    모드
                </p>
            </div>
        </div>
    )

}

export default ControlPanel3;