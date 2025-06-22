#ifndef IMU_H
#define IMU_H
#include "main.h"
#include "cmsis_os.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>

#define GCALIBRATE 1
#define ACALIBRATE -1
#define IMU_DEBUG 0
#define MPU6050_I2C_RETRIES 5


static const uint8_t MPU6050_ADDR = 0b1101000 << 1;
static const uint8_t REG_CONFIG = 0x1A;
static const uint8_t REG_GYRO_CONFIG = 0x1B;
static const uint8_t REG_ACCEL_CONFIG = 0x1C;
static const uint8_t REG_VALS_START = 0x3B;
static const uint32_t customTimeout = 100;

#define GYRO_LSB_SENS (65.5)
#define GX_CALIB_OFFSET (-4.8070992)
#define GY_CALIB_OFFSET (0.8561069)
#define GZ_CALIB_OFFSET (-0.0524427)
#define ACCEL_LSB_SENS (8192.0f)
#define AX_CALIB_OFFSET(v) (-0.00489*(v)+0.04452)
#define AY_CALIB_OFFSET(v) (-0.00091*(v)+0.01034)
#define AZ_CALIB_OFFSET(v) (0.02738*(v)-0.09464)

void StartIMUDebugTask(void *argument);
void StartIMUTask(void *argument);
void calibrateGyro(uint8_t* buf);
void calibrateAccel(uint8_t* buf, int axis);
void configIMU(uint8_t* buf);
vec3 getGyroVals(uint8_t* buf);
vec3 getAccelVals(uint8_t* buf);
double getInitAngle(uint8_t* buf);


#endif