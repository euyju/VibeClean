#ifndef INC_MPU6050_H_
#define INC_MPU6050_H_

#include "stm32f4xx_hal.h"

// MPU6050 I2C Address
#define MPU6050_ADDR         (0x68 << 1)  // AD0 = GND

// MPU6050 Registers
#define MPU6050_REG_PWR_MGMT_1   0x6B
#define MPU6050_REG_SMPLRT_DIV   0x19
#define MPU6050_REG_CONFIG       0x1A
#define MPU6050_REG_GYRO_CONFIG  0x1B
#define MPU6050_REG_ACCEL_CONFIG 0x1C
#define MPU6050_REG_WHO_AM_I     0x75
#define MPU6050_REG_ACCEL_XOUT_H 0x3B

#define ACCEL_XOUT_H         0x3B   // odom_imu.c에서 이 이름을 사용함
#define GYRO_XOUT_H          0x43   // 자이로 X축 레지스터 주소 (필수 추가)
#define MPU6050_GYRO_SCALE   16.4f  // 자이로 스케일 팩터 (2000dps 기준)

// Accelerometer Scale Factor (for ±2g range)
#define ACCEL_SCALE_FACTOR   16384.0f

// Function Prototypes
HAL_StatusTypeDef MPU6050_Init(I2C_HandleTypeDef *hi2c);
HAL_StatusTypeDef MPU6050_ReadAccel(I2C_HandleTypeDef *hi2c, float *ax, float *ay, float *az);
HAL_StatusTypeDef MPU6050_ReadAccelRaw(I2C_HandleTypeDef *hi2c, int16_t *ax, int16_t *ay, int16_t *az);
uint8_t MPU6050_WhoAmI(I2C_HandleTypeDef *hi2c);

#endif /* INC_MPU6050_H_ */
