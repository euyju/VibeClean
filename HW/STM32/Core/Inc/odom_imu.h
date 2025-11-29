/*
 * odom_imu.h
 * 오도메트리(엔코더) 및 IMU(MPU6050) 센서 융합을 위한 헤더 파일
 */

#ifndef INC_ODOM_IMU_H_
#define INC_ODOM_IMU_H_

#include "main.h" // HAL 함수 사용을 위해 main.h 포함
#include "mpu6050.h"

// =========================================================================
// 1. MPU6050 레지스터 및 상수 정의
// =========================================================================
#define GYRO_XOUT_H 0x43        // MPU6050 자이로 출력 주소
#define MPU6050_GYRO_SCALE 16.4f // ±2000 deg/s 기준 (float 형태로 수정)
#define ODOMETRY_DT 0.1f         // 오도메트리 업데이트 주기 (초)
#define FUSION_ALPHA 0.98f       // 상보 필터 자이로 가중치 (0.0~1.0)
#define M_PI 3.1415926535f       // 원주율 (M_PI 정의가 없을 경우)


// =========================================================================
// 2. 오도메트리 물리 상수 정의 (float 형태로 수정)
// =========================================================================
#define WHEEL_RADIUS_MM     30.0f      // 바퀴 반지름 (mm)
#define ROBOT_AXLE_LENGTH_MM 160.0f    // 바퀴 축 간 거리 (mm)
#define ENCODER_RESOLUTION  1000.0f    // 엔코더 해상도 (펄스 수)
#define TIMER_PERIOD        65536      // TIM3/TIM8 주기


// =========================================================================
// 3. 전역 변수 선언 (extern)
// =========================================================================
extern float g_robot_x;
extern float g_robot_y;
extern float g_robot_yaw;
extern float gyro_bias_Z;

// 엔코더 카운터 추적 변수
extern int prev_enc_L;
extern int prev_enc_R;


// =========================================================================
// 4. 함수 프로토타입 선언
// =========================================================================
void MPU6050_Read_Accel(int16_t *Ax, int16_t *Ay, int16_t *Az);
void MPU6050_Read_Gyro(int16_t *Gx, int16_t *Gy, int16_t *Gz);
void calculate_gyro_bias(void);
void update_odometry(void);

#endif /* INC_ODOM_IMU_H_ */
