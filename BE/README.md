## 📌 Controller

클라이언트의 요청(Request)을 수신하고, 적절한 서비스(Service) 계층으로 전달하는 역할을 담당합니다.
주된 기능은 다음과 같습니다:

* API 엔드포인트 정의
* 요청 파라미터 검증
* 서비스 호출 및 결과 반환(Response 생성)

---

## 📌 Domain

데이터베이스와 직접 연결되는 **엔티티(Entity)** 클래스가 존재하는 계층입니다.

* 프로젝트에서 사용하는 DB 테이블 구조를 객체 모델로 표현
* JPA와 매핑되는 필드 및 관계(Entity Relationship) 정의

---

## 📌 DTO (Data Transfer Object)

애플리케이션 내부에서 데이터를 주고받기 위해 사용하는 객체들의 모음입니다.

* Controller ↔ Service 간 데이터 전달
* Request/Response용 객체 정의
* 엔티티와 분리된 안전한 데이터 구조 제공

---

## 📌 Service

비즈니스 로직을 수행하는 핵심 계층입니다.

* Controller로부터 전달받은 요청을 처리
* 여러 Repository를 조합하여 실제 동작 수행
* 도메인 규칙 또는 프로젝트 로직 구현

---

## 📌 Repository

DB에 접근하기 위한 인터페이스 계층입니다.

* JpaRepository를 확장하여 기본 CRUD 제공
* Domain 엔티티와 연결되어 데이터베이스와 직접 통신
* 특정 조건의 조회를 위한 쿼리 메서드 정의

