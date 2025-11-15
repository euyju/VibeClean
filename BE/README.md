# BE (Backend)

VibeClean 프로젝트의 백엔드 서버 디렉토리입니다.

## 👥 담당자

- **고현서** - Backend 개발, STM32 통신, 발표 및 PPT 제작

## 📁 폴더 구조

```
BE/
├── config/                          # 설정 파일
│   └── MqttConfig.java             # MQTT 브로커 연결 설정
├── controller/                      # REST API 컨트롤러
│   ├── manual/                     # 수동 제어 API
│   │   └── ManualController.java
│   ├── sensor/                     # 센서 데이터 API
│   │   └── SensorController.java
│   ├── stats/                      # 통계 API
│   │   └── StatsController.java
│   ├── status/                     # 로봇 상태 API
│   │   └── StatusController.java
│   └── stm/                        # STM32 통신 API
│       └── StmController.java
├── domain/                          # 엔티티 (DB 테이블 매핑)
│   ├── manual/                     # 수동 제어 엔티티
│   │   ├── ManualPower.java
│   │   └── ManualSpeed.java
│   ├── sensor/                     # 센서 엔티티
│   │   └── Sensor.java
│   └── status/                     # 상태 엔티티
│       └── Status.java
├── dto/                             # Data Transfer Objects
│   ├── manual/                     # 수동 제어 DTO
│   │   ├── request/
│   │   └── response/
│   ├── mqtt/                       # MQTT 메시지 DTO
│   │   └── TelemetryMessage.java
│   ├── sensor/                     # 센서 DTO
│   │   ├── request/
│   │   └── response/
│   ├── stats/                      # 통계 DTO
│   │   └── response/
│   ├── status/                     # 상태 DTO
│   │   ├── request/
│   │   └── response/
│   ├── stm/                        # STM32 통신 DTO
│   │   └── request/
│   ├── repository/                 # JPA Repository
│   │   ├── manual/
│   │   ├── sensor/
│   │   ├── stats/
│   │   └── status/
│   └── service/                    # 비즈니스 로직
│       ├── manual/
│       ├── mqtt/
│       ├── sensor/
│       ├── stats/
│       ├── status/
│       └── stm/
├── VibeCleanApplication.java       # Spring Boot 메인 클래스
└── README.md                        # 이 파일
```

## 개발 환경

### 기술 스택
- **Framework**: Spring Boot 3.x
- **Language**: Java 17+
- **Database**: JPA/Hibernate (H2/MySQL/PostgreSQL)
- **MQTT**: Eclipse Paho MQTT Client
- **Build Tool**: Gradle 또는 Maven

### 주요 기능
- **REST API**: 로봇 제어 및 상태 조회 API
- **MQTT 통신**: STM32와 실시간 양방향 통신
- **데이터 관리**: 센서 데이터, 통계, 청소 이력 저장
- **스케줄링**: 로봇 상태 모니터링 및 자동 처리

## 빌드 및 실행 가이드

### 1. 개발 환경 설정

#### 필수 요구사항
- **JDK 17 이상** 설치
- **IDE**: IntelliJ IDEA (권장), Eclipse, 또는 VS Code
- **MQTT Broker**: Mosquitto 또는 HiveMQ (로컬/클라우드)

#### 프로젝트 클론
```bash
git clone https://github.com/euyju/VibeClean.git
cd VibeClean_Project/BE
```

### 2. 설정 파일

`src/main/resources/application.yml` 파일에서 MQTT 및 DB 설정:

```yaml
mqtt:
  broker-url: tcp://broker.hivemq.com:1883  # MQTT 브로커 주소
  client-id: vibeclean-backend              # 클라이언트 ID

spring:
  datasource:
    url: jdbc:h2:mem:testdb                 # DB 연결 정보
    driver-class-name: org.h2.Driver
    username: sa
    password:
  jpa:
    hibernate:
      ddl-auto: update                      # 자동 스키마 생성
    show-sql: true                          # SQL 로그 출력
```

### 3. 프로젝트 빌드

#### Gradle 사용
```bash
# 빌드
./gradlew build

# 테스트 포함 빌드
./gradlew clean build

# 테스트 스킵
./gradlew build -x test
```

#### Maven 사용
```bash
# 빌드
mvn clean install

# 테스트 스킵
mvn clean install -DskipTests
```

### 4. 애플리케이션 실행

#### IDE에서 실행
1. `VibeCleanApplication.java` 파일 열기
2. `main()` 메서드 우클릭
3. `Run 'VibeCleanApplication'` 선택

#### 명령줄에서 실행
```bash
# Gradle
./gradlew bootRun

# Maven
mvn spring-boot:run

# JAR 파일 실행
java -jar build/libs/vibeclean-0.0.1-SNAPSHOT.jar
```

