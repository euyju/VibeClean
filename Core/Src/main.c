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
#include <stdio.h>    // printf, sprintf 사용을 위해
#include <stdarg.h>   // 가변 인자 사용을 위해
#include <math.h>
//#include "ESP8266_HAL.h"
#include <string.h>   // strlen 사용을 위해
#include "stm32f4xx_hal.h" // HAL 함수 사용을 위해


/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
// HC-SR04 핀 정의
#define TRIG_PIN    GPIO_PIN_0
#define TRIG_PORT    GPIOC
#define ECHO_PIN    GPIO_PIN_1
#define ECHO_PORT    GPIOA

#define TRIG_PIN1    GPIO_PIN_1
#define TRIG_PORT1    GPIOC
#define ECHO_PIN1    GPIO_PIN_8
#define ECHO_PORT1    GPIOB

#define TRIG_PIN2    GPIO_PIN_2
#define TRIG_PORT2    GPIOC
#define ECHO_PIN2    GPIO_PIN_10
#define ECHO_PORT2    GPIOB

uint8_t rx_data[100];

#define WIZFI360_MAX_RESPONSE_SIZE 512
uint8_t wizfi_rx_buffer[WIZFI360_MAX_RESPONSE_SIZE];


// 클럭 설정 (SystemClock_Config 기준 HCLK = 84MHz)
// 1초 = 84,000,000 사이클 -> 1us = 84 사이클
#define DWT_DELAY_UNIT (HAL_RCC_GetHCLKFreq() / 1000000)

// HC-SR04 관련 상수
#define SOUND_SPEED_CM_PER_US 0.0343 // 음속: 343m/s = 0.0343 cm/us
#define MAX_TIMEOUT_US 30000 // 30ms (HC-SR04 최대 측정 거리 고려)



// 모터 1 (A) - ENA: TIM1_CH1 (PA8)
// 방향 핀
#define M1_IN1_PORT GPIOB
#define M1_IN1_PIN  GPIO_PIN_0
#define M1_IN2_PORT GPIOB
#define M1_IN2_PIN  GPIO_PIN_1

// 모터 2 (B) - ENB: TIM1_CH2 (PA9)
// 방향 핀
#define M2_IN1_PORT GPIOB
#define M2_IN1_PIN  GPIO_PIN_2
#define M2_IN2_PORT GPIOB
#define M2_IN2_PIN  GPIO_PIN_12

// 모터 3 (A) - ENA: TIM4_CH1 (Pb6)
#define M3_IN1_PORT GPIOC
#define M3_IN1_PIN  GPIO_PIN_3
#define M3_IN2_PORT GPIOC
#define M3_IN2_PIN  GPIO_PIN_4


// 모터 ID 및 방향 정의
#define MOTOR_A     1
#define MOTOR_B     2
#define MOTOR_C     3


#define FORWARD     1
#define BACKWARD    2
#define RIGHT    3 // 우회전
#define LEFT      4 // 좌회전 (필요시)
#define STOP        0

// PWM 최대값 (CubeMX TIM1 ARR 설정에 따라 변경될 수 있음. 여기서는 1000 가정)
#define PWM_MAX_VALUE 1000

// 벽 감지 임계 거리 (센티미터 단위)
#define WALL_DISTANCE_THRESHOLD 30.0f
// 직진 속도 및 회전 속도 (PWM 값)
#define BASE_SPEED          300
#define TURN_SPEED          300
// 회전 시간 상수 (90도 회전에 필요한 시간, 보정 필요)
#define TURN_90_TIME_MS     3200 // (모터와 바퀴에 따라 크게 달라짐)
// 유턴 시 전진/후진 거리 시간 (본체 폭만큼 이동하기 위한 시간, 보정 필요)
#define BODY_MOVE_TIME_MS   800 // 예시 값 (본체 폭에 따라 조절)

#define MPU6050_ADDR  (0x68 << 1)
#define PWR_MGMT_1    0x6B
#define ACCEL_XOUT_H  0x3B

//Odometry 상수
#define WHEEL_RADIUS_MM     30      // 바퀴 반지름 (mm)
#define ROBOT_AXLE_LENGTH_MM 160    // 바퀴 축 간 거리 (mm)
#define ENCODER_RESOLUTION  1000    // 엔코더 해상도 (펄스 수)
#define TIMER_PERIOD        65536   // TIM3/TIM8 주기

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;
TIM_HandleTypeDef htim8;

UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;

/* USER CODE BEGIN PV */
TIM_HandleTypeDef htim2;
//2D 맵핑용 위치변수
float g_robot_x = 0.0f;     // 현재의 X 좌표
float g_robot_y = 0.0f;     // 현재의 Y 좌표
float g_robot_yaw = 0.0f;   // 현재의 방향 각도 (단위: 도 Degree)


// MQTT control 상태 변수
int g_powerOn = -1;          // 0 = OFF, 1 = ON
int g_fanSpeed = -1;         // 0~3
int g_modeManual = -1;       // 0 = AUTO, 1 = MANUAL
char g_direction[8] = "NULL";  // "FWD","BACK","LEFT","RIGHT","STOP"

