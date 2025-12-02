/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>    // printf, sprintf 사용
#include <stdarg.h>   // 가변 인자 사용
#include <math.h>
#include "ESP8266_HAL.h"
#include <string.h>   // strlen 사용
#include "stm32f4xx_hal.h" // HAL 함수 사용
#include "mpu6050.h"  // Edge-AI용 MPU6050 드라이버
#include "edge_ai_wrapper.h"  // Edge Impulse SDK Wrapper 헤더
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
// RGB LED 핀 정의
#define LED_R_PORT GPIOB
#define LED_R_PIN  GPIO_PIN_13
#define LED_G_PORT GPIOB
#define LED_G_PIN  GPIO_PIN_14
#define LED_B_PORT GPIOB
#define LED_B_PIN  GPIO_PIN_15

// RGB LED 색상 정의 (Common Cathode: Active High)
#define RGB_OFF         0, 0, 0
#define RGB_RED         1, 0, 0
#define RGB_GREEN       0, 1, 0
#define RGB_BLUE        0, 0, 1
#define RGB_YELLOW      1, 1, 0
#define RGB_MAGENTA     1, 0, 1
#define RGB_CYAN        0, 1, 1
#define RGB_WHITE       1, 1, 1

// LED 상태 타이밍 (ms)
#define LED_BLINK_INTERVAL_BASE    500
#define LED_BLINK_INTERVAL_FLOOR   500

// LED 우선순위 상태 정의
typedef enum {
    LED_STATE_NORMAL = 0,
    LED_STATE_STANDBY = 1
} LED_Priority_State_t;

#define MPU6050_ADDR  (0x68 << 1)
#define PWR_MGMT_1    0x6B
#define ACCEL_XOUT_H  0x3B

// Edge-AI 관련 상수
#define EDGE_AI_SAMPLE_COUNT    200    // 2초 @ 100Hz (윈도우 크기)
#define EDGE_AI_AXES            3      // 3축 가속도
#define EDGE_AI_SLIDE_INTERVAL  100    // 1초마다 AI 판단 (100 샘플 = 1초)

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;
I2C_HandleTypeDef hi2c3;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;
TIM_HandleTypeDef htim6;
TIM_HandleTypeDef htim8;

UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;

/* USER CODE BEGIN PV */
TIM_HandleTypeDef htim2;

// Edge-AI 데이터 버퍼 (슬라이딩 윈도우 방식)
float edge_ai_buffer[EDGE_AI_SAMPLE_COUNT * EDGE_AI_AXES];
volatile uint16_t edge_ai_sample_index = 0;
volatile uint8_t edge_ai_buffer_ready = 0;
volatile uint16_t edge_ai_slide_counter = 0;  // 슬라이딩 인터벌 카운터

// IMU 센서 디버깅용 변수
volatile uint8_t imu_debug_counter = 0;  // 0.1초마다 출력하기 위한 카운터
volatile uint8_t imu_debug_ready = 0;    // 출력 준비 플래그
float imu_debug_ax, imu_debug_ay, imu_debug_az;  // 마지막 측정값 저장

// MQTT 관련 전역 변수
#define WIZFI360_MAX_RESPONSE_SIZE 512
char wizfi_rx_buffer[WIZFI360_MAX_RESPONSE_SIZE];

// MQTT Subscribe 수신 버퍼
#define MQTT_RX_BUF_SIZE 256
char mqtt_rx_buf[MQTT_RX_BUF_SIZE];
int mqtt_rx_len = 0;

// MQTT control 상태 변수
int g_powerOn = -1;

// Edge-AI 판별 결과 저장
char g_current_floor[16] = "Unknown";

// LED 제어 상태 변수
LED_Priority_State_t g_led_priority_state = LED_STATE_NORMAL;
uint32_t g_led_last_update_tick = 0;
uint8_t g_led_toggle_state = 0;  // 0 or 1 (교차 깜빡임용)
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM4_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM8_Init(void);
static void MX_TIM6_Init(void);
static void MX_I2C3_Init(void);
/* USER CODE BEGIN PFP */
// UART 함수
void UART_Printf(const char *format, ...);

