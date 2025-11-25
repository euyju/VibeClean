import React from 'react';
import styles from './StatusDisplay.module.css';

import { PowerIcon, LayersIcon, WindIcon } from './Icons';

function StatusDisplay({ status, className, isPowerOn, fanSpeed }){

    //바닥 상태에 따른 클래스 선택 함수
    const getFloorClass = (floor) => {
        switch (floor) {
            case 'Hard': return styles.floorHard;
            case 'Carpet': return styles.floorCarpet;
            case 'Dusty': return styles.floorDusty;
            default: return ''; //디폴트 상태 (N/A)
        }
    };

    const powerClass = isPowerOn ? styles.powerOn : '';
    const fanClass = fanSpeed > 0 ? styles.fanActive : '';

    return(
        <div className={`${className} ${styles.container}`}>
            <h2 className={styles.title}>실시간 상태창</h2>

            <div className={styles.statusGrid}>
                {/* 전원 카드*/}
                <div className={`${styles.statusCard} ${powerClass}`}> {/* powerClass 애니메이션 온오프 */}
                    <PowerIcon className={styles.icon} />
                    <span className={styles.label}>전원</span>
                    <span className={styles.value}>{isPowerOn ? 'ON' : 'OFF'}</span>
                </div>
                {/* 바닥 상태 카드 */}
                <div className={`${styles.statusCard} ${getFloorClass(status.currentFloor)}`}> {/* getFloorClass 이용 */}
                    <LayersIcon className={styles.icon} />
                    <span className={styles.label}>바닥 상태</span>
                    <span className={styles.value}>{status.currentFloor || 'N/A'}</span>
                </div>  
                {/* 팬 속도 카드 */}
                <div className={`${styles.statusCard} ${fanClass}`}> {/* fanClass로 애니메이션 온오프 */}
                    <WindIcon className={styles.icon} />
                    <span className={styles.label}>팬 속도</span>
                    <span className={styles.value}>{fanSpeed}단</span>
                </div>  
            </div>
        </div>
    );
}

export default StatusDisplay;