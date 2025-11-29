/*
 * odom_imu.c
 * 오도메트리(엔코더) 및 IMU(MPU6050) 센서 융합 로직 구현
 */
#include "main.h"
#include "odom_imu.h"
#include <math.h> // cosf, sinf 사용
#include "mpu6050.h"

// 외부 HAL 핸들러 선언 (main.c에서 extern으로 불러옴)
extern I2C_HandleTypeDef hi2c1; // I2C 통신 핸들
extern TIM_HandleTypeDef htim3;  // 왼쪽 엔코더 타이머 핸들
extern TIM_HandleTypeDef htim8;  // 오른쪽 엔코더 타이머 핸들

// UART_Printf 함수 프로토타입 (디버깅용)
extern void UART_Printf(const char *format, ...);
extern void HAL_Delay(uint32_t Delay); // HAL_Delay 사용


// =========================================================================
// 1. 전역 변수 정의 (메모리 할당)
// =========================================================================
float g_robot_x = 0.0f;
float g_robot_y = 0.0f;
float g_robot_yaw = 0.0f;
float gyro_bias_Z = 0.0f;
int prev_enc_L = 0;
int prev_enc_R = 0;


// =========================================================================
// 2. MPU6050 초기화 및 데이터 읽기 함수
// =========================================================================

// [수정됨] 함수 이름 변경: MPU6050_Init -> Odom_MPU_Init
void Odom_MPU_Init(void) {
    uint8_t check;
    uint8_t data;

    // WHO_AM_I register read
    HAL_I2C_Mem_Read(&hi2c1, MPU6050_ADDR, 0x75, 1, &check, 1, 1000);

    if (check == 104) {  // 0x68
        data = 0;
        // PWR_MGMT_1 설정: 슬립 모드 해제
        HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, 0x6B, 1, &data, 1, 1000);

        // 자이로 감도 설정: ±2000 deg/s 범위 설정 (Scale Factor: 16.4)
        data = 0x18; // 0x18 = GYRO_CONFIG (0x1B)에서 FS_SEL=3 설정
        HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, 0x1B, 1, &data, 1, 1000);
    }
}

void MPU6050_Read_Accel(int16_t *Ax, int16_t *Ay, int16_t *Az) {
    uint8_t Rec_Data[6];
    // 헤더가 정상적으로 include 되었다면 ACCEL_XOUT_H 에러는 사라집니다.
    HAL_I2C_Mem_Read(&hi2c1, MPU6050_ADDR, ACCEL_XOUT_H, 1, Rec_Data, 6, 1000);

    *Ax = (int16_t)(Rec_Data[0] << 8 | Rec_Data[1]);
    *Ay = (int16_t)(Rec_Data[2] << 8 | Rec_Data[3]);
    *Az = (int16_t)(Rec_Data[4] << 8 | Rec_Data[5]);
}

void MPU6050_Read_Gyro(int16_t *Gx, int16_t *Gy, int16_t *Gz) {
	uint8_t Rec_Data[6];
	// GYRO_XOUT_H (0x43)부터 6바이트 (Gx, Gy, Gz)를 읽어옴
	HAL_I2C_Mem_Read(&hi2c1, MPU6050_ADDR, GYRO_XOUT_H, 1, Rec_Data, 6, 100);

	*Gx = (int16_t)(Rec_Data[0] << 8 | Rec_Data[1]);
	*Gy = (int16_t)(Rec_Data[2] << 8 | Rec_Data[3]);
	*Gz = (int16_t)(Rec_Data[4] << 8 | Rec_Data[5]);
}


