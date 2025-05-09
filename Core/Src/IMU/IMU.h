#ifndef IMU_H
#define IMU_H
#include "main.h"
#include "cmsis_os.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>

#define CALIBRATE 0

static const uint8_t MPU6050_ADDR = 0b1101000 << 1;
static const uint8_t REG_CONFIG = 0x1A;
static const uint8_t REG_GYRO_CONFIG = 0x1B;
static const uint8_t REG_ACCEL_CONFIG = 0x1C;
static const uint8_t REG_VALS_START = 0x3B;
static const uint32_t customTimeout = 100;

#define X_CALIB_OFFSET (-4.927)
#define Y_CALIB_OFFSET (0.455)
#define Z_CALIB_OFFSET (0.066)

typedef struct {
	double x;
	double y;
	double z;
} Gval;

void StartDefaultTask(void *argument);
void calibrateGyro(uint8_t* buf);
Gval getGyroVals(uint8_t* buf);

#endif