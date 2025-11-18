# HW (Hardware & Embedded Software)

VibeClean 프로젝트의 하드웨어 및 임베디드 소프트웨어 디렉토리입니다.

## 👥 담당자

- **박태정** - Hardware 설계 및 Embedded Software 개발 
- **이의주** - Embedded Software 개발
- **백승찬** - Edge AI 설계 및 Embedded Software 개발

## 📁 폴더 구조

```
HW/
├── STM32/                      # STM32F446 기반 임베디드 시스템
│   ├── Core/                   # 핵심 소스 코드
│   │   ├── Inc/               # 헤더 파일
│   │   │   ├── main.h
│   │   │   ├── ESP8266_HAL.h         # ESP8266 WiFi 모듈 드라이버
│   │   │   ├── UartRingbuffer_multi.h # UART 멀티채널 링 버퍼
│   │   │   ├── stm32f4xx_hal_conf.h   # HAL 설정
│   │   │   └── stm32f4xx_it.h         # 인터럽트 핸들러
│   │   ├── Src/               # 소스 파일
│   │   │   ├── main.c                 # 메인 프로그램
│   │   │   ├── ESP8266_HAL.c          # ESP8266 드라이버 구현
│   │   │   ├── UartRingbuffer_multi.c # UART 버퍼 구현
│   │   │   ├── stm32f4xx_hal_msp.c    # HAL MSP 초기화
│   │   │   ├── stm32f4xx_it.c         # 인터럽트 핸들러
│   │   │   └── system_stm32f4xx.c     # 시스템 초기화
│   │   └── Startup/           # 시작 코드
│   │       └── startup_stm32f446retx.s
│   ├── Drivers/               # STM32 HAL 드라이버 (자동 생성)
│   ├── SoundTest.ioc          # STM32CubeMX 프로젝트 파일
│   ├── STM32F446RETX_FLASH.ld # 링커 스크립트 (FLASH)
│   ├── STM32F446RETX_RAM.ld   # 링커 스크립트 (RAM)
│   ├── .cproject              # Eclipse 프로젝트 설정
│   ├── .project               # Eclipse 프로젝트 파일
│   └── .gitignore             # STM32 전용 gitignore
└── README.md                  # 이 파일
```

## 🛠️ 개발 환경

### 하드웨어
- **MCU**: STM32F446RET6
  - ARM Cortex-M4 @ 180MHz
  - 512KB Flash, 128KB RAM
  - LQFP64 패키지

### 주변 장치
- **WiFi 모듈**: ESP8266
- **센서**: HC-SR04 초음파 센서 (x3), 자이로 센서 (I2C)
- **모터**: DC 모터 (x3) - 좌/우 바퀴, 청소기
- **통신**: USART2, USART3
- **타이머**: TIM1, TIM2, TIM4
- **I2C**: I2C1

## 📌 핀맵 (Pin Mapping)

### 자이로 센서 (Gyro Sensor)
| 기능 | STM32 핀 | 비고 |
|------|----------|------|
| SDA  | PB9      | I2C1 Data |
| SCL  | PB6      | I2C1 Clock |
| GND  | PD0      | Ground |

### 초음파 센서 (HC-SR04 Ultrasonic Sensors)
| 센서 번호 | 신호 | STM32 핀 | 타이머 채널 | 비고 |
|-----------|------|----------|-------------|------|
| 센서 1    | TRIG | PC0      | -           | Trigger 출력 |
|           | ECHO | PA1      | TIM2_CH2    | Echo 입력 (펄스 측정) |
| 센서 2    | TRIG | PC1      | -           | Trigger 출력 |
|           | ECHO | PB8      | TIM2_CH1    | Echo 입력 (펄스 측정) |
| 센서 3    | TRIG | PC2      | -           | Trigger 출력 |
|           | ECHO | PB10     | TIM2_CH3    | Echo 입력 (펄스 측정) |

### 모터 드라이버 (Motor Driver - L298N)
| 모터 | 기능 | STM32 핀 | 타이머 채널 | 비고 |
|------|------|----------|-------------|------|
| **M1 (왼쪽 바퀴)** | IN1 | PB0 | - | Direction 1 |
|                    | IN2 | PB1 | - | Direction 2 |
|                    | ENA | PA8 | TIM1_CH1 | PWM 속도 제어 |
| **M2 (오른쪽 바퀴)** | IN3 | PB2 | - | Direction 1 |
|                      | IN4 | PB12 | - | Direction 2 |
|                      | ENB | PA9 | TIM1_CH2 | PWM 속도 제어 |
| **M3 (청소기)** | IN1 | PC3 | - | Direction 1 |
|                 | IN2 | PC4 | - | Direction 2 |
|                 | ENB | PB7 | TIM4_CH2 | PWM 속도 제어 |

