import React from 'react';
import styles from './StatusDisplay.module.css';

import { PowerIcon, LayersIcon, WindIcon } from './Icons';

function StatusDisplay({ status, className, isPowerOn, fanSpeed }){
    return(
        <div className={`${className} ${styles.container}`}>
            <h2 className={styles.title}>실시간 상태창</h2>

            <div className={styles.statusGrid}>
                {/* 전원 */}
                <div className={styles.statusCard}>
                    <PowerIcon className={styles.icon} />
                    <span className={styles.label}>전원</span>
                    <span className={styles.value}>{isPowerOn ? 'ON' : 'OFF'}</span>
                </div>
                {/* 바닥 상태 */}
                <div className={styles.statusCard}>
                    <LayersIcon className={styles.icon} />
                    <span className={styles.label}>바닥 상태</span>
                    <span className={styles.value}>{status.currentFloor || 'N/A'}</span>
                </div>  
                {/* 팬 속도 */}
                <div className={styles.statusCard}>
                    <WindIcon className={styles.icon} />
                    <span className={styles.label}>팬 속도</span>
                    <span className={styles.value}>{fanSpeed}단</span>
                </div>  
            </div>
        </div>
    );
}

export default StatusDisplay;