# FE (Frontend)

VibeClean 프로젝트의 프론트엔드 대시보드 디렉토리입니다.

## 👥 담당자

  * **변정섭** - Frontend (React) 개발, UI/UX 디자인

## 📁 폴더 구조

```
FE/
├── public/                    # 정적 파일 보관함
│   ├── 1.ico                  # 파비콘
│   ├── 1.png                  # 로고 이미지
│   ├── index.html             # React 앱의 진입점 HTML
│   ├── manifest.json          # PWA 설정 파일
│   └── robots.txt             # 검색 엔진 설정
│
├── src/                       # React 개발 소스 코드 폴더
│   ├── 1.png                  # import용 로고 이미지
│   ├── App.js                 # 메인 컴포넌트, 상태 및 API 관리
│   ├── App.module.css         # 대시보드 전체 그리드 레이아웃 및 공통 스타일
│   ├── background1.jpg        # 배경 이미지
│   ├── index.css              # 전역 스타일 (Global CSS)
│   ├── index.js               # React 실행 진입점 (Entry Point)
│   ├── reportWebVitals.js     # 성능 측정 도구
│   │
│   └── components/            # 재사용 가능한 UI '부품' 폴더
│           ControlPanel1.css      # '시스템 전원' 패널 스타일
│           ControlPanel1.jsx      # '시스템 전원' 패널 컴포넌트
│           ControlPanel2.css      # '팬 속도 조절' 패널 스타일
│           ControlPanel2.jsx      # '팬 속도 조절' 패널 컴포넌트
│           ControlPanel3.css      # '수동 주행 조작' 패널 스타일
│           ControlPanel3.jsx      # '수동 주행 조작' 패널 컴포넌트
│           Graph1.css             # '노면 감지 통계' 그래프 스타일
│           Graph1.jsx             # '노면 감지 통계' 그래프 컴포넌트
│           Graph2.css             # '실시간 센서 데이터' 그래프 스타일
│           Graph2.jsx             # '실시간 센서 데이터' 그래프 컴포넌트
│           Icons.jsx              # SVG 아이콘 컴포넌트 모음
│           PathMap.css            # '주행 경로' 맵 스타일
│           PathMap.jsx            # '주행 경로' 맵 컴포넌트
│           StatusDisplay.jsx      # '실시간 상태창' 컴포넌트
│           StatusDisplay.module.css # StatusDisplay 전용 스타일 (Module)
│           Switch.css             # 토글 스위치 스타일
│           Switch.jsx             # 토글 스위치 컴포넌트
│
├── .gitignore                 # Git 무시 파일 목록
├── package.json               # 프로젝트 정보 및 의존성 목록
├── package-lock.json          # 의존성 버전 고정
└── README.md                  # 이 파일
```

## 💻 개발 환경 및 기술 스택

  * **Framework**: React.js
  * **Language**: JavaScript (ES6+)
  * **API Communication**: axios
  * **Styling**: CSS Modules, 순수 CSS (Flexbox, Grid), Glassmorphism Design
  * **Package Manager**: npm
  * **Runtime**: Node.js (v16+)

## 🚀 주요 기능

  * **실시간 대시보드**: 백엔드 API를 주기적으로 폴링(polling)하여 로봇의 상태(전원, 팬 속도, 바닥 상태, 경로, 센서 값)를 실시간으로 시각화합니다.
  * **양방향 수동 제어**: 사용자가 UI(스위치, 버튼, 십자키)를 조작하면, `axios`를 통해 BE 서버로 제어 명령을 전송합니다.
  * **동적 그래프 시각화**:
      * **Graph1**: 노면 상태별 통계를 막대 그래프로 시각화
      * **Graph2**: 3축(X, Y, Z) 센서 데이터를 실시간 꺾은선 그래프(SVG)로 시각화
  * **주행 경로 맵핑**: 로봇이 이동한 경로 좌표를 누적하여 실시간으로 지도에 그립니다.
  * **글래스모피즘 UI**: 현대적이고 세련된 반투명 유리 효과 디자인을 적용했습니다.

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

### **조회 (Polling)**

  * **API 1: 실시간 상태 조회** (`GET /api/robot/status`)
      * 로봇의 전원, 모드, 바닥 상태, 팬 속도, 주행 경로를 조회합니다.
  * **API 2: 노면 감지 통계** (`GET /api/robot/stats`)
      * 누적 청소 시간과 바닥 종류별(카펫, 일반, 먼지) 비율 통계를 조회합니다.
  * **API 3: 실시간 센서 데이터** (`GET /api/robot/sensor`)
      * 로봇의 X, Y, Z 센서 값을 실시간으로 조회합니다.

### **제어 (Control)**

  * **API 4: 팬 속도 설정** (`POST /api/manual/speed`)
      * 팬 속도를 1\~3단으로 조절합니다.
  * **API 5: 전원 제어** (`POST /api/manual/power`)
      * 로봇의 전원을 ON/OFF 합니다.
  * **API 6: 모드 변경** (`POST /api/manual/mode`)
      * 주행 모드를 AUTO 또는 MANUAL로 변경합니다.
  * **API 7: 방향 제어** (`POST /api/manual/direction`)
      * 수동 모드에서 로봇의 이동 방향(FWD, BACK, LEFT, RIGHT, STOP)을 제어합니다.

-----

## 🧩 주요 컴포넌트 설명

### `App.js` (중앙 관제실)

  * **역할**: 프로젝트의 최상위 컴포넌트입니다.
  * **기능**: `useState`로 모든 상태를 관리하고, `useEffect`로 API 폴링을 수행하며, 자식 컴포넌트들에게 데이터와 제어 함수를 `props`로 전달합니다.

### `StatusDisplay.jsx`

  * 로봇의 핵심 상태(전원, 모드, 배터리 등)를 요약해서 보여주는 패널입니다.

### `ControlPanel 시리즈`

  * **CP1**: 시스템 전원 ON/OFF 토글 스위치
  * **CP2**: 팬 속도(0\~3단) 조절 버튼 및 인디케이터
  * **CP3**: AUTO/MANUAL 모드 전환 및 수동 주행용 십자키 컨트롤러

### `PathMap.jsx`

  * 로봇의 이동 경로(`pathHistory`)를 시각적으로 보여주는 맵 컴포넌트입니다.

### `Graph 시리즈`

  * **Graph1**: 노면 감지 통계(비율)를 막대 그래프로 시각화
  * **Graph2**: 실시간 센서 데이터(X, Y, Z)를 SVG 꺾은선 그래프로 시각화

-----

## 📚 참고 자료

  * [React 공식 문서](https://reactjs.org/)
  * [axios 공식 문서](https://axios-http.com/)
  * [MDN Web Docs (CSS Grid/Flexbox)](https://developer.mozilla.org/ko/)

## PR 가이드

1.  새로운 기능 개발 시 `develop` 브랜치에서 별도 브랜치 생성 (`feature/fe-new-feature`)
2.  코드 변경 후 로컬 테스트 완료 확인 (`npm start`)
3.  커밋 메시지 명확하게 작성 (`git commit -m "Feat: 실시간 센서 그래프(Graph2) 구현"`)
4.  `develop` 브랜치로 Pull Request 생성 및 리뷰 요청