서버가 정상적으로 시작되면 `http://localhost:8080` 에서 접근 가능합니다.

## API 엔드포인트

### 로봇 상태 관리
| Method | Endpoint | 설명 |
|--------|----------|------|
| GET | `/api/status` | 현재 로봇 상태 조회 |
| POST | `/api/status` | 로봇 상태 업데이트 |

### 센서 데이터
| Method | Endpoint | 설명 |
|--------|----------|------|
| GET | `/api/sensor` | 센서 데이터 조회 |
| POST | `/api/sensor` | 센서 데이터 저장 |

### 수동 제어
| Method | Endpoint | 설명 |
|--------|----------|------|
| POST | `/api/manual/power` | 청소기 전원 제어 |
| POST | `/api/manual/speed` | 모터 속도 제어 |

### 통계
| Method | Endpoint | 설명 |
|--------|----------|------|
| GET | `/api/stats` | 청소 통계 조회 |

### STM32 통신
| Method | Endpoint | 설명 |
|--------|----------|------|
| POST | `/api/stm/command` | STM32로 명령 전송 |

## 주요 기능

### MQTT 통신
- **Broker 연결**: Eclipse Paho MQTT 클라이언트 사용
- **Pub/Sub 패턴**: STM32에서 발행한 텔레메트리 데이터 구독
- **자동 재연결**: 연결 끊김 시 자동으로 재연결
- **실시간 처리**: `TelemetryHandler`에서 실시간 메시지 처리

### REST API 계층 구조
```
Controller (요청 수신/응답)
    ↓
Service (비즈니스 로직)
    ↓
Repository (데이터 접근)
    ↓
Domain (DB 엔티티)
```

### 로봇 상태 모니터링
- **RobotOfflineChecker**: 주기적으로 로봇 연결 상태 확인
- **Scheduled Task**: Spring `@Scheduled` 어노테이션 활용
- **오프라인 감지**: 일정 시간 이상 응답 없을 시 알림

## 주요 클래스 설명

### Controller
클라이언트 요청을 수신하고 적절한 서비스로 전달합니다.
- API 엔드포인트 정의
- 요청 파라미터 검증
- 서비스 호출 및 응답 반환

### Domain
데이터베이스 테이블과 매핑되는 엔티티 클래스입니다.
- JPA 엔티티 정의
- 테이블 구조를 객체 모델로 표현
- 관계(Relationship) 정의

### DTO (Data Transfer Object)
계층 간 데이터 전송을 위한 객체입니다.
- Request/Response 객체
- 엔티티와 분리된 안전한 데이터 구조
- Controller ↔ Service 간 데이터 전달

### Service
비즈니스 로직을 처리하는 핵심 계층입니다.
- Controller에서 전달받은 요청 처리
- Repository를 조합한 복합 로직 수행
- 도메인 규칙 및 비즈니스 로직 구현

### Repository
데이터베이스 접근을 담당하는 인터페이스입니다.
- JpaRepository 확장
- 기본 CRUD 제공
- 커스텀 쿼리 메서드 정의

## ⚠️ 주의사항

1. **환경 변수 설정**
   - `application.yml`에서 MQTT 브로커 URL, DB 연결 정보 확인
   - 프로덕션 환경에서는 환경 변수 또는 외부 설정 파일 사용 권장

2. **MQTT 브로커 연결**
   - 로컬 테스트 시 Mosquitto 설치 권장
   - 클라우드 브로커 사용 시 네트워크 방화벽 설정 확인

3. **포트 충돌**
   - 기본 포트 8080이 사용 중이면 `application.yml`에서 변경
   ```yaml
   server:
     port: 8081
   ```

4. **데이터베이스**
   - H2는 개발용, 프로덕션에서는 MySQL/PostgreSQL 사용 권장
   - `ddl-auto: update`는 개발 환경에만 사용

## 📚 참고 자료

- [Spring Boot Documentation](https://docs.spring.io/spring-boot/docs/current/reference/htmlsingle/)
- [Spring Data JPA](https://docs.spring.io/spring-data/jpa/docs/current/reference/html/)
- [Eclipse Paho MQTT Client](https://www.eclipse.org/paho/index.php?page=clients/java/index.php)
- [MQTT Protocol](https://mqtt.org/mqtt-specification/)

## PR 가이드

1. 새로운 기능 개발 시 별도 브랜치 생성
   ```bash
   git checkout -b feature/be-new-feature
   ```

2. 코드 변경 후 빌드 및 테스트 완료 확인
   ```bash
   ./gradlew clean build test
   ```

3. 커밋 메시지는 명확하게 작성
   ```bash
   git commit -m "Add MQTT reconnection handler"
   ```

4. Pull Request 생성 및 리뷰 요청