// MQTT 수신 라인 버퍼
char mqtt_rx_buf[256];
int  mqtt_rx_len = 0;

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
/* USER CODE BEGIN PFP */
void DWT_Init(void);
void DWT_Delay_us(uint32_t us);
float HCSR04_Read(GPIO_TypeDef *trigPort, uint16_t trigPin,
                  GPIO_TypeDef *echoPort, uint16_t echoPin);
void UART_Printf(const char *format, ...);



uint8_t rx[512];




/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/**
  * @brief DWT (Data Watchpoint and Trace) 초기화 함수
  * @retval None
  */


int _write(int file, char *ptr, int len)   // UART_Printf를 위한 retarget
{
    HAL_UART_Transmit(&huart3, (uint8_t*)ptr, len, 100);
    return len;
}

void DWT_Init(void) {
    // TRCENA 활성화 (DWT 활성화)
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    // CYCCNT 카운터 초기화
    DWT->CYCCNT = 0;
    // CYCCNT 카운터 활성화
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

/**
  * @brief DWT 기반 마이크로초 지연 함수
  * @param us: 지연할 마이크로초
  * @retval None
  */
void DWT_Delay_us(uint32_t us) {
    uint32_t start_tick = DWT->CYCCNT;
    uint32_t delay_ticks = us * DWT_DELAY_UNIT;
    while ((DWT->CYCCNT - start_tick) < delay_ticks);
}

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
    // UART 전송 (Blocking 모드)
    HAL_UART_Transmit(&huart2, (uint8_t *)str, strlen(str), HAL_MAX_DELAY);
}

/**
  * @brief HC-SR04 거리 측정 함수
  * @retval 측정 거리 (cm), 실패 시 -1.0
  */
float HCSR04_Read(GPIO_TypeDef *trigPort, uint16_t trigPin,
                  GPIO_TypeDef *echoPort, uint16_t echoPin)
{
    uint32_t start_time, end_time, duration_cycles;

    HAL_GPIO_WritePin(trigPort, trigPin, GPIO_PIN_RESET);
    DWT_Delay_us(2);

    HAL_GPIO_WritePin(trigPort, trigPin, GPIO_PIN_SET);
    DWT_Delay_us(10);
    HAL_GPIO_WritePin(trigPort, trigPin, GPIO_PIN_RESET);

    start_time = DWT->CYCCNT;
    while (HAL_GPIO_ReadPin(echoPort, echoPin) == GPIO_PIN_RESET) {
        if ((DWT->CYCCNT - start_time) > (MAX_TIMEOUT_US * DWT_DELAY_UNIT))
            return -1.0f;
    }

    start_time = DWT->CYCCNT;
    while (HAL_GPIO_ReadPin(echoPort, echoPin) == GPIO_PIN_SET) {
        if ((DWT->CYCCNT - start_time) > (MAX_TIMEOUT_US * DWT_DELAY_UNIT))
            return -1.0f;
    }

    end_time = DWT->CYCCNT;
    duration_cycles = end_time - start_time;

    float distance = ((float)duration_cycles / (float)DWT_DELAY_UNIT)
                     * SOUND_SPEED_CM_PER_US / 2.0f;

    return distance;
}

/* 안전한 방향 제어 함수 */
void set_motor_direction(uint8_t motor_id, uint8_t direction)
{
    GPIO_TypeDef *in1_port = NULL, *in2_port = NULL;
    uint16_t in1_pin = 0, in2_pin = 0;

    if (motor_id == MOTOR_A) {
        in1_port = M1_IN1_PORT;
        in2_port = M1_IN2_PORT;
        in1_pin  = M1_IN1_PIN;
        in2_pin  = M1_IN2_PIN;
    } else if (motor_id == MOTOR_B) {
        in1_port = M2_IN1_PORT;
        in2_port = M2_IN2_PORT;
        in1_pin  = M2_IN1_PIN;
        in2_pin  = M2_IN2_PIN;
    }else if (motor_id == MOTOR_C) {
        in1_port = M3_IN1_PORT;
        in2_port = M3_IN2_PORT;
        in1_pin  = M3_IN1_PIN;
        in2_pin  = M3_IN2_PIN;
    }
    else {
        return;
    }

    if (direction == FORWARD) {
        HAL_GPIO_WritePin(in1_port, in1_pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(in2_port, in2_pin, GPIO_PIN_RESET);

    } else if (direction == BACKWARD) {
        HAL_GPIO_WritePin(in1_port, in1_pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(in2_port, in2_pin, GPIO_PIN_SET);
    }
    else if (direction == BACKWARD) {
           HAL_GPIO_WritePin(in1_port, in1_pin, GPIO_PIN_RESET);
           HAL_GPIO_WritePin(in2_port, in2_pin, GPIO_PIN_SET);
       }
    else { // STOP (coast)
        HAL_GPIO_WritePin(in1_port, in1_pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(in2_port, in2_pin, GPIO_PIN_RESET);
    }
}


/* 속도 설정은 그대로 사용하되 PWM_MAX와 TIM1 ARR 일치 확인 필요 */
void set_motor_speed(uint8_t motor_id, uint16_t speed)
{
    if (speed > PWM_MAX_VALUE) speed = PWM_MAX_VALUE;

    if (motor_id == MOTOR_A) {
        __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, speed);
    } else if (motor_id == MOTOR_B) {
        __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, speed);
    } else if (motor_id == MOTOR_C) {
        __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, speed);
    }

}

