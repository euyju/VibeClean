/*
 * odom_imu.h
 *
 * Created on: 2025. 12. 03.
 * Author: User
 */

#ifndef INC_ODOM_IMU_H_
#define INC_ODOM_IMU_H_

#include "main.h"

// ==========================================
// [사용자 하드웨어 설정]
// ==========================================

// 1. 로봇 기구학 정보 (MM 단위 입력)
#define WHEEL_RADIUS_MM       30.0f      // 바퀴 반지름 (30mm)
#define ROBOT_AXLE_LENGTH_MM  160.0f     // 두 바퀴 사이 거리 (160mm) - 참고용
#define ENCODER_RESOLUTION    1000.0f    // 바퀴 1회전 당 펄스 수 (PPR)
                                         // 주의: STM32 타이머가 4체배 모드라면
                                         // 실제 카운트는 PPR * 4 일 수 있으므로 확인 필요!
                                         // 여기 입력값은 "타이머가 1회전 시 실제 세는 총 카운트"여야 함.

// 2. MPU6050 설정
#define MPU6050_ADDR          (0x68 << 1)
#define GYRO_Z_OFFSET_COUNT   100        // 초기 캘리브레이션 샘플 수

// 3. 타이머 주기 (TIM7 설정값과 일치해야 함)
#define ODOMETRY_FREQ         50.0f      // 50Hz
#define ODOMETRY_DT           (1.0f / ODOMETRY_FREQ) // 0.02초

// ==========================================
// [전역 변수] - main.c에서 extern으로 참조
// ==========================================
extern volatile float g_robot_x;    // 로봇 X 위치 (m)
extern volatile float g_robot_y;    // 로봇 Y 위치 (m)
extern volatile float g_robot_yaw;  // 로봇 헤딩 각도 (rad)

// ==========================================
// [함수 프로토타입]
// ==========================================
void Odom_IMU_Init(I2C_HandleTypeDef *hi2c, TIM_HandleTypeDef *htim_L, TIM_HandleTypeDef *htim_R);
void Odom_IMU_Update_IT(void);
void Odom_IMU_Reset(float x, float y, float yaw);

#endif /* INC_ODOM_IMU_H_ */
