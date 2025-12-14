/*
 * odom_imu.c
 *
 * Created on: 2025. 12. 03.
 * Author: User
 */

#include "odom_imu.h"
#include <math.h>

// ==========================================
// [전역 변수 정의]
// ==========================================
volatile float g_robot_x = 0.0f;
volatile float g_robot_y = 0.0f;
volatile float g_robot_yaw = 0.0f;

// ==========================================
// [내부 변수]
// ==========================================
static I2C_HandleTypeDef *odo_hi2c;
static TIM_HandleTypeDef *odo_htim_L;
static TIM_HandleTypeDef *odo_htim_R;

static int16_t prev_enc_L = 0;
static int16_t prev_enc_R = 0;

static float gyro_z_offset = 0.0f;
static float m_per_pulse = 0.0f;

// ==========================================
// [내부 함수] MPU6050 자이로 Z축 Raw 읽기
// ==========================================
static int16_t Read_Gyro_Z_Raw(void) {
    uint8_t buffer[2];
    HAL_I2C_Mem_Read(odo_hi2c, MPU6050_ADDR, 0x47, 1, buffer, 2, 10);
    return (int16_t)((buffer[0] << 8) | buffer[1]);
}

// ==========================================
// [초기화 함수]
// ==========================================
void Odom_IMU_Init(I2C_HandleTypeDef *hi2c, TIM_HandleTypeDef *htim_L, TIM_HandleTypeDef *htim_R) {
    odo_hi2c = hi2c;
    odo_htim_L = htim_L;
    odo_htim_R = htim_R;

    g_robot_x = 0.0f;
    g_robot_y = 0.0f;
    g_robot_yaw = 0.0f;

    // 1. 펄스당 이동 거리 계산 (Meter 단위로 변환)
    // 공식: (2 * PI * 반지름_m) / 펄스수
    float radius_m = WHEEL_RADIUS_MM / 1000.0f; // mm -> m 변환
    m_per_pulse = (2.0f * 3.141592f * radius_m) / ENCODER_RESOLUTION;

    // 2. 엔코더 초기값 읽기
    prev_enc_L = (int16_t)__HAL_TIM_GET_COUNTER(odo_htim_L);
    prev_enc_R = (int16_t)__HAL_TIM_GET_COUNTER(odo_htim_R);

    // 3. 자이로 오프셋 캘리브레이션
    int32_t sum = 0;
    for (int i = 0; i < GYRO_Z_OFFSET_COUNT; i++) {
        sum += Read_Gyro_Z_Raw();
        HAL_Delay(5);
    }
    gyro_z_offset = (float)sum / GYRO_Z_OFFSET_COUNT;
}

// ==========================================
// [업데이트 함수] - 50Hz 인터럽트용
// ==========================================
void Odom_IMU_Update_IT(void) {
    // 1. 엔코더 변화량 (int16_t 오버플로우 자동 처리)
    int16_t curr_enc_L = (int16_t)__HAL_TIM_GET_COUNTER(odo_htim_L);
    int16_t curr_enc_R = (int16_t)__HAL_TIM_GET_COUNTER(odo_htim_R);

    int16_t delta_pulse_L = curr_enc_L - prev_enc_L;
    int16_t delta_pulse_R = curr_enc_R - prev_enc_R;

    prev_enc_L = curr_enc_L;
    prev_enc_R = curr_enc_R;

    // [중요] 모터 방향 확인: 전진 시 pulse가 증가(+) 해야 함.
    // 반대라면 아래 주석 해제하여 부호 반전
    // delta_pulse_L = -delta_pulse_L;
    // delta_pulse_R = -delta_pulse_R;

    // 2. 이동 거리 계산 (m)
    float dist_L = delta_pulse_L * m_per_pulse;
    float dist_R = delta_pulse_R * m_per_pulse;
    float center_dist = (dist_L + dist_R) * 0.5f;

    // 3. 회전 각도 계산 (Gyro Z 적분)
    int16_t gyro_z_raw = Read_Gyro_Z_Raw();

    // (Raw - Offset) / Sensitivity
    // MPU6050 기본값(±250dps) 기준 Sensitivity = 131.0
    float gyro_dps = ((float)gyro_z_raw - gyro_z_offset) / 131.0f;

    // dps -> rad/s -> rad (적분)
    float delta_yaw = gyro_dps * 0.0174533f * ODOMETRY_DT;

    // 4. 좌표 업데이트
    g_robot_yaw += delta_yaw;

    // 각도 정규화 (-PI ~ PI)
    if (g_robot_yaw > 3.141592f) g_robot_yaw -= 6.283184f;
    else if (g_robot_yaw < -3.141592f) g_robot_yaw += 6.283184f;

    g_robot_x += center_dist * cosf(g_robot_yaw);
    g_robot_y += center_dist * sinf(g_robot_yaw);
}