/* --- 모터 제어 유틸 함수 추가 --- */
void stop_all_motors(void)
{
    set_motor_speed(MOTOR_A, 0);
    set_motor_speed(MOTOR_B, 0);
    set_motor_speed(MOTOR_C, 0);

    set_motor_direction(MOTOR_A, STOP);
    set_motor_direction(MOTOR_B, STOP);
    set_motor_direction(MOTOR_C, STOP);
}

void move_forward_pwm(uint16_t pwm)
{
    set_motor_direction(MOTOR_A, FORWARD);
    set_motor_direction(MOTOR_B, FORWARD);
    set_motor_speed(MOTOR_A, pwm);
    set_motor_speed(MOTOR_B, pwm);
}

void move_backward_pwm(uint16_t pwm)
{
    set_motor_direction(MOTOR_A, BACKWARD);
    set_motor_direction(MOTOR_B, BACKWARD);
    set_motor_speed(MOTOR_A, pwm);
    set_motor_speed(MOTOR_B, pwm);
}

void rotate_right_inplace(uint16_t pwm) // 오른쪽으로 제자리 회전 (좌/우 휠 반대방향)
{
    // 왼쪽 앞바퀴 전진, 오른쪽 바퀴 후진 -> 우회전
    set_motor_direction(MOTOR_A, FORWARD);   // 왼쪽
    set_motor_direction(MOTOR_B, BACKWARD);  // 오른쪽
    set_motor_speed(MOTOR_A, pwm);
    set_motor_speed(MOTOR_B, pwm);
}

void rotate_left_inplace(uint16_t pwm) // 왼쪽으로 제자리 회전
{
    set_motor_direction(MOTOR_A, BACKWARD);
    set_motor_direction(MOTOR_B, FORWARD);
    set_motor_speed(MOTOR_A, pwm);
    set_motor_speed(MOTOR_B, pwm);
}

/* 간단한 유턴: 제자리 180도 회전 (두번 90도) */
void perform_u_turn(void)
{
    // 180도: 두 번 90도 회전
    rotate_right_inplace(TURN_SPEED);
    HAL_Delay(TURN_90_TIME_MS);
    stop_all_motors();
    HAL_Delay(100);

    rotate_right_inplace(TURN_SPEED);
    HAL_Delay(TURN_90_TIME_MS);
    stop_all_motors();
    HAL_Delay(100);
}

/* 장애물 발견 시 회피 시퀀스:
   1) 정지
   2) 백업(짧게)
   3) 제자리 회전 90도 (우회전)
   4) 전진 (몸체 폭 만큼)
   5) 유턴(180)으로 라인 닫기(옵션)
*/
void R_avoidance_sequence(void)
{
    // 1) 정지
    stop_all_motors();
    HAL_Delay(50);

    // 2) 백업
    move_backward_pwm(BASE_SPEED);
    HAL_Delay(300); // 300ms 뒤로 (조정 필요)
    stop_all_motors();
    HAL_Delay(50);

    // 3) 제자리 우회전 90도
    rotate_right_inplace(TURN_SPEED);
    HAL_Delay(TURN_90_TIME_MS);
    stop_all_motors();
    HAL_Delay(50);

    // 4) 전진으로 통과
    move_forward_pwm(BASE_SPEED);
    HAL_Delay(BODY_MOVE_TIME_MS);
    stop_all_motors();
    HAL_Delay(50);

    // 3) 제자리 우회전 90도
        rotate_right_inplace(TURN_SPEED);
        HAL_Delay(TURN_90_TIME_MS);
        stop_all_motors();
        HAL_Delay(50);


}

void L_avoidance_sequence(void)
{
    // 1) 정지
    stop_all_motors();
    HAL_Delay(50);

    // 2) 백업
    move_backward_pwm(BASE_SPEED);
    HAL_Delay(300); // 300ms 뒤로 (조정 필요)
    stop_all_motors();
    HAL_Delay(50);

    // 3) 제자리 좌회전 90도
    rotate_left_inplace(TURN_SPEED);
    HAL_Delay(TURN_90_TIME_MS);
    stop_all_motors();
    HAL_Delay(50);

    // 4) 전진으로 통과
    move_forward_pwm(BASE_SPEED);
    HAL_Delay(BODY_MOVE_TIME_MS);
    stop_all_motors();
    HAL_Delay(50);

    // 3) 제자리 좌회전 90도
        rotate_left_inplace(TURN_SPEED);
        HAL_Delay(TURN_90_TIME_MS);
        stop_all_motors();
        HAL_Delay(50);


}




/**
  * @brief 모터 제어를 위해 필요한 초기 설정을 수행합니다.
  * @retval None
  */
