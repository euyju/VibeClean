import React from 'react';
import './ControlPanel1.css';
import { PowerIcon } from './Icons';
import { Switch } from './Switch';

function ControlPanel1({ className, isPowerOn, setIsPowerOn }) {
    return(
        <div className={className}>
            <h2>시스템 전원</h2>

            <div className="content-wrapper">
                {/* Power Icon */}
                <div className={`power-icon-container ${isPowerOn ? 'active' : ''}`}>
                    <PowerIcon className="power-icon" strokeWidth={2} />
                </div>
                {/* Status Text */}
                <div className="status-text-container">
                    <p className="status-label">전원 상태</p>
                    <p className={`status-text ${isPowerOn ? 'active' : ''}`}>
                        {isPowerOn ? 'ON' : 'OFF'}
                    </p>
                </div>

                {/* Switch */}
                <div className="switch-container">
                    <span>OFF</span>
                    <Switch 
                        checked={isPowerOn} 
                        onCheckedChange={setIsPowerOn}
                    />
                    <span>ON</span>
                </div>
            </div>
        </div>
    );
}

export default ControlPanel1;