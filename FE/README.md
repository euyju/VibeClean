# FE (Frontend)

VibeClean 프로젝트의 프론트엔드 대시보드 디렉토리입니다.

## 👥 담당자

  * **변정섭** - Frontend (React) 개발, UI/UX 디자인

## 📁 폴더 구조

```
FE/
├── public/             # 정적 파일
│   ├── index.html      # React 앱의 진입점 HTML
│   └── logo.png        # 로고 이미지
├── src/
│   ├── components/     # 재사용 가능한 UI 컴포넌트
│   │   ├── StatusDisplay.jsx     # 실시간 상태창
│   │   ├── ControlPanel1.jsx     # 시스템 전원 패널
│   │   ├── ControlPanel2.jsx     # 팬 속도 조절 패널
│   │   ├── PathMap.jsx         # 주행 경로 맵
│   │   ├── Icons.jsx           # SVG 아이콘 컴포넌트
│   │   └── Switch.jsx          # 토글 스위치 컴포넌트
│   ├── App.jsx         # 메인 애플리케이션 (레이아웃, 핵심 로직)
│   ├── App.module.css  # App 전용 CSS 모듈 (그리드 레이아웃)
│   ├── index.js        # React DOM 렌더링 진입점
│   ├── index.css       # 전역 CSS 스타일 (배경, 폰트 등)
│   └── (각 컴포넌트별 .css) # 각 컴포넌트 세부 스타일
├── .gitignore          # Git 무시 파일 목록
├── package.json        # 프로젝트 정보 및 의존성 목록
├── package-lock.json   # 의존성 버전 고정
└── README.md           # 이 파일
```

## 💻 개발 환경 및 기술 스택

  * **Framework**: React.js
  * **Language**: JavaScript (ES6+)
  * **API Communication**: axios
  * **Styling**: CSS Modules, 순수 CSS (Flexbox, Grid)
  * **Package Manager**: npm
  * **Runtime**: Node.js (v16+)

## 🚀 주요 기능

  * **실시간 대시보드**: 백엔드 API를 주기적으로 폴링(polling)하여 로봇의 상태(전원, 팬 속도, 바닥 상태, 경로)를 실시간으로 시각화합니다.
  * **양방향 수동 제어**: 사용자가 UI(스위치, 버튼)를 조작하면, `axios`를 통해 BE 서버로 제어 명령(API 4, 5)을 전송합니다.
  * **컴포넌트 기반 UI**: 대시보드의 각 패널(`StatusDisplay`, `ControlPanel` 등)을 재사용 가능한 컴포넌트로 모듈화하여 관리합니다.
  * **동적 스타일링**: 로봇의 상태(`isPowerOn`, `fanSpeed`)에 따라 아이콘, 버튼, 인디케이터의 스타일이 동적으로 변경됩니다.

-----

## 🛠️ 빌드 및 실행 가이드