void motor_control_init(void)
{
    // 1. PWM 출력 시작 (PA8: ENA, PA9: ENB)
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);



    // 2. 초기 모터 방향 설정 (정지)
    set_motor_direction(MOTOR_A, STOP);
    set_motor_direction(MOTOR_B, STOP);
    set_motor_direction(MOTOR_C, STOP);

    // 3. 초기 모터 속도 설정 (0)
    set_motor_speed(MOTOR_A, 0);
    set_motor_speed(MOTOR_B, 0);
    set_motor_speed(MOTOR_C, 0);

}

void MPU6050_Init(void) {
    uint8_t check;
    uint8_t data;

    // WHO_AM_I register read
    HAL_I2C_Mem_Read(&hi2c1, MPU6050_ADDR, 0x75, 1, &check, 1, 1000);

    if (check == 104) {  // 0x68
        data = 0;
        HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, PWR_MGMT_1, 1, &data, 1, 1000);
    }
}

void MPU6050_Read_Accel(int16_t *Ax, int16_t *Ay, int16_t *Az) {
    uint8_t Rec_Data[6];
    HAL_I2C_Mem_Read(&hi2c1, MPU6050_ADDR, ACCEL_XOUT_H, 1, Rec_Data, 6, 1000);

    *Ax = (int16_t)(Rec_Data[0] << 8 | Rec_Data[1]);
    *Ay = (int16_t)(Rec_Data[2] << 8 | Rec_Data[3]);
    *Az = (int16_t)(Rec_Data[4] << 8 | Rec_Data[5]);
}

// 엔코더 카운터 추적 변수 (이전 값과의 차이를 계산하기 위함)
static int prev_enc_L = 0;    // 이전 왼쪽 엔코더 값 (TIM3 카운터)
static int prev_enc_R = 0;    // 이전 오른쪽 엔코더 값 (TIM8 카운터)

/**
  * 엔코더 및 IMU를 이용해 로봇의 현재 위치(x, y, yaw)를 정수 단위로 업데이트합니다.
  */
void update_odometry(void)
{
   const float PI = 3.1415926535f;
       const int PERIOD = TIMER_PERIOD; // 65536

       // 1. 엔코더 카운터 값 읽기
       int current_enc_L = (int)__HAL_TIM_GET_COUNTER(&htim3);
       int current_enc_R = (int)__HAL_TIM_GET_COUNTER(&htim8);

       // 2. 카운터 변화량 계산 (Delta Pulse) - 랩어라운드 처리 적용
       int delta_L_pulse = current_enc_L - prev_enc_L;
       int delta_R_pulse = current_enc_R - prev_enc_R;

       //  랩어라운드 처리
       if (delta_L_pulse > (PERIOD / 2)) {
           delta_L_pulse -= PERIOD; // 언더플로우 발생 시
       } else if (delta_L_pulse < -(PERIOD / 2)) {
           delta_L_pulse += PERIOD; // 오버플로우 발생 시
       }

       if (delta_R_pulse > (PERIOD / 2)) {
           delta_R_pulse -= PERIOD;
       } else if (delta_R_pulse < -(PERIOD / 2)) {
           delta_R_pulse += PERIOD;
       }

       prev_enc_L = current_enc_L;
       prev_enc_R = current_enc_R;

       // 3. 펄스 변화량을 실제 이동 거리(mm)로 변환
       float distance_L_mm = (float)delta_L_pulse / ENCODER_RESOLUTION * (2.0f * PI * WHEEL_RADIUS_MM);
       float distance_R_mm = (float)delta_R_pulse / ENCODER_RESOLUTION * (2.0f * PI * WHEEL_RADIUS_MM);

       // 평균 이동 거리 및 각도 변화 계산
       float distance_center_mm = (distance_R_mm + distance_L_mm) / 2.0f;
       float delta_theta_rad = (distance_R_mm - distance_L_mm) / ROBOT_AXLE_LENGTH_MM; // 회전 각도(라디안)


       // 4. 좌표 (X, Y) 및 방향 (Yaw) 업데이트 (Middle Point 적분 방식 적용)

       // 4-1. 현재 Yaw를 라디안으로 변환
       float current_yaw_rad = g_robot_yaw * PI / 180.0f;

       // 4-2. 중간 각도 (Average Yaw) 계산: X, Y 계산에 사용 (정확도 향상)
       float avg_yaw_rad = current_yaw_rad + delta_theta_rad / 2.0f;

       // 4-3. X, Y 좌표 업데이트
       float delta_x = distance_center_mm * cosf(avg_yaw_rad);
       float delta_y = distance_center_mm * sinf(avg_yaw_rad);

       g_robot_x += delta_x;
       g_robot_y += delta_y;

       // 4-4. Yaw 각도 업데이트 및 정규화
       float next_yaw_rad = current_yaw_rad + delta_theta_rad;
       g_robot_yaw = next_yaw_rad * 180.0f / PI;

       // Yaw 각도 정규화 (0~359.999도)
       while (g_robot_yaw >= 360.0f) g_robot_yaw -= 360.0f;
       while (g_robot_yaw < 0.0f) g_robot_yaw += 360.0f;
}



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

