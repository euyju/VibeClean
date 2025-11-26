import React, { useEffect, useState, /*useEffect*/ } from 'react';
import axios from 'axios';

//import logoImage from './1.png'
import styles from './App.module.css';

import StatusDisplay from './components/StatusDisplay';
import ControlPanel1 from './components/ControlPanel1';
import ControlPanel2 from './components/ControlPanel2';
import ControlPanel3 from './components/ControlPanel3';
import PathMap from './components/PathMap';
import Graph1 from './components/Graph1';
import Graph2 from './components/Graph2';

//HW OFF시 기본 상태
const offlineState = {
  power: "OFF",
  currentFloor: null,
  mode: "AUTO",
  fanSpeed: 0,
  pathHistory: []
}

//1번 그래프(노면)용 초기 상태
const initialStats = {
  totalRuntimeSeconds: 0,
  floorDistribution: {
    Carpet: 0,
    Hard: 0,
    Dusty: 0
  }
};

function App() {
  const [robotStatus, setRobotStatus] = useState(offlineState); //상태 정보 받아오기
  const [isPowerOn, setIsPowerOn] = useState(false);            //전원 상태 설정
  const [fanSpeed, setFanSpeed] = useState(0);                  //팬 속도 설정
  const [floorStats, setFloorStats] = useState(initialStats);   //그래프 1번 설정
  const [sensorHistory, setSensorHistory] = useState(           //그래프 2번 설정
    Array(20).fill({ x:0, y:0, z:0 })
  );  

  //API 1번(데이터 받아오기) 폴링 useEffcet
  useEffect(() => {
    //BE로부터 data를 받아오는 함수
    const fetchData = async () => {
      try {
        const response = await axios.get('/api/robot/status');
        
        //수신 데이터로 status 설정
        setRobotStatus(response.data);
        setIsPowerOn(response.data.power === "ON");
        setFanSpeed(response.data.fanSpeed);

        console.log("API 1번 데이터 수신: ", response.data);
      } catch (error) {
        setRobotStatus(offlineState); //수신 실패시 디폴트값 설정
        setIsPowerOn(false);
        setFanSpeed(0);

        console.error("API 1번 수신 실패: ", error.message);
      }
    };
    fetchData(); //페이지 로딩하자마자 한번 바로 호출

    const intervalId = setInterval(fetchData, 1000);
    return () => clearInterval(intervalId);
  }, []); // []를 비워두어 useEffect가 처음 마운트시 딱 한번만 실행되게 함

  //API 2번(노면 감지 통계) 폴링 useEffect
  useEffect(() => {
    const fetchStats = async () => {
      try {
        //DB로부터 API 호출
        const response = await axios.get('/api/robot/stats');
        setFloorStats(response.data);

        console.log("API 2번 데이터 수신: ", response.data);
      } catch (error) {
        console.error("API 2번: 노면 감지 통계 데이터 호출 실패", error);
        setFloorStats(initialStats);
      }
    };

    fetchStats();
    const interval = setInterval(fetchStats, 1000); //1초에 한번씩 데이터 갱신
    return () => clearInterval(interval); // 타이머 정리
  }, []); // []를 비워두어 useEffect가 처음 마운트시 딱 한번만 실행되게 함

  //API 3번(센서 데이터) 폴링 useEffect
  useEffect (() => {
    const fetchSensorData = async () => {
      try {
        const response = await axios.get('/api/robot/sensor');
        const newData = response.data;

        //센서 데이터 누적 로직 (Queue FIFO 이용)
        setSensorHistory(prevHistory => {
          const newHistory = [...prevHistory, newData]; //기존 배열을 복사하고, 새로운 데이터를 바로 뒤에 붙임
          
          //데이터 20개 이상 누적시 가장 오래된 데이터(앞쪽) 자르기
          if (newHistory.length > 20) {
            return newHistory.slice(newHistory.length - 20);
          }

          console.log("API 3번 데이터 수신: ", newData);
          
          return newHistory;
        });
      } catch (error) {
        console.error("API 3번: 센서 테이터 호출 실패: ", error);
        setSensorHistory(prev => [...prev.slice(1), { x:0, y:0, z:0 }]) //에러시 0.0 삽입 (그래프 끊김 없이)
      }
    };

    fetchSensorData();
    const interval = setInterval(fetchSensorData, 1000); //1초에 한번 갱신
    return () => clearInterval(interval);
  }, []);

  //팬 속도 변경 요청 핸들러
  const handleFanChange = async (newSpeed) => {
    setFanSpeed(newSpeed);

    try {
      await axios.post('/api/manual/speed', { fanSpeed: newSpeed });
      console.log(`API 4: 팬 속도 ${newSpeed}단 요청 전송`);
    } catch (error) {
      console.error("API 4번 요청 실패: ", error);
    }
  };
  
  //전원 상태 변경 요청 핸들러
  const handlePowerChange = async (newPowerState) => {
    setIsPowerOn(newPowerState);

    const powerString = newPowerState ? "ON" : "OFF";
    try {
      await axios.post('/api/manual/power', { power: powerString });
      console.log(`API 5번: 전원 ${powerString} 요청 전송`);
    } catch (error) {
      console.error("API 5번 요청 실패 : ", error);
    }
  };
  
  //주행 모드 변경 요청 핸들러
  const handleModeChange = async (newMode) => {
    setRobotStatus(prev => ({ ...prev, mode: newMode})); //API 1번에서 받아온 데이터 기반으로 재설정

    try {
      await axios.post('/api/manual/mode', { mode: newMode });
      console.log(`API 6번: 주행 모드 변경 요청 -> ${newMode}`);
    } catch (error) {
      console.log("API 6 요청 실패 : ", error);
    }
  };

  //방향 조작 제어 핸들러 (누를 때, 뗄 때)
  const handleDirectionChange = async (direction) => {
    if (robotStatus.mode !== 'MANUAL') return; //현 상태(mode)가 MANUAL일때만 동작

    try {
      await axios.post('/api/manual/direction', { direction: direction });
      console.log(`API 7번: 방향 전송 -> ${direction}`);
    } catch (error) {
      console.log("API 7번 요청 실패: ", error);
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
          setFanSpeed={handleFanChange} //팬 속도 변경 함수 전달
        />

        <ControlPanel3
          className={`${styles.panel} ${styles.controls3}`}
          currentMode={robotStatus.mode || "AUTO"} //현재 모드 전달
          onModeChange={handleModeChange} //모드 변경 함수 전달
          onDirectionChange={handleDirectionChange} //방향 제어 함수 전달
        />
        
        <PathMap 
          className={`${styles.panel} ${styles.map}`}
          path={robotStatus.pathHistory} 
        />

        <Graph1
          className={`${styles.panel} ${styles.graph1}`}
          data={floorStats}
        />

        <Graph2
          className={`${styles.panel} ${styles.graph2}`}
          data={sensorHistory}
        />

     </div>
    </div>
  );
}

export default App;