### 소프트웨어 도구
- **IDE**: STM32CubeIDE
- **펌웨어**: STM32 HAL Library
- **설정 도구**: STM32CubeMX
- **디버거**: ST-Link

## 🚀 빌드 및 실행 가이드

### 1. 개발 환경 설정

#### STM32CubeIDE 사용
1. [STM32CubeIDE](https://www.st.com/en/development-tools/stm32cubeide.html) 다운로드 및 설치
2. STM32CubeIDE 실행
3. `File` > `Open Projects from File System...`
4. `HW/STM32` 폴더 선택

### 2. 프로젝트 빌드

```bash
# STM32CubeIDE에서
Project > Build Project

# 또는 단축키
Ctrl + B (Windows/Linux)
Cmd + B (macOS)
```

빌드 결과물은 `Debug/` 또는 `Release/` 폴더에 생성됩니다:
- `SoundTest.elf` - 실행 파일
- `SoundTest.bin` - 바이너리 파일
- `SoundTest.hex` - HEX 파일

### 3. 펌웨어 업로드

1. ST-Link를 보드에 연결
2. USB를 통해 PC에 연결
3. IDE에서 실행:
   ```
   Run > Debug (F11) 또는 Run (Ctrl+F11)
   ```

### 4. 디버깅

```
Run > Debug Configurations...
> STM32 Cortex-M C/C++ Application
> Debug
```

## 📝 주요 기능

### ESP8266 WiFi 통신
- AT 명령어 기반 WiFi 모듈 제어
- TCP/UDP 클라이언트/서버 기능
- UART 인터페이스 (USART3)

### 초음파 센서
- HC-SR04 다중 센서 제어
- 거리 측정 및 데이터 수집
- 타이머 기반 정밀 측정

### UART 멀티채널 링 버퍼
- 효율적인 UART 데이터 관리
- 논블로킹 통신
- 다중 UART 포트 지원

## 🔧 설정 변경 방법

### 핀 설정 또는 주변장치 변경
1. `SoundTest.ioc` 파일을 STM32CubeMX로 열기
2. 필요한 설정 변경
3. `Generate Code` 클릭
4. 생성된 코드 확인 및 사용자 코드 영역(`USER CODE BEGIN/END`) 유지

### HAL 라이브러리 설정
`Core/Inc/stm32f4xx_hal_conf.h` 파일에서 필요한 HAL 모듈 활성화/비활성화

## ⚠️ 주의사항

1. **사용자 코드 영역 준수**
   - STM32CubeMX가 코드를 재생성할 때 사용자 코드가 유지되도록 반드시 `USER CODE BEGIN`과 `USER CODE END` 사이에 코드 작성

2. **빌드 결과물 Git 제외**
   - `Debug/`, `Release/` 폴더는 `.gitignore`에 포함되어 있으므로 커밋되지 않음
   - 바이너리 파일(`.elf`, `.bin`, `.hex`)도 Git에 포함하지 않음

3. **전원 및 연결**
   - ST-Link 연결 시 전원 점퍼 설정 확인
   - 외부 센서 연결 시 전압 레벨 확인 (3.3V/5V)

## 📚 참고 자료

- [STM32F446 Reference Manual](https://www.st.com/resource/en/reference_manual/dm00135183.pdf)
- [STM32F446 Datasheet](https://www.st.com/resource/en/datasheet/stm32f446re.pdf)
- [STM32 HAL Documentation](https://www.st.com/resource/en/user_manual/dm00105879.pdf)
- [ESP8266 AT Command Set](https://www.espressif.com/sites/default/files/documentation/4a-esp8266_at_instruction_set_en.pdf)

## PR 가이드

1. 새로운 기능 개발 시 별도 브랜치 생성
   ```bash
   git checkout -b feature/hw-new-feature
   ```

2. 코드 변경 후 빌드 및 테스트 완료 확인

3. 커밋 메시지는 명확하게 작성
   ```bash
   git commit -m "Add HC-SR04 distance measurement function"
   ```

4. Pull Request 생성 및 리뷰 요청