// WizFi360에 AT 문자열만 보내고, 응답은 읽지 않는 버전 publish와 subscribe 시, 버퍼를 공동으로 사용하여 충돌 일어나는 걸
// 방지하기 위해서 별도로 꼭 필요하다.
void WizFi_SendOnly(UART_HandleTypeDef *huart_wiz,
                    UART_HandleTypeDef *huart_term,
                    const char *command)
{
    // 디버깅용으로 어떤 명령 보냈는지 터미널에만 찍고
    char tx_log[64];
    int len = snprintf(tx_log, sizeof(tx_log),
                       "\r\n[CMD-TX] %s\r\n", command);
    HAL_UART_Transmit(huart_term, (uint8_t *)tx_log, len, HAL_MAX_DELAY);

    // WizFi360으로 명령 전송
    char cmd_with_crlf[128];
    snprintf(cmd_with_crlf, sizeof(cmd_with_crlf), "%s\r\n", command);
    HAL_UART_Transmit(huart_wiz,
                      (uint8_t *)cmd_with_crlf,
                      strlen(cmd_with_crlf),
                      500);
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



/*
void Setup_WiFi_And_MQTT(void)
{
    // 1. Wi-Fi 연결
    Send_AT_Command(&huart3, &huart2, "AT+CWJAP=\"S24\",\"dial8787@@\"", wizfi_rx_buffer, WIZFI360_MAX_RESPONSE_SIZE, 8000);
    HAL_Delay(5000);

    // 2. TCP 연결 (로컬 Mosquitto 브로커)
    Send_AT_Command(&huart3, &huart2, "AT+CIPSTART=\"TCP\",\"192.168.178.61\",1883", wizfi_rx_buffer, WIZFI360_MAX_RESPONSE_SIZE, 5000);
    HAL_Delay(3000);

    // 3. MQTT 연결 설정 (User/Password/ClientID/KeepAlive)
    // 브로커 인증이 없으면 "" 사용
    Send_AT_Command(&huart3, &huart2, "AT+MQTTSET=\"\",\"\",\"STM32Client\",300", wizfi_rx_buffer, WIZFI360_MAX_RESPONSE_SIZE, 2000);
    HAL_Delay(1000);

    // 4. MQTT 토픽 설정
    // Publish: "vibeclean/robot1/telemetry", Subscribe: "vibeclean/robot1/control/#"
    Send_AT_Command(&huart3, &huart2,
        "AT+MQTTTOPIC=\"vibeclean/robot1/telemetry\",\"vibeclean/robot1/control/#\"",
        wizfi_rx_buffer, WIZFI360_MAX_RESPONSE_SIZE, 2000);


    // 4-1. Publish QoS 설정 (예: QoS1)
    Send_AT_Command(&huart3, &huart2, "AT+MQTTQOS=1", wizfi_rx_buffer, WIZFI360_MAX_RESPONSE_SIZE, 1000);
    HAL_Delay(500);

    // 5. MQTT CONNECT (Single connection, 인증 없음)
    Send_AT_Command(&huart3, &huart2, "AT+MQTTCON=0,\"192.168.178.61\",1883", wizfi_rx_buffer, WIZFI360_MAX_RESPONSE_SIZE, 5000);
    HAL_Delay(3000);

    // 6. MQTT 구독 (토픽은 이미 AT+MQTTTOPIC로 설정)
    Send_AT_Command(&huart3, &huart2, "AT+MQTTSUB", wizfi_rx_buffer, WIZFI360_MAX_RESPONSE_SIZE, 3000);
    HAL_Delay(1000);
}
*/


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

uint8_t MQTT_PublishJSON(
    UART_HandleTypeDef *huart_wiz,
    UART_HandleTypeDef *huart_term,
    const char *jsonMsg,
    uint8_t *resp, uint16_t resp_size
) {
    char cmd[256];
    snprintf(cmd, sizeof(cmd),
             "AT+MQTTPUB=\"%s\"", jsonMsg);

    //Send_AT_Command(huart_wiz, huart_term, cmd, resp, resp_size, 3000);
    WizFi_SendOnly(huart_wiz, huart_term, cmd);
    //return strstr((char*)resp, "OK") != NULL;
    return 1;
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
        resp, sizeof(resp), 15000);

    // -------- MQTT 설정 --------
    if (!MQTT_SetConfig(huart_wiz, huart_term,
                        "", "", "vibeclean01", 300,
                        resp, sizeof(resp))) return 0;

    if (!MQTT_SetTopics(huart_wiz, huart_term,
                        "vibeclean/robot1/telemetry",
                        "vibeclean/robot1/control/#",
                        resp, sizeof(resp))) return 0;

    if (!MQTT_SetQos(huart_wiz, huart_term, 0, resp, sizeof(resp)))
        return 0;

    if (!MQTT_ConnectBroker(huart_wiz, huart_term,
                            "192.168.178.61", 1883,
                            resp, sizeof(resp)))
        return 0;

    return 1;
}


