#include "mpu6050.h"

/**
 * @brief  Initialize MPU6050 sensor
 * @param  hi2c: I2C handle
 * @retval HAL status
 */
HAL_StatusTypeDef MPU6050_Init(I2C_HandleTypeDef *hi2c)
{
    uint8_t data;
    HAL_StatusTypeDef status;

    // Check WHO_AM_I register
    if (MPU6050_WhoAmI(hi2c) != 0x68) {
        return HAL_ERROR;
    }

    // Wake up MPU6050 (clear sleep bit)
    data = 0x00;
    status = HAL_I2C_Mem_Write(hi2c, MPU6050_ADDR, MPU6050_REG_PWR_MGMT_1, 1, &data, 1, 100);
    if (status != HAL_OK) return status;

    HAL_Delay(100);  // Wait for sensor to stabilize

    // Set sample rate divider (1kHz / (1 + 9) = 100Hz)
    data = 0x09;
    status = HAL_I2C_Mem_Write(hi2c, MPU6050_ADDR, MPU6050_REG_SMPLRT_DIV, 1, &data, 1, 100);
    if (status != HAL_OK) return status;

    // Set DLPF (Digital Low Pass Filter) - Bandwidth 44Hz
    data = 0x03;
    status = HAL_I2C_Mem_Write(hi2c, MPU6050_ADDR, MPU6050_REG_CONFIG, 1, &data, 1, 100);
    if (status != HAL_OK) return status;

    // Set accelerometer range to ±2g
    data = 0x00;
    status = HAL_I2C_Mem_Write(hi2c, MPU6050_ADDR, MPU6050_REG_ACCEL_CONFIG, 1, &data, 1, 100);
    if (status != HAL_OK) return status;

    // Set gyroscope range to ±250°/s (not used for Edge-AI but good to configure)
    data = 0x00;
    status = HAL_I2C_Mem_Write(hi2c, MPU6050_ADDR, MPU6050_REG_GYRO_CONFIG, 1, &data, 1, 100);
    if (status != HAL_OK) return status;

    return HAL_OK;
}

/**
 * @brief  Read WHO_AM_I register
 * @param  hi2c: I2C handle
 * @retval WHO_AM_I value (should be 0x68)
 */
uint8_t MPU6050_WhoAmI(I2C_HandleTypeDef *hi2c)
{
    uint8_t who_am_i = 0;
    HAL_I2C_Mem_Read(hi2c, MPU6050_ADDR, MPU6050_REG_WHO_AM_I, 1, &who_am_i, 1, 100);
    return who_am_i;
}

/**
 * @brief  Read raw accelerometer data
 * @param  hi2c: I2C handle
 * @param  ax, ay, az: Pointers to store raw values
 * @retval HAL status
 */
HAL_StatusTypeDef MPU6050_ReadAccelRaw(I2C_HandleTypeDef *hi2c, int16_t *ax, int16_t *ay, int16_t *az)
{
    uint8_t buffer[6];
    HAL_StatusTypeDef status;

    // Read 6 bytes starting from ACCEL_XOUT_H
    status = HAL_I2C_Mem_Read(hi2c, MPU6050_ADDR, MPU6050_REG_ACCEL_XOUT_H, 1, buffer, 6, 100);
    if (status != HAL_OK) return status;

    // Combine high and low bytes (big-endian)
    *ax = (int16_t)((buffer[0] << 8) | buffer[1]);
    *ay = (int16_t)((buffer[2] << 8) | buffer[3]);
    *az = (int16_t)((buffer[4] << 8) | buffer[5]);

    return HAL_OK;
}

/**
 * @brief  Read accelerometer data in g units
 * @param  hi2c: I2C handle
 * @param  ax, ay, az: Pointers to store values in g
 * @retval HAL status
 */
HAL_StatusTypeDef MPU6050_ReadAccel(I2C_HandleTypeDef *hi2c, float *ax, float *ay, float *az)
{
    int16_t raw_ax, raw_ay, raw_az;
    HAL_StatusTypeDef status;

    status = MPU6050_ReadAccelRaw(hi2c, &raw_ax, &raw_ay, &raw_az);
    if (status != HAL_OK) return status;

    // Convert to g units
    *ax = (float)raw_ax / ACCEL_SCALE_FACTOR;
    *ay = (float)raw_ay / ACCEL_SCALE_FACTOR;
    *az = (float)raw_az / ACCEL_SCALE_FACTOR;

    return HAL_OK;
}