// =========================================================================
// 3. 자이로 바이어스 계산 함수
// =========================================================================
void calculate_gyro_bias(void) {
	long long sum_raw = 0;
	const int CAL_COUNT = 1000; // 1000회 측정
	int16_t Gx, Gy, Gz;

	UART_Printf("Starting Gyro Z-axis Calibration (1000 samples)...\r\n");

	// N회 반복하며 Z축 Raw 값 누적
	for (int i = 0; i < CAL_COUNT; i++) {
		MPU6050_Read_Gyro(&Gx, &Gy, &Gz);
		sum_raw += Gz;
		HAL_Delay(5); // 측정 간격 (5ms)
	}

	// 평균 Raw 값 계산 및 DPS 변환
	float avg_raw = (float)sum_raw / CAL_COUNT;
	float avg_dps = avg_raw / MPU6050_GYRO_SCALE;

	// Rad/s 단위로 변환하여 전역 바이어스 변수에 저장
	gyro_bias_Z = avg_dps * (M_PI / 180.0f); // M_PI는 odom_imu.h에서 정의됨

	UART_Printf("Calibration Done. Gyro Bias Z: %.5f rad/s\r\n", gyro_bias_Z);
}


// =========================================================================
// 4. 오도메트리 업데이트 함수 (IMU 융합 적용)
// =========================================================================
void update_odometry(void)
{
	const float PI = M_PI;
	const int PERIOD = TIMER_PERIOD;

	// 1. 엔코더 카운터 값 읽기
	int current_enc_L = (int)__HAL_TIM_GET_COUNTER(&htim3);
	int current_enc_R = (int)__HAL_TIM_GET_COUNTER(&htim8);

	// 2. 카운터 변화량 계산 (Delta Pulse) - 랩어라운드 처리 적용
	int delta_L_pulse = current_enc_L - prev_enc_L;
	int delta_R_pulse = current_enc_R - prev_enc_R;

	//  랩어라운드 처리
	if (delta_L_pulse > (PERIOD / 2)) {
		delta_L_pulse -= PERIOD;
	} else if (delta_L_pulse < -(PERIOD / 2)) {
		delta_L_pulse += PERIOD;
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

	// 평균 이동 거리 및 엔코더 기반 각도 변화 계산
	float distance_center_mm = (distance_R_mm + distance_L_mm) / 2.0f;
	// 엔코더 기반의 각도 변화량
	float delta_theta_enc = (distance_R_mm - distance_L_mm) / ROBOT_AXLE_LENGTH_MM;


	// [IMU 융합 로직]
	int16_t Gx_raw, Gy_raw, Gz_raw;
	MPU6050_Read_Gyro(&Gx_raw, &Gy_raw, &Gz_raw);

	float angular_vel_gyro_dps = (float)Gz_raw / MPU6050_GYRO_SCALE;
	float angular_vel_gyro_rps = angular_vel_gyro_dps * (PI / 180.0f);
	angular_vel_gyro_rps -= gyro_bias_Z;
	float delta_theta_gyro = angular_vel_gyro_rps * ODOMETRY_DT;

	// 상보 필터 융합
	float delta_theta_fused = (FUSION_ALPHA * delta_theta_gyro) +
	                          ((1.0f - FUSION_ALPHA) * delta_theta_enc);

    // 최종 각도 변화
    float delta_theta_rad = delta_theta_fused;


	// 4. 좌표 (X, Y) 및 방향 (Yaw) 업데이트 (Middle Point 적분 방식 적용)
	float current_yaw_rad = g_robot_yaw * PI / 180.0f;

	float avg_yaw_rad = current_yaw_rad + delta_theta_rad / 2.0f;

	float delta_x = distance_center_mm * cosf(avg_yaw_rad);
	float delta_y = distance_center_mm * sinf(avg_yaw_rad);

	g_robot_x += delta_x;
	g_robot_y += delta_y;

	// Yaw 각도 업데이트 및 정규화
	float next_yaw_rad = current_yaw_rad + delta_theta_rad;
	g_robot_yaw = next_yaw_rad * 180.0f / PI;

	while (g_robot_yaw >= 360.0f) g_robot_yaw -= 360.0f;
	while (g_robot_yaw < 0.0f) g_robot_yaw += 360.0f;
}
