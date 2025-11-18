import React, { useEffect, useState, /*useEffect*/ } from 'react';
//import { mockRobotStatus } from './mockData';
import axios from 'axios';

//import logoImage from './1.png'
import styles from './App.module.css';

import StatusDisplay from './components/StatusDisplay';
import ControlPanel1 from './components/ControlPanel1';
import ControlPanel2 from './components/ControlPanel2';
import PathMap from './components/PathMap';

/*
const MAP_WIDTH = 300;
const MAP_HEIGHT = 300;
const STEP_SIZE = 5; // 점 한번 찍히는 거리
*/
const offlineState = {
  power: "OFF",
  currentFloor: null,
  fanSpeed: 0,
  pathHistory: []
}

function App() {
  const [robotStatus, setRobotStatus] = useState(offlineState); //상태 정보 받아오기
  const [isPowerOn, setIsPowerOn] = useState(false);            //전원 상태 설정
  const [fanSpeed, setFanSpeed] = useState(0);                  //팬 속도 설정

  useEffect(() => {
    //BE로부터 data를 받아오는 함수
    const fetchData = async () => {
      /*
      console.log("가짜 API 호출 시도...");
      
      // 가짜 비동기 함수 (0.5초 딜레이)
      const fakeApiCall = () => new Promise(resolve => {
        setTimeout(() => {
          const randomSpeed = Math.floor(Math.random() * 4); // 0~3단
          const newPoint = { x: Math.random() * 300, y: Math.random() * 300 };

          resolve({
            data: {
              power: "ON",
              currentFloor: Math.random() > 0.5 ? "Carpet" : "Hard",
              fanSpeed: randomSpeed,
              pathHistory: [newPoint]
            }
          });
        }, 500);
      });
      */
      try {
        const response =  await axios.get('/api/robot/status'); //await fakeApiCall();
        
        //수신 데이터로 status 설정
        setRobotStatus(response.data);
        setIsPowerOn(response.data.power === "ON");
        setFanSpeed(response.data.fanSpeed);

        console.log("API 1 데이터 수신:", response.data); //수신데이터 확인용
      } catch (error) {
          setRobotStatus(offlineState); //수신 실패시 디폴트값으로 설정

          setIsPowerOn(false);
          setFanSpeed(0);

          console.error("API 1 수신 실패", error.message);
      }
    };
    fetchData(); //페이지 로딩 시 1회 호출

    const intervalId = setInterval(fetchData, 2000); //2초에 한번씩 fetchData 반복호출

    return () => clearInterval(intervalId); //컴포넌트 사라졌을때 타이머 정리
  }, []);

  const handleFanChanege = async (newSpeed) => {
    setFanSpeed(newSpeed);

    try {
      await axios.post('/api/manual/speed', { fanSpeed: newSpeed });
      console.log(`API 4: 팬 속도 ${newSpeed}단 요청 전송`);
    } catch (error) {
      console.error("API 4번 요청 실패: ", error);
    }
  };

  const handlePowerChange = async (newPowerState) => {
    setIsPowerOn(newPowerState);

    const powerString = newPowerState ? "ON" : "OFF";
    try {
      await axios.post('/api/manual/power', { power: powerString });
      console.log(`API 5번: 전원 ${powerString} 요청 전송`);
    } catch (error) {
      console.error("API 5번 요청 실패 :", error);
    }
  };

  return (
    <div className={styles.dashboardWrapper}>
      <div className={styles.gridContainer}>
        <header className={styles.header}>
            {/*<a className={styles.logo}><img src={logoImage} alt="VibeClean Logo"></img></a>*/}
            <h1>VibeClean Dashboard</h1>
        </header>

        <StatusDisplay 
          className={`${styles.panel} ${styles.status}`} 
          status={robotStatus} 
          isPowerOn={isPowerOn}
          fanSpeed={fanSpeed}
        />
        
        <ControlPanel1 
          className={`${styles.panel} ${styles.controls1}`}
          isPowerOn={isPowerOn}
          setIsPowerOn={handlePowerChange}
        />

        <ControlPanel2
          className={`${styles.panel} ${styles.controls2}`} 
          fanSpeed={fanSpeed}
          setFanSpeed={handleFanChanege}
        />
        
        <PathMap 
          className={`${styles.panel} ${styles.map}`}
          path={robotStatus.pathHistory} 
        />

        <div className={`${styles.panel} ${styles.graph1}`}>
          <h2>(그래프 1)</h2>
          <p>노면 감지 통계</p>
        </div>

        <div className={`${styles.panel} ${styles.graph2}`}>
          <h2>(그래프 2)</h2>
          <p>(API 3 ?)</p>
        </div>
     </div>
    </div>
  );
}

export default App;