// MQTT 함수
void MQTT_ProcessResponseBuffer(uint8_t *buf, uint16_t len);

// RGB LED 제어 함수
void set_rgb_led(uint8_t r, uint8_t g, uint8_t b);
void update_led_state(void);
void set_led_priority_state(LED_Priority_State_t state);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/**
  * @brief UART2를 이용한 printf 함수
  * @param format: 출력 형식 문자열
  * @retval None
  */
void UART_Printf(const char *format, ...) {
    char str[100];
    va_list args;
    va_start(args, format);
    vsprintf(str, format, args);
    va_end(args);
    HAL_UART_Transmit(&huart2, (uint8_t *)str, strlen(str), HAL_MAX_DELAY);
}

/**
  * @brief RGB LED 색상 설정 함수
  * @param r: Red 상태 (0=OFF, 1=ON)
  * @param g: Green 상태 (0=OFF, 1=ON)
  * @param b: Blue 상태 (0=OFF, 1=ON)
  * @retval None
  */
void set_rgb_led(uint8_t r, uint8_t g, uint8_t b)
{
    HAL_GPIO_WritePin(LED_R_PORT, LED_R_PIN, r ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LED_G_PORT, LED_G_PIN, g ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LED_B_PORT, LED_B_PIN, b ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

/**
  * @brief LED 우선순위 상태 설정 함수
  * @param state: LED_STATE_NORMAL, LED_STATE_OBSTACLE, LED_STATE_STANDBY
  * @retval None
  */
void set_led_priority_state(LED_Priority_State_t state)
{
    g_led_priority_state = state;
    g_led_toggle_state = 0;  // 상태 변경 시 토글 상태 초기화
    g_led_last_update_tick = HAL_GetTick();  // 타이머 리셋
}

/**
  * @brief LED 상태 업데이트 함수 (메인 루프에서 주기적으로 호출)
  * @retval None
  * @note 우선순위: STANDBY(최고) > OBSTACLE > NORMAL
  * @note 일반 모드: 베이스 색상(1000ms) ↔ 바닥 색상(500ms) 2:1 비율
  */

void update_led_state(void)
{
    uint32_t current_tick = HAL_GetTick();
    uint32_t interval;

    // 우선순위 1: 대기 상태 (모터 정지) - 빨간색 고정
    if (g_led_priority_state == LED_STATE_STANDBY) {
        set_rgb_led(RGB_RED);
        return;
    }

    // 우선순위 2: 장애물 회피 - 빨간색 고정 (점멸 없음)
    if (g_led_priority_state == LED_STATE_OBSTACLE) {
        set_rgb_led(RGB_RED);  // 빨간색 계속 켜짐
        return;
    }

    // 우선순위 3: 일반 동작
    // Manual Mode: 파란색 고정 (점멸 없음)
    if (g_modeManual == 1) {
        set_rgb_led(RGB_BLUE);  // 수동 모드는 파란색 고정
        return;
    }

    // Auto Mode: 노면 감지에 따른 교차 깜빡임 (1:1 비율)
    // g_led_toggle_state: 0 = 하얀색(500ms), 1 = 바닥 색상(500ms)

    // 현재 표시할 색상 먼저 결정
    uint8_t base_r = 1, base_g = 1, base_b = 1;  // 하얀색

    // 바닥 색상 결정 (Edge-AI 판별 결과에 따라)
    uint8_t floor_r, floor_g, floor_b;
    if (strcmp(g_current_floor, "Hard") == 0) {
        // Hard Floor -> Green (초록색)
        floor_r = 0; floor_g = 1; floor_b = 0;
    } else if (strcmp(g_current_floor, "Carpet") == 0) {
        // Carpet -> Magenta (자홍색)
        floor_r = 1; floor_g = 0; floor_b = 1;
    } else if (strcmp(g_current_floor, "Dusty") == 0) {
        // Dusty -> Red (빨간색)
        floor_r = 1; floor_g = 0; floor_b = 0;
    } else {
        // Unknown -> Off (꺼짐)
        floor_r = 0; floor_g = 0; floor_b = 0;
    }

    // 현재 상태에 따른 interval 설정
    if (g_led_toggle_state == 0) {
        interval = LED_BLINK_INTERVAL_BASE;  // 하얀색 표시 시간 (500ms)
    } else {
        interval = LED_BLINK_INTERVAL_FLOOR; // 바닥 색상 표시 시간 (500ms)
    }

    if (current_tick - g_led_last_update_tick >= interval) {
        // 다음 색상으로 전환
        g_led_toggle_state = !g_led_toggle_state;
        g_led_last_update_tick = current_tick;
    }

    // 현재 토글 상태에 따라 LED 색상 표시
    // toggle_state = 0 → 하얀색 표시 (1000ms 유지)
    // toggle_state = 1 → 바닥 색상 표시 (500ms 유지)
    if (g_led_toggle_state == 0) {
        set_rgb_led(base_r, base_g, base_b);  // 하얀색
    } else {
        set_rgb_led(floor_r, floor_g, floor_b);  // 바닥색
    }
}

// MQTT 통신 함수
void Send_AT_Command(UART_HandleTypeDef *huart_wiz, UART_HandleTypeDef *huart_term, const char *command, uint8_t *response_buffer, uint16_t buffer_size, uint32_t timeout)
{
    // 1. 터미널(huart_term, PuTTY)로 전송할 명령 표시
    char tx_log[64];
    int len = snprintf(tx_log, sizeof(tx_log), "\r\n[CMD] Sending: %s\r\n", command);
    HAL_UART_Transmit(huart_term, (uint8_t *)tx_log, len, HAL_MAX_DELAY);

    // 2. WizFi360 (huart_wiz)에 명령 전송 (명령어 끝에 CR+LF(\r\n) 필수)
    char cmd_with_crlf[128];
    snprintf(cmd_with_crlf, sizeof(cmd_with_crlf), "%s\r\n", command);
    HAL_UART_Transmit(huart_wiz, (uint8_t *)cmd_with_crlf, strlen(cmd_with_crlf), 500);

    // 3. 응답 수신 준비
    memset(response_buffer, 0, buffer_size); // 버퍼 초기화
    uint16_t index = 0;
    uint32_t start_tick = HAL_GetTick();

    // 4. WizFi360으로부터 응답 수신 (간단한 타임아웃 폴링 방식)
    while ((HAL_GetTick() - start_tick) < timeout)
    {
        uint8_t rx_byte;
        // HAL_UART_Receive는 지정된 타임아웃 내에 1바이트를 수신합니다.
        if (HAL_UART_Receive(huart_wiz, &rx_byte, 1, 100) == HAL_OK)
        {
            if (index < buffer_size - 1)
            {
                response_buffer[index++] = rx_byte;
            }
            // 새로운 바이트를 수신할 때마다 타임아웃을 연장합니다.
            start_tick = HAL_GetTick();
        }
    }

    // 5. 수신된 응답을 터미널(huart_term, PuTTY)로 출력
    char *rx_header = "[RSP] Response Received:\r\n";
    HAL_UART_Transmit(huart_term, (uint8_t *)rx_header, strlen(rx_header), HAL_MAX_DELAY);

    MQTT_ProcessResponseBuffer(response_buffer, index);

    // 수신된 raw 데이터를 그대로 터미널로 출력
    HAL_UART_Transmit(huart_term, response_buffer, index, HAL_MAX_DELAY);

    char *rx_footer = "\r\n[END] ----------------------------------\r\n";
    HAL_UART_Transmit(huart_term, (uint8_t *)rx_footer, strlen(rx_footer), HAL_MAX_DELAY);
}

// 현재 control 상태를 PuTTY(USART2)에 출력하는 디버그용 함수
static void Debug_PrintControlState(const char *topic, const char *json)
{
    char buf[160];

    int len = snprintf(buf, sizeof(buf),
                       "\r\n[CONTROL] topic = %s\r\n"
                       "          json  = %s\r\n"
                       "          power = %d, speed = %d, mode = %d, dir = %s\r\n",
                       topic,
                       json,
                       g_powerOn,
                       g_fanSpeed,
                       g_modeManual,
                       g_direction);

    HAL_UART_Transmit(&huart2, (uint8_t *)buf, len, HAL_MAX_DELAY);
}

// -----------------------------------------------------
// MQTT 초기화용 공용 AT 명령 함수들
// -----------------------------------------------------

uint8_t MQTT_SetConfig(
    UART_HandleTypeDef *huart_wiz,
    UART_HandleTypeDef *huart_term,
    const char *user,
    const char *pass,
    const char *clientID,
    int aliveTime,
    uint8_t *resp, uint16_t resp_size
) {
    char cmd[256];
    snprintf(cmd, sizeof(cmd),
             "AT+MQTTSET=\"%s\",\"%s\",\"%s\",%d",
             user, pass, clientID, aliveTime);

    Send_AT_Command(huart_wiz, huart_term, cmd, resp, resp_size, 3000);
    return strstr((char*)resp, "OK") != NULL;
}

uint8_t MQTT_SetTopics(
    UART_HandleTypeDef *huart_wiz,
    UART_HandleTypeDef *huart_term,
    const char *pubTopic,
    const char *subTopic,
    uint8_t *resp, uint16_t resp_size
) {
    char cmd[256];
    snprintf(cmd, sizeof(cmd),
             "AT+MQTTTOPIC=\"%s\",\"%s\"",
             pubTopic, subTopic);

    Send_AT_Command(huart_wiz, huart_term, cmd, resp, resp_size, 3000);
    return strstr((char*)resp, "OK") != NULL;
}

uint8_t MQTT_SetQos(
    UART_HandleTypeDef *huart_wiz,
    UART_HandleTypeDef *huart_term,
    int qos,
    uint8_t *resp, uint16_t resp_size
) {
    char cmd[32];
    snprintf(cmd, sizeof(cmd), "AT+MQTTQOS=%d", qos);

    Send_AT_Command(huart_wiz, huart_term, cmd, resp, resp_size, 2000);
    return strstr((char*)resp, "OK") != NULL;
}

uint8_t MQTT_ConnectBroker(
    UART_HandleTypeDef *huart_wiz,
    UART_HandleTypeDef *huart_term,
    const char *brokerIP,
    int port,
    uint8_t *resp, uint16_t resp_size
) {
    char cmd[128];
    snprintf(cmd, sizeof(cmd),
             "AT+MQTTCON=0,\"%s\",%d",
             brokerIP, port);

    Send_AT_Command(huart_wiz, huart_term, cmd, resp, resp_size, 6000);

    return strstr((char*)resp, "CONNECT") || strstr((char*)resp, "OK");
}

uint8_t MQTT_Init_All(
    UART_HandleTypeDef *huart_wiz,
    UART_HandleTypeDef *huart_term
){
    uint8_t resp[512];

    // -------- Wi-Fi 연결 --------
    Send_AT_Command(huart_wiz, huart_term, "AT", resp, sizeof(resp), 1000);
    Send_AT_Command(huart_wiz, huart_term, "AT+RST", resp, sizeof(resp), 5000);
    Send_AT_Command(huart_wiz, huart_term, "AT+CWMODE=1", resp, sizeof(resp), 2000);

    // WiFi 접속
    Send_AT_Command(huart_wiz, huart_term,
        "AT+CWJAP=\"S24\",\"dial8787@@\"",
        resp, sizeof(resp), 5000);

    // -------- MQTT 설정 --------
    if (!MQTT_SetConfig(huart_wiz, huart_term,
                        "", "", "STM32_AI", 300,
                        resp, sizeof(resp))) return 0;

    if (!MQTT_SetTopics(huart_wiz, huart_term,
                        "vibeclean/robot1/ai",
                        "vibeclean/robot1/control/#",
                        resp, sizeof(resp))) return 0;

    if (!MQTT_SetQos(huart_wiz, huart_term, 0, resp, sizeof(resp)))
        return 0;

    if (!MQTT_ConnectBroker(huart_wiz, huart_term,
                            "192.168.223.61", 1883,
                            resp, sizeof(resp)))
        return 0;

    return 1;
}


void Publish_Message(void)
{
    char cmd[512];
    char json_message[400];

    // 실시간 센서 데이터와 AI 판별 결과를 포함한 JSON 생성
    snprintf(json_message, sizeof(json_message),
            "{"
                "\"currentFloor\":\"%s\","
                "\"fanSpeed\":%d,"
                "\"sensor\":{"
                    "\"x\":%.3f,"
                    "\"y\":%.3f,"
                    "\"z\":%.3f"
                "}"
            "}",
            g_current_floor,
            0,
            imu_debug_ax,
            imu_debug_ay,
            imu_debug_az
    );


    // topic은 이미 AT+MQTTTOPIC로 설정되어 있으므로 메시지만 전달
    snprintf(cmd, sizeof(cmd), "AT+MQTTPUB=\"%s\"", json_message);
    Send_AT_Command(&huart3, &huart2, cmd, wizfi_rx_buffer, WIZFI360_MAX_RESPONSE_SIZE, 1000);
//    WizFi_SendOnly(&huart3, &huart2, cmd); //wizfi통해서 보내기만 하는 애
}

// 토픽과 JSON을 보고 상태 변수에 반영
void HandleControlJson(const char *topic, const char *json)
{
    if (strstr(topic, "control/power")) {
        if (strstr(json, "\"ON\""))  g_powerOn = 1;
        if (strstr(json, "\"OFF\"")) g_powerOn = 0;
    }
    else if (strstr(topic, "control/speed")) {
        int v = 0;
        // 숫자 하나만 뽑는 단순 파서 (예: {"fanSpeed":2})
        sscanf(json, "%*[^0-9]%d", &v);
        if (v < 0) v = 0;
        if (v > 3) v = 3;
        g_fanSpeed = v;
    }
    else if (strstr(topic, "control/mode")) {
        if (strstr(json, "\"MANUAL\"")) g_modeManual = 1;
        if (strstr(json, "\"AUTO\""))   g_modeManual = 0;
    }
    else if (strstr(topic, "control/direction")) {
        if (strstr(json, "\"FWD\""))   strcpy(g_direction, "FWD");
        else if (strstr(json, "\"BACK\""))  strcpy(g_direction, "BACK");
        else if (strstr(json, "\"LEFT\""))  strcpy(g_direction, "LEFT");
        else if (strstr(json, "\"RIGHT\"")) strcpy(g_direction, "RIGHT");
        else if (strstr(json, "\"STOP\""))  strcpy(g_direction, "STOP");
    }

    Debug_PrintControlState(topic, json);
}

// WizFi360이 보낸 한 줄(line)을 토픽 / json 으로 분리
void ParseMqttLine(char *line, int len)
{
    line[len] = '\0';

    // "topic->\"...\"" 형식에서 화살표 위치 찾기
    char *arrow = strstr(line, "->");
    if (!arrow) return;

    *arrow = '\0';
    char *topic = line;
    char *payload = arrow + 2;

    // 공백/따옴표 건너뛰기
    while (*payload == ' ' || *payload == '"' || *payload == '\r' || *payload == '\n')
        payload++;

    // JSON 시작 '{' 위치 찾기
    char *brace = strchr(payload, '{');
    if (!brace) return;

    char *end = strrchr(brace, '}');
    if (!end) return;
    *(end + 1) = '\0';   // JSON 문자열을 '\0'로 끝나게

    HandleControlJson(topic, brace);
}

// WizFi360 응답 버퍼 전체에서 "topic -> { json }" 형태의 구독 메시지를 찾아 처리
void MQTT_ProcessResponseBuffer(uint8_t *buf, uint16_t len)
{
    char line[256];
    uint16_t pos = 0;

    for (uint16_t i = 0; i < len; i++) {
        char c = (char)buf[i];

        // CR은 무시
        if (c == '\r') {
            continue;
        }

        // 줄 끝이거나 line 버퍼가 꽉 찬 경우
        if (c == '\n' || pos >= sizeof(line) - 1) {

            if (pos > 0) {
                line[pos] = '\0';

                // "->" 들어간 라인만 MQTT 메시지로 인정
                if (strstr(line, "->")) {
                    ParseMqttLine(line, strlen(line));
                }
            }

            pos = 0;
        }
        else {
            // 일반 문자는 line에 저장
            line[pos++] = c;
        }
    }

    // 마지막 라인이 '\n' 없이 종료되었을 때 처리
    if (pos > 0) {
        line[pos] = '\0';
        if (strstr(line, "->")) {
            ParseMqttLine(line, strlen(line));
        }
    }
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_TIM2_Init();
  MX_TIM1_Init();
  MX_TIM4_Init();
  MX_I2C1_Init();
  MX_USART3_UART_Init();
  MX_TIM3_Init();
  MX_TIM8_Init();
  MX_TIM6_Init();
  MX_I2C3_Init();
  /* USER CODE BEGIN 2 */
  UART_Printf("\r\n=== VibeClean ===\r\n");

  // I2C 통신 테스트
  UART_Printf("Testing I2C communication...\r\n");
  uint8_t who_am_i = MPU6050_WhoAmI(&hi2c1);
  UART_Printf("WHO_AM_I Register: 0x%02X (Expected: 0x68)\r\n", who_am_i);

  // MPU6050 센서 초기화
  UART_Printf("Initializing MPU6050...\r\n");
  HAL_StatusTypeDef mpu_status = MPU6050_Init(&hi2c1);

  if (mpu_status == HAL_OK) {
      UART_Printf("MPU6050 Init: OK\r\n");
  } else {
      UART_Printf("MPU6050 Init: FAILED! (Status: %d)\r\n", mpu_status);
      UART_Printf("Check I2C connections (SDA: PB9, SCL: PB8)\r\n");
      UART_Printf("Check MPU6050 power supply (3.3V)\r\n");
      UART_Printf("Check pull-up resistors on SDA/SCL (4.7k ohm)\r\n");
  }

  // Edge Impulse 분류기 초기화
  if (edge_ai_init() == 0) {
      UART_Printf("Edge Impulse Init: OK\r\n");
  } else {
      UART_Printf("Edge Impulse Init: FAILED!\r\n");
  }

  // TIM6 100Hz 인터럽트 시작
  HAL_TIM_Base_Start_IT(&htim6);
  UART_Printf("TIM6 100Hz Timer: Started\r\n");
  UART_Printf("Collecting %d samples (%.1f seconds)...\r\n\r\n",
              EDGE_AI_SAMPLE_COUNT, EDGE_AI_SAMPLE_COUNT / 100.0f);

  // RGB LED 초기화 (초기 상태: OFF)
  set_rgb_led(RGB_OFF);
  UART_Printf("RGB LED: Initialized\r\n");

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  uint32_t last_pub_tick = 0;

  HAL_Delay(3000);
  MQTT_Init_All(&huart3, &huart2);
  
  while (1)
  {
	  // ==========================================================
	  // [1] MQTT Publish & Data Receiving (데이터 수신/발신 통합)
	  // ==========================================================
	  // Publish 함수 내부에서 AT 커맨드 응답을 기다릴 때
	  // Subscribe 수동조작 명령도 같이 수신하여 처리함.
	        uint32_t now_tick = HAL_GetTick();
	        if (now_tick - last_pub_tick >= 600) { // 1000 = 1초 주기 (필요시 단축 가능)
	            Publish_Message();
	            last_pub_tick = now_tick;
	        }

	  // ==========================================================
	  // [2] POWER OFF 체크
	  // ==========================================================
	        if (g_powerOn == 0) {
	            set_led_priority_state(LED_STATE_STANDBY);
	            HAL_Delay(100);
	            continue;
	        }

	  // ==========================================================
	  // [3] 센서 및 AI 업데이트
	  // ==========================================================
      // === RGB LED 상태 업데이트 (논블로킹 방식) ===
      update_led_state();

      // === IMU 센서 디버깅 출력 (0.1초마다) ===
      if (imu_debug_ready) {
          UART_Printf("[IMU] Ax: %.3fg, Ay: %.3fg, Az: %.3fg\r\n",
                     imu_debug_ax, imu_debug_ay, imu_debug_az);
          imu_debug_ready = 0;  // 플래그 리셋
      }

      // === Edge-AI 테스트 코드 ===
      if (edge_ai_buffer_ready) {
          surface_classification_t result = {0};

          // Edge Impulse 분류기 실행
          if (edge_ai_classify(edge_ai_buffer, EDGE_AI_SAMPLE_COUNT * EDGE_AI_AXES, &result) == 0) {
              // 판별 결과 출력 (Hard, Carpet, Dusty - 대문자 시작)
              UART_Printf("[AI] Hard: %.2f, Carpet: %.2f, Dusty: %.2f\r\n",
                         result.Hard, result.Carpet, result.Dusty);

              // 가장 높은 확률의 노면 타입 저장
              if (result.Hard >= result.Carpet && result.Hard >= result.Dusty) {
                  strcpy(g_current_floor, "Hard");
              } else if (result.Carpet >= result.Hard && result.Carpet >= result.Dusty) {
                  strcpy(g_current_floor, "Carpet");
              } else {
                  strcpy(g_current_floor, "Dusty");
              }
          } else {
              UART_Printf("[AI] Classification FAILED\r\n");
              strcpy(g_current_floor, "Unknown");
          }

          // 버퍼 준비 완료 플래그 리셋
          edge_ai_buffer_ready = 0;
      }

      HAL_Delay(1);
//      // === MQTT 메시지 발행 (5초마다) ===    //위치이동하고 주석 처리하였습니다.
//      uint32_t now_tick = HAL_GetTick();
//      if (now_tick - last_pub_tick >= 1000) {
//          Publish_Message();
//          last_pub_tick = now_tick;
//      }

     /* USER CODE BEGIN WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 50;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 400000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief I2C3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C3_Init(void)
{

  /* USER CODE BEGIN I2C3_Init 0 */

  /* USER CODE END I2C3_Init 0 */

  /* USER CODE BEGIN I2C3_Init 1 */

  /* USER CODE END I2C3_Init 1 */
  hi2c3.Instance = I2C3;
  hi2c3.Init.ClockSpeed = 400000;
  hi2c3.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c3.Init.OwnAddress1 = 0;
  hi2c3.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c3.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c3.Init.OwnAddress2 = 0;
  hi2c3.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c3.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C3_Init 2 */

  /* USER CODE END I2C3_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 84-1;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 999;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */
  HAL_TIM_MspPostInit(&htim1);

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_IC_InitTypeDef sConfigIC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 84-1;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 65535;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_IC_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 0;
  if (HAL_TIM_IC_ConfigChannel(&htim2, &sConfigIC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_IC_ConfigChannel(&htim2, &sConfigIC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_IC_ConfigChannel(&htim2, &sConfigIC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_Encoder_InitTypeDef sConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 0;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 65535;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  sConfig.EncoderMode = TIM_ENCODERMODE_TI1;
  sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC1Filter = 0;
  sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC2Filter = 0;
  if (HAL_TIM_Encoder_Init(&htim3, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */

}

/**
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM4_Init(void)
{

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 84-1;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 999;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */
  HAL_TIM_MspPostInit(&htim4);

}

/**
  * @brief TIM6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM6_Init(void)
{

  /* USER CODE BEGIN TIM6_Init 0 */

  /* USER CODE END TIM6_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM6_Init 1 */

  /* USER CODE END TIM6_Init 1 */
  htim6.Instance = TIM6;
  htim6.Init.Prescaler = 4999;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 99;
  htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM6_Init 2 */

  /* USER CODE END TIM6_Init 2 */

}

/**
  * @brief TIM8 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM8_Init(void)
{

  /* USER CODE BEGIN TIM8_Init 0 */

  /* USER CODE END TIM8_Init 0 */

  TIM_Encoder_InitTypeDef sConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM8_Init 1 */

  /* USER CODE END TIM8_Init 1 */
  htim8.Instance = TIM8;
  htim8.Init.Prescaler = 0;
  htim8.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim8.Init.Period = 65535;
  htim8.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim8.Init.RepetitionCounter = 0;
  htim8.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  sConfig.EncoderMode = TIM_ENCODERMODE_TI1;
  sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC1Filter = 0;
  sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC2Filter = 0;
  if (HAL_TIM_Encoder_Init(&htim8, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim8, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM8_Init 2 */

  /* USER CODE END TIM8_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
                          |GPIO_PIN_4|GPIO_PIN_5, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_12
                          |GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PC0 PC1 PC2 PC3
                           PC4 PC5 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
                          |GPIO_PIN_4|GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PA0 */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB0 PB1 PB2 PB12
                           PB13 PB14 PB15 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_12
                          |GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */
  // PA0 (Trig) Output 설정
  GPIO_InitStruct.Pin = TRIG_PIN; // PA0
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(TRIG_PORT, &GPIO_InitStruct);

  // PA1 (Echo) Input 설정
  GPIO_InitStruct.Pin = ECHO_PIN; // PA1
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL; // HC-SR04는 보통 풀업/풀다운 불필요
  HAL_GPIO_Init(ECHO_PORT, &GPIO_InitStruct);


  // S2: PC1 (TRIG), PA15 (ECHO)
  GPIO_InitStruct.Pin = TRIG_PIN1; // PC1
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(TRIG_PORT1, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = ECHO_PIN1; // PA15
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(ECHO_PORT1, &GPIO_InitStruct);


  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/**
  * @brief  TIM6 인터럽트 콜백 - Edge-AI 100Hz 데이터 수집 + IMU 디버깅 (슬라이딩 윈도우)
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM6) {
        float ax, ay, az;

        // MPU6050에서 가속도 읽기
        if (MPU6050_ReadAccel(&hi2c1, &ax, &ay, &az) == HAL_OK) {
            // Edge-AI 버퍼에 저장(현재 ax는 1.0g 단위이므로, 다시 16384.0f를 곱함)
            // 순환 버퍼 방식으로 저장 (슬라이딩 윈도우)
            edge_ai_buffer[edge_ai_sample_index * 3 + 0] = ax * 16384.0f;
            edge_ai_buffer[edge_ai_sample_index * 3 + 1] = ay * 16384.0f;
            edge_ai_buffer[edge_ai_sample_index * 3 + 2] = az * 16384.0f;

            // 디버깅용 - 10번마다 한 번씩(0.1초) 센서값 저장
            imu_debug_counter++;
            if (imu_debug_counter >= 10) {
                imu_debug_ax = ax;
                imu_debug_ay = ay;
                imu_debug_az = az;
                imu_debug_ready = 1;  // 출력 준비 완료
                imu_debug_counter = 0;
            }

            edge_ai_sample_index++;
            edge_ai_slide_counter++;

            // 버퍼가 가득 차면 순환 (슬라이딩 윈도우)
            if (edge_ai_sample_index >= EDGE_AI_SAMPLE_COUNT) {
                edge_ai_sample_index = 0;
            }

            // 1초(100개 샘플)마다 AI 판단 트리거
            if (edge_ai_slide_counter >= EDGE_AI_SLIDE_INTERVAL) {
                edge_ai_buffer_ready = 1;
                edge_ai_slide_counter = 0;
            }
        }
    }
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