### 1\. 개발 환경 설정

  * **필수 요구사항**: [Node.js](https://nodejs.org/) (v16 이상 권장)
  * **IDE**: VS Code (권장)

### 2\. 프로젝트 클론 및 의존성 설치

```bash
# VibeClean 프로젝트의 최상위 폴더에서 시작
cd VibeClean/FE

# 의존성 패키지 설치
npm install
```

### 3\. 애플리케이션 실행 (개발 모드)

```bash
# FE 프로젝트 폴더(VibeClean/FE)에서 실행
npm start
```

서버가 정상적으로 시작되면 `http://localhost:3000` 에서 대시보드에 접근 가능합니다.

-----

## ⚠️ **중요: 실행 전 필수 확인 사항**

본 FE 프로젝트는 백엔드(BE) 서버와 실시간으로 통신해야 정상 작동합니다.

1.  **BE 서버 실행**: 테스트 전에 `VibeClean/BE` 프로젝트가 `http://localhost:8080` (또는 지정된 포트)에서 실행 중이어야 합니다.
2.  **CORS 설정**: BE 서버에 **`http://localhost:3000`** 주소의 요청을 허용하는 **CORS (Cross-Origin Resource Sharing) 설정**이 반드시 필요합니다. 이 설정이 없으면 FE가 BE로 보내는 모든 API 요청이 브라우저 보안 정책에 의해 차단됩니다.

-----

## 🤝 연동 API 목록 (BE 제공)

FE 대시보드는 다음의 주요 API를 호출하여 작동합니다.

### **API 1: 실시간 상태 조회 (핵심)**

  * `GET /api/robot/status`
  * **설명**: 2초마다 주기적으로 호출하여 로봇의 모든 실시간 상태(전원, 바닥, 팬, 경로)를 받아와 `App.jsx`의 메인 `state`에 저장합니다.

### **API 5: 전원 상태 수동 설정**

  * `POST /api/manual/power`
  * **설명**: '시스템 전원' 패널의 스위치 조작 시 호출됩니다. `{"power": "ON"}` 또는 `{"power": "OFF"}` 데이터를 전송합니다.

### **API 4: 팬 속도 수동 설정**

  * `POST /api/manual/speed`
  * **설명**: '팬 속도 조절' 패널의 버튼 클릭 시 호출됩니다. `{"fanSpeed": 2}`와 같이 0\~3 사이의 정수를 전송합니다.

-----

## 🧩 주요 컴포넌트 설명

### `App.jsx` (중앙 관제실)

  * **역할**: 프로젝트의 '두뇌'입니다. 모든 핵심 상태(`robotStatus`, `isPowerOn`, `fanSpeed`)를 `useState`로 관리합니다.
  * **핵심 로직**:
    1.  `useEffect`를 사용해 API 1번을 2초마다 주기적으로 호출(폴링)하고, `robotStatus` 상태를 업데이트합니다.
    2.  `handlePowerChange`, `handleFanChange` 함수를 정의하여 API 4, 5번을 `axios.post`로 전송합니다.
    3.  모든 자식 컴포넌트에게 필요한 `state`와 '핸들러 함수'를 `props`로 전달합니다.

### `StatusDisplay.jsx` (전광판)

  * **역할**: `App.jsx`로부터 받은 `props`(status, isPowerOn, fanSpeed)를 화면에 표시합니다.
  * **특징**: 자체적인 로직이나 상태가 없는 '멍청한' 컴포넌트로, 부모가 주는 데이터가 바뀌면 자동으로 리렌더링됩니다.

### `ControlPanel1.jsx` (시스템 전원)

  * **역할**: 전원 스위치 UI를 제공합니다.
  * **특징**: `App.jsx`로부터 `isPowerOn`(현재 상태)과 `handlePowerChange`(변경 함수)를 `props`로 받아 `Switch` 컴포넌트와 연결합니다.

### `ControlPanel2.jsx` (팬 속도 조절)

  * **역할**: 팬 속도 조절 UI를 제공합니다.
  * **특징**: `App.jsx`로부터 `fanSpeed`(현재 속도)와 `handleFanChange`(변경 함수)를 `props`로 받아 버튼과 인디케이터를 동적으로 렌더링합니다.

### `PathMap.jsx` (지도)

  * **역할**: `App.jsx`로부터 `pathHistory` 배열을 `props`로 받아 `.map()`을 통해 화면에 점을 그립니다.
  * **특징**: API 1번이 갱신될 때마다 새로운 `pathHistory` 배열을 받아, 기존 점들을 포함한 모든 경로를 다시 그립니다.

-----

## 📚 참고 자료

  * [React 공식 문서](https://reactjs.org/)
  * [axios 공식 문서](https://axios-http.com/)

## PR 가이드

1.  새로운 기능 개발 시 `develop` 브랜치에서 별도 브랜치 생성 (`feature/fe-new-feature`)
2.  코드 변경 후 로컬 테스트 완료 확인 (`npm start`)
3.  커밋 메시지 명확하게 작성 (`git commit -m "Feat: 팬 속도 조절 패널(CP2) UI 구현"`)
4.  `develop` 브랜치로 Pull Request 생성 및 리뷰 요청
