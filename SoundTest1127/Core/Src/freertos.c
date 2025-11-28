/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
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
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "cmsis_os.h"  // <--- 이 줄을 추가해야 osDelay를 인식합니다.
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
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

// 신호용 변수 가져오기
extern volatile uint8_t g_imu_read_flag;
// AI 버퍼 관련 변수들 가져오기 (main.c에 있다고 가정)
extern float edge_ai_buffer[];
extern volatile uint16_t edge_ai_sample_index;
extern volatile uint8_t edge_ai_buffer_ready;
extern I2C_HandleTypeDef hi2c1; // I2C 핸들

// 디버깅 변수들
extern volatile uint8_t imu_debug_counter;
extern volatile uint8_t imu_debug_ready;
extern float imu_debug_ax, imu_debug_ay, imu_debug_az;

// 상수 (main.h에 없으면 여기에 정의)
#ifndef EDGE_AI_SAMPLE_COUNT
#define EDGE_AI_SAMPLE_COUNT 200
#endif

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE BEGIN Application */

// ==========================================
// 1. main.c에 있는 변수와 함수들 가져오기 (extern)
// ==========================================

// [변수 가져오기]
extern volatile float g_d1, g_d2, g_d3;
extern volatile int g_obstacle_detected;
extern volatile int g_powerOn;
extern volatile uint16_t g_target_suction_pwm;
extern volatile uint8_t edge_ai_buffer_ready; // AI 버퍼 플래그

// [함수 가져오기]
// (main.h에 선언되어 있다면 생략 가능하지만, 안전하게 적어둠)
extern void move_forward_pwm(uint16_t pwm);
extern void move_backward_pwm(uint16_t pwm);
extern void stop_all_motors(void);
extern void clean_go_pwm(uint16_t pwm);
extern void rotate_right_inplace(uint16_t pwm);
extern void rotate_left_inplace(uint16_t pwm);
extern void set_motor_speed(uint8_t motor_id, uint16_t speed);
extern float HCSR04_Read(GPIO_TypeDef *trigPort, uint16_t trigPin, GPIO_TypeDef *echoPort, uint16_t echoPin);
extern void UART_Printf(const char *format, ...); // main.c의 매크로가 아닌 실제 함수 필요
extern uint8_t MQTT_Init_All(UART_HandleTypeDef *huart_wiz, UART_HandleTypeDef *huart_term);
extern void Publish_Message(void);

// UART 핸들 (MQTT 초기화용)
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;
// 타이머 핸들 (AI용)
extern TIM_HandleTypeDef htim6;

// 상수 정의 (main.c와 동일하게)
#define BASE_SPEED 1000
#define TURN_SPEED 300
#define TURN_90_TIME_MS 3200

// 핀 정의 (main.h에 없으면 여기에 다시 정의 필요, 보통 main.h에 있음)
// 만약 에러 나면 main.h를 확인하거나 여기에 #define 복사

// ==========================================
// 2. 태스크 함수 구현 (실제 로직)
// ==========================================
/* 1. MotorTask : 모터 제어 및 상태 출력 */
void StartMotorTask(void *argument)
{
  // 시작할 때 한 번 출력
  UART_Printf("[MotorTask] Started!\r\n");

  for(;;)
  {
    if (g_obstacle_detected) {
        UART_Printf("[Motor] Obstacle! Avoiding...\r\n");
        stop_all_motors();
        osDelay(100);

        move_backward_pwm(BASE_SPEED);
        osDelay(500);

        rotate_right_inplace(TURN_SPEED);
        osDelay(TURN_90_TIME_MS);

        stop_all_motors();
        g_obstacle_detected = 0;
    }
    else {
        // g_powerOn이 1이어야 움직임 (기본값 확인 필요)
        if (g_powerOn) {
             // 디버깅: 너무 자주 찍으면 느려지니 주석 처리하거나 가끔 찍기
             // UART_Printf("[Motor] Moving Forward...\r\n");
             move_forward_pwm(BASE_SPEED);
             clean_go_pwm(g_target_suction_pwm);
        } else {
             // UART_Printf("[Motor] Stop (Power OFF)\r\n");
             stop_all_motors();
        }
    }
    osDelay(10);
  }
}