void Publish_Message(void)
{
    char cmd[256];

    const char *json_message =
        "{\"currentFloor\":\"Carpet\",\"fanSpeed\":3,"
        "\"position\":{\"x\":10,\"y\":5},"
        "\"sensor\":{\"x\":0.1,\"y\":0.2,\"z\":0.3}}";

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

// WizFi360으로부터 들어오는 subscribe 메시지 처리
void MQTT_ProcessIncoming(void)
{
    uint8_t ch;

    // timeout=0 으로 non-blocking 폴링
    while (HAL_UART_Receive(&huart3, &ch, 1, 0) == HAL_OK) {
        if (mqtt_rx_len < (int)sizeof(mqtt_rx_buf) - 1) {
            mqtt_rx_buf[mqtt_rx_len++] = (char)ch;
            mqtt_rx_buf[mqtt_rx_len] = '\0';
        }

        // 한 줄 끝(일반적으로 \n 기준)이면 파싱
        if (ch == '\n') {
            ParseMqttLine(mqtt_rx_buf, mqtt_rx_len);
            mqtt_rx_len = 0;
            mqtt_rx_buf[0] = '\0';
        }
    }
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

    // ★ 마지막 라인이 '\n' 없이 종료되었을 때 처리
    if (pos > 0) {
        line[pos] = '\0';
        if (strstr(line, "->")) {
            ParseMqttLine(line, strlen(line));
        }
    }
}


/*void ApplyManualControl(void)
{
    if (!g_powerOn) {
        // 전원 OFF면 무조건 정지
        stop_all_motors();
        return;
    }

    if (!g_modeManual) {
        // AUTO 모드에서는 수동 방향 명령 무시
        return;
    }

    // fanSpeed(0~3)를 PWM으로 단순 매핑 (원하는 대로 조정 가능)
    uint16_t pwm = 0;
    switch (g_fanSpeed) {
        case 0: pwm = 0;                break;
        case 1: pwm = BASE_SPEED / 2;   break;
        case 2: pwm = BASE_SPEED;       break;
        case 3: pwm = BASE_SPEED * 3 / 2; break;
    }

    if (strcmp(g_direction, "FWD") == 0) {
        move_forward_pwm(pwm);
    } else if (strcmp(g_direction, "BACK") == 0) {
        move_backward_pwm(pwm);
    } else if (strcmp(g_direction, "LEFT") == 0) {
        rotate_left_inplace(pwm);
    } else if (strcmp(g_direction, "RIGHT") == 0) {
        rotate_right_inplace(pwm);
    } else { // STOP 혹은 기타
        stop_all_motors();
    }
}*/


/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
//Putty 작동 테스트용 변수
   int countL, countR; //  엔코더 카운트 저장 변수
   char debug_msg[100]; // 디버깅 메시지 버퍼

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
  /* USER CODE BEGIN 2 */
  // DWT 초기화 (마이크로초 측정을 위해 필수)
  DWT_Init();
  UART_Printf("STM32 HC-SR04 Measurement Ready (Trig: PA0, Echo: PA1)\r\n");




  motor_control_init(); // <- 반드시 호출 (PWM Start + 초기화)
  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);

  //엔코더 카운팅 시작
  HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL);
  HAL_TIM_Encoder_Start(&htim8, TIM_CHANNEL_ALL);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  int failCount = 0;
  int tempAovoid = 0; // 0 = right, 1 = left

  char post_data_buffer[150]; //json 문자열 버퍼


//      ESP_Connect_WiFi("S24", "dial8787@@");
//
//      ESP_MQTT_Connect("stmClient", "broker.hivemq.com", 1883);
//
//      ESP_MQTT_Publish("stm32/test", "hello mqtt");

      // WiFi 연결 정보





////  MPU6050_Init();
//
//  float d1, d2, d3;
//  int16_t Ax, Ay, Az;
//  char buf[100];
//  // 1️⃣ Wi-Fi 연결
//     ESP_Init("YOUR_WIFI_SSID", "YOUR_WIFI_PASSWORD");
//     char value[64];  // 서버로부터 받은 값을 저장할 버퍼
//
//     // 2️⃣ GET 요청 및 값 파싱
//     if (ESP_HTTP_Get_Value("192.168.178.61", "/api/manual/speed", "fanSpeed", value))
//     {
//         char msg[128];
//         sprintf(msg, "fanSpeed value = %s\r\n", value);
//         Uart_sendstring(msg, &huart2);
//     }
//     else
//     {
//         Uart_sendstring("HTTP GET Failed\r\n", &huart2);
//     }
//
//  // POST
//  ESP_HTTP_Post("192.168.178.61", "/data", "{\"id\":1,\"value\":42}");
//
//  float distance1, distance2, distance3;


//  char *start_msg = "STM32 WizFi360 AT Test Start! (USART2 for PuTTY, USART1 for WizFi360)\r\n";
//  HAL_UART_Transmit(&huart2, (uint8_t *)start_msg, strlen(start_msg), HAL_MAX_DELAY);
//
//  // WizFi360 모듈 부팅 시간 대기 (필수)
//  char *boot_msg = "[INFO] Waiting 3 seconds for WizFi360 boot up...\r\n";
//  HAL_UART_Transmit(&huart2, (uint8_t *)boot_msg, strlen(boot_msg), HAL_MAX_DELAY);
//  HAL_Delay(3000);

  char *start_msg = "STM32 WizFi360 MQTT Test Start! (PuTTY=USART2, WizFi360=USART3)\r\n";
