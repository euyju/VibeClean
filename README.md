# VibeClean - Iot programing team project

##프로젝트 소개 (Introduction)

본 프로젝트는 STM32보드와 Edge AI 기술을 활용하여 노면 상태를 실시간으로 감지하고, 이를 기반으로 자율주행 및 지능형 청소 기능을 수행하는 로봇 청소기 시스템을 개발하는 것을 목표로 합니다.
---
##핵심 기능 (MVP)
1. 자율주행 (초음파 센서 감지)  → 벽 감지시 경로 변경
2. 대시보드 실시간 웹 모니터링 (React)
3. 노면감지 AI모델 → 서보모터 → 무선 청소기 n단 제어 / 속도 제어
4. 2D 맵핑 → 경로 history 표시
---
##개발 환경

Embedded: STM32CubeIDE

Frontend: 

Backend: 

AI:

Version Control: GitHub
---
#팀원 및 역할 분담

백승찬	(PM) Edge AI	전체 총괄 및 일정 관리, 노면 분류 모델(TinyML) 개발, 데이터셋 수집, 노선 관리

이의주	(ESW)	자율주행 보조, 2D 맵핑, Edge AI 보조, GitHub 관리

박태정	(HW)	자율주행 및 HW 총괄

고현서	(BE)	BE(Spring Boot), STM 통신, 발표 및 PPT 제작

변정섭	(FE)	FE(React) 대시보드, 자료 조사, 회의록 관리

---
#협업 가이드라인

1. 브랜치 전략 (Branch Strategy)

main: 배포 가능한 최종 버전. (직접 커밋 금지)
develop: 개발 진행 중인 최신 버전. (모든 기능 브랜치의 병합 대상)
feature/[기능이름]: 개인/기능별 작업 공간. (develop에서 분기)

2. 표준 협업 Flow
   1. develop 브랜치 Pull: develop의 최신 코드를 로컬로 가져옵니다.
   2. Feature 브랜치 생성: 로컬에서 작업 브랜치 생성 후 작업합니다. (예: feature/react-dashboard-ui)
   3. Commit & Push: 작업 완료 후 개인 브랜치에 커밋하고 푸시합니다.
   4. Pull Request (PR) 생성: feature/[기능이름] -> develop 브랜치로 PR을 생성합니다.
   5. Merge: 리뷰 완료 및 승인 후 develop에 병합합니다.

 (PR 생성방법 참고)
 
 <img width="368" height="408" alt="image" src="https://github.com/user-attachments/assets/ee0f346e-039b-47d9-bb9c-a656954e04b2" />

 
  현재 브랜치를 develop으로 두신 다음, Find or Create branch...에 feature/[기능이름] 입력 후 create branch ... 클릭하시면 됩니다.