/* 2. SensorTask : 초음파 값 측정 및 PuTTY 출력 */
void StartSensorTask(void *argument)
{
  UART_Printf("[SensorTask] Started!\r\n");

  for(;;)
  {
    float d1 = HCSR04_Read(TRIG_PORT, TRIG_PIN, ECHO_PORT, ECHO_PIN);
    osDelay(10);
    float d2 = HCSR04_Read(TRIG_PORT1, TRIG_PIN1, ECHO_PORT1, ECHO_PIN1);
    osDelay(10);
    float d3 = HCSR04_Read(TRIG_PORT2, TRIG_PIN2, ECHO_PORT2, ECHO_PIN2);

    g_d1 = d1; g_d2 = d2; g_d3 = d3;

    // [디버깅] PuTTY에 거리 값 출력 (0.5초마다)
    UART_Printf("[Sens] D1:%.1f D2:%.1f D3:%.1f\r\n", d1, d2, d3);

    // 장애물 판단 (테스트를 위해 거리를 좀 넉넉하게 30cm)
    if ((d1 > 1.0f && d1 < 30.0f) ||
        (d2 > 1.0f && d2 < 30.0f) ||
        (d3 > 1.0f && d3 < 30.0f))
    {
        g_obstacle_detected = 1;
    }

    osDelay(500); // 출력 보려고 0.5초로 늦춤. 테스트 후 50으로 변경
  }
}

/* 3. CommTask : AT 명령어가 자동으로 출력됨 */
void StartCommTask(void *argument)
{
  osDelay(2000); // 다른 태스크들 자리 잡을 때까지 대기

  UART_Printf("[CommTask] Starting WizFi Init...\r\n");

  // 이 함수 내부에서 AT 명령어를 보낼 때마다 UART_Printf가 호출되어
  // PuTTY에 [CMD] ..., [RSP] ... 가 뜹니다.
  MQTT_Init_All(&huart3, &huart2);

  for(;;)
  {
    Publish_Message();
    osDelay(2000);
  }
}

/* 4. AITask */
void StartAITask(void *argument)
{
  // 타이머 시작 (이게 돌아야 g_imu_read_flag가 1이 됨)
  HAL_TIM_Base_Start_IT(&htim6);

  UART_Printf("[AI] Timer Started. Waiting for data...\r\n");

  for(;;)
  {
    // 1. 타이머가 "시간 됐다"고 신호를 줬는지 확인
    if (g_imu_read_flag == 1) {

        g_imu_read_flag = 0; // 신호 확인했으니 깃발 내리기

        // 2. 여기서 안전하게 I2C 통신 수행
        if (!edge_ai_buffer_ready) {
            float ax, ay, az;

            // ★ 인터럽트 밖이므로 여기서 I2C를 써도 안전합니다 ★
            if (MPU6050_ReadAccel(&hi2c1, &ax, &ay, &az) == HAL_OK) {

                // 버퍼에 저장
                edge_ai_buffer[edge_ai_sample_index * 3 + 0] = ax * 16384.0f;
                edge_ai_buffer[edge_ai_sample_index * 3 + 1] = ay * 16384.0f;
                edge_ai_buffer[edge_ai_sample_index * 3 + 2] = az * 16384.0f;

                // 디버깅용 카운터
                imu_debug_counter++;
                if (imu_debug_counter >= 10) {
                    imu_debug_ax = ax;
                    imu_debug_ay = ay;
                    imu_debug_az = az;
                    imu_debug_ready = 1;
                    imu_debug_counter = 0;
                }

                edge_ai_sample_index++;

                // 버퍼 꽉 찼는지 확인
                if (edge_ai_sample_index >= EDGE_AI_SAMPLE_COUNT) {
                    edge_ai_buffer_ready = 1;
                    edge_ai_sample_index = 0;

                    // (옵션) 여기서 추론 함수 호출 가능
                    // UART_Printf("[AI] Buffer Full! Ready to inference.\r\n");
                }
            }
        }
    }

    // 3. AI 추론 로직 (버퍼가 찼을 때)
    if (edge_ai_buffer_ready) {
        UART_Printf("[AI] Inferencing...\r\n");
        // edge_ai_classify(...);
        edge_ai_buffer_ready = 0; // 다시 수집 시작
    }

    // 매우 중요: 타이머 깃발을 기다리기 위해 아주 짧게 대기
    osDelay(1);
  }
}

/* USER CODE END Application */