//  HAL_UART_Transmit(&huart2, (uint8_t *)start_msg, strlen(start_msg), HAL_MAX_DELAY);

  HAL_Delay(3000); // WizFi360 부팅 대기

//  Setup_WiFi_And_MQTT();

  MQTT_Init_All(&huart3, &huart2);   // Wiz: huart1, Terminal: huart2


  // JSON 텔레메트리 예시 publish
  const char *json =
      "{\"currentFloor\":\"Carpet\",\"fanSpeed\":3,"
      "\"position\":{\"x\":10,\"y\":5},"
      "\"sensor\":{\"x\":0.1,\"y\":0.2,\"z\":0.3}}";

  MQTT_PublishJSON(&huart3, &huart2, json, rx, sizeof(rx));

  uint32_t last_pub_tick = 0;


  while (1)
  {


      Publish_Message();
      HAL_Delay(100); // 5초마다 메시지 발행
      // 2) control 상태값에 따라 모터 제어
      //ApplyManualControl(); 이미 특정 변수에 값 넣는 거로 존재함.

      // 1) control 토픽에서 들어온 메시지 처리
//      MQTT_ProcessIncoming(); // 여기서 다 받고 이 함수 안에서 받은 거를 parse함수로 넘김
      // parse에서는 json형태의 값을 topic과 payload로 분리함.
      // 파싱 후 hadler함수를 호출하여 받은 topic를 비교해 해당 토픽 if문에 payload를 반영한 변수 값 갱심

      // 3) 일정 주기로 telemetry publish (예: 2초)  // 이미 위에서 publish_Message하고 HAL_DELAY2초 걸어둬서 우선 주석
      /*uint32_t now = HAL_GetTick();
           if (now - last_pub_tick >= 2000) {
                Publish_Message();
             last_pub_tick = now;
           }

      HAL_Delay(10);*/  // 너무 빡세지 않게 약간 쉬게

//       // 1. WizFi360 연결 확인 (응답: OK)
//       Send_AT_Command(&huart3, &huart2, "AT", wizfi_rx_buffer, WIZFI360_MAX_RESPONSE_SIZE, 1000);
//       HAL_Delay(3000); // 다음 명령 전송까지 3초 대기
//
//       // 2. 펌웨어 버전 정보 요청 (응답: AT+GMR, OK)
//       Send_AT_Command(&huart3, &huart2, "AT+GMR", wizfi_rx_buffer, WIZFI360_MAX_RESPONSE_SIZE, 1000);
//       HAL_Delay(3000);
//
//       // 3. Wi-Fi 모드 확인 (응답: AT+CWMODE_CUR=1/2/3, OK)
//       Send_AT_Command(&huart3, &huart2, "AT+CWMODE_CUR?", wizfi_rx_buffer, WIZFI360_MAX_RESPONSE_SIZE, 1000);
//       HAL_Delay(3000);
//
//       char *loop_end = "\r\n[INFO] AT Command sequence finished. Repeating in 10s...\r\n";
//       HAL_UART_Transmit(&huart2, (uint8_t *)loop_end, strlen(loop_end), HAL_MAX_DELAY);
//       HAL_Delay(10000); // 모든 명령 후 10초 대기

          //x,y 좌표 확인 Putty 테스트 코드2
          // main 함수 시작부분의 Putty 작동 테스트용 변수와 이하 코드 주석 해제한 후,
          // 이외 while (1) 내용 주석처리 하면 Putty 테스트 가능합니다.
         // 1. 오도메트리 업데이트 (현재 X, Y, Yaw 계산)
//         update_odometry();

         // 2. 시리얼 포트로 X, Y 좌표 출력 (테스트용)
         // float 값을 소수점 두 자리까지 출력하여 정확도를 확인합니다.
//         sprintf(debug_msg, "X: %.2f | Y: %.2f | Yaw: %.1f\r\n",
//                 g_robot_x, g_robot_y, g_robot_yaw);
//         UART_Printf(debug_msg);
//         HAL_Delay(100); // ms마다 업데이트 확인



//               //실제 사용할 2D Mapping 좌표전송 코드 (통신구축 완료 후 수정필요)
//             HAL_Delay(5000);  // 5초마다 재요청 가능
//             ESP_HTTP_Get_Value("192.168.178.61", "/api/manual/speed", "fanSpeed", value);
//
//             char msg[128];
//             sprintf(msg, "fanSpeed = %s\r\n", value);
//             Uart_sendstring(msg, &huart2);
//
//             // 1. 오도메트리 업데이트 (좌표 계산)
//                 update_odometry();
//
//             // 2. 백엔드로 보낼 JSON 데이터 생성
//                 int x_int = (int)roundf(g_robot_x);
//                 int y_int = (int)roundf(g_robot_y);
//
//             // JSON 형식으로 포맷팅
//                 sprintf(post_data_buffer,
//                         "{\"robotId\":1,\"x\":%d,\"y\":%d}",
//                         x_int, y_int);
//
//             // 3. POST 요청으로 서버에 데이터 전송
//                 ESP_HTTP_Post("192.168.178.61", "/api/robot/location", post_data_buffer);
//
//             // 4. 전송 주기 설정
//                 HAL_Delay(100); // 100ms (0.1초) 주기로 전송


//     MPU6050_Read_Accel(&Ax, &Ay, &Az);
//
//         // g 단위 변환
//         float ax_g = Ax / 16384.0f;
//         float ay_g = Ay / 16384.0f;
//         float az_g = Az / 16384.0f;
//
//         // Roll / Pitch 계산
//         float roll  = atan2f(ay_g, az_g) * 180.0f / 3.14159265f;
//         float pitch = atan2f(-ax_g, sqrtf(ay_g*ay_g + az_g*az_g)) * 180.0f / 3.14159265f;
//
//         // UART로 출력
//         char buf[100];
//         sprintf(buf, "Roll: %.2f  Pitch: %.2f\r\n", roll, pitch);
//         HAL_UART_Transmit(&huart2, (uint8_t*)buf, strlen(buf), 1000);
//
//         HAL_Delay(500);
//      //전진 유지
//     move_forward_pwm(BASE_SPEED);
//
//         d1 = HCSR04_Read(TRIG_PORT, TRIG_PIN, ECHO_PORT, ECHO_PIN);
//         DWT_Delay_us(5000);
//         d2 = HCSR04_Read(TRIG_PORT1, TRIG_PIN1, ECHO_PORT1, ECHO_PIN1);
//         DWT_Delay_us(5000);
//         d3 = HCSR04_Read(TRIG_PORT2, TRIG_PIN2, ECHO_PORT2, ECHO_PIN2);
//
//         UART_Printf("S1: %.1f cm | S2: %.1f cm | S3: %.1f cm\r\n", d1, d2, d3);
//
//         // 센서값 모두 0이면 일시적인 에러로 간주
//         if ((d1 > 1 && d1 <= WALL_DISTANCE_THRESHOLD)
//                   || (d2 > 1 && d2 <= WALL_DISTANCE_THRESHOLD)
//                   || (d3 > 1 && d3 <= WALL_DISTANCE_THRESHOLD)) {
//             failCount++;
//             if (failCount > 5) { // 연속 3회 이상이면 진짜 장애물일 수도 있음
//                 stop_all_motors();
//                 HAL_Delay(50);
//                 if(tempAovoid == 0){
//                    R_avoidance_sequence();
//                    tempAovoid = 1;
//                 }
//                 else{
//                    L_avoidance_sequence();
//                    tempAovoid = 0;
//
//                 }
//                 failCount = 0;
//             }
//         } else {
//             failCount = 0;
//         }
//
//
//         HAL_Delay(100);
//
//

     /* USER CODE BEGIN WHILE */


//      모터 c 전진 2s
//         set_motor_direction(MOTOR_C, FORWARD);
//
//         set_motor_speed(MOTOR_C, 900);
//
//         HAL_Delay(3000);
//
//         set_motor_speed(MOTOR_C, 0);
//         HAL_Delay(500);
//
//      모터 A 전진 2s
//         set_motor_direction(MOTOR_A, FORWARD);
//         set_motor_direction(MOTOR_B, FORWARD);
//
//         set_motor_speed(MOTOR_A, 400);
//         set_motor_speed(MOTOR_B, 400);
//
//         HAL_Delay(1000);
//
//         set_motor_speed(MOTOR_A, 0);
//         HAL_Delay(500);
//
//         // 모터 A 후진 2s
//         set_motor_direction(MOTOR_A, BACKWARD);
//         set_motor_direction(MOTOR_B, BACKWARD);
//
//         set_motor_speed(MOTOR_A, 400);
//         set_motor_speed(MOTOR_B, 400);
//
//         HAL_Delay(1000);
//
//         set_motor_speed(MOTOR_A, 0);
//         set_motor_speed(MOTOR_B, 0);
//
//         HAL_Delay(1000);
//
//      센서 1에서 거리 읽기
//         distance1 = HCSR04_Read(TRIG_PORT, TRIG_PIN, ECHO_PORT, ECHO_PIN);
//
//         // 센서 2에서 거리 읽기
//         distance2 = HCSR04_Read(TRIG_PORT1, TRIG_PIN1, ECHO_PORT1, ECHO_PIN1);
//
//         // 센서 2에서 거리 읽기
//         distance3 = HCSR04_Read(TRIG_PORT2, TRIG_PIN2, ECHO_PORT2, ECHO_PIN2);
//
//         // 결과 출력
//         if (distance1 > 0)
//             UART_Printf("S1: %.2f cm  ", distance1);
//         else
//             UART_Printf("S1: Fail  ");
//
//         if (distance2 > 0)
//             UART_Printf("S2: %.2f cm  ", distance2);
//         else
//             UART_Printf("S2: Fail\r\n");
//
//         if (distance3 > 0)
//                      UART_Printf("S3: %.2f cm\r\n", distance3);
//                  else
//                      UART_Printf("S3: Fail\r\n");
//
//         HAL_Delay(500);
    /* USER CODE END WHILE */

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
  hi2c1.Init.ClockSpeed = 100000;
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
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
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
  htim4.Init.Period = 65535;
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
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_12, GPIO_PIN_RESET);

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

  /*Configure GPIO pins : PB0 PB1 PB2 PB12 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_12;
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
