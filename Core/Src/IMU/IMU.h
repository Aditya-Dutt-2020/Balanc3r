#ifndef IMU_H
#define IMU_H
#include "main.h"
#include "cmsis_os.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>

#define GCALIBRATE 0
#define ACALIBRATE -1

static const uint8_t MPU6050_ADDR = 0b1101000 << 1;
static const uint8_t REG_CONFIG = 0x1A;
static const uint8_t REG_GYRO_CONFIG = 0x1B;
static const uint8_t REG_ACCEL_CONFIG = 0x1C;
static const uint8_t REG_VALS_START = 0x3B;
static const uint32_t customTimeout = 100;

#define GYRO_LSB_SENS (65.5)
#define GX_CALIB_OFFSET (-4.844)
#define GY_CALIB_OFFSET (0.246)
#define GZ_CALIB_OFFSET (0.032)
#define ACCEL_LSB_SENS (8192.0f)
#define AX_CALIB_OFFSET(v) (-0.01035*(v)+0.06328)
#define AY_CALIB_OFFSET(v) (-0.00296*(v)+0.01880)
#define AZ_CALIB_OFFSET(v) (0.02999*(v)-0.10065)

typedef struct {
	double x;
	double y;
	double z;
} vec3;

void StartDefaultTask(void *argument);
void calibrateGyro(uint8_t* buf);
void calibrateAccel(uint8_t* buf, int axis);
void configIMU(uint8_t* buf);
vec3 getGyroVals(uint8_t* buf);
vec3 getAccelVals(uint8_t* buf);

#endif