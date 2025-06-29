#include "IMU.h"

void StartIMUTask(void *argument) {
	if(!IMU_DEBUG) {
		osEventFlagsSet(IMUDebugFlagHandle, 0x01);
	}
	osEventFlagsWait(IMUDebugFlagHandle, 0x01, osFlagsWaitAny, osWaitForever);
	printf("Running main imu\n");
	uint8_t buf[14];
	configIMU(buf);
	double angle = getInitAngle(buf);

	uint32_t tickFreq = osKernelGetTickFreq();
	uint32_t lastTick = 0;
	uint32_t currentTick;
	double deltaT;
	osStatus_t status;
	lastTick = osKernelGetTickCount();

	for (;;) {
		currentTick = osKernelGetTickCount();
		deltaT = (double)(currentTick - lastTick) / tickFreq;

		vec3 angularVel = getGyroVals(buf);
		lastTick = currentTick;
		angle += angularVel.y * deltaT;
		status = osMessageQueuePut(gyroQueueHandle, &angle, 0, 0);
//		if (status != osOK) {
//			printf("ERROR IN MESSAGE gyro PUT: %d\n", status);
//		}

		osDelay(5);
	}
}

double getInitAngle(uint8_t* buf) {
	vec3 accelVals = getAccelVals(buf);
	return (atan2(accelVals.x, sqrt(accelVals.y*accelVals.y + accelVals.z*accelVals.z)) * (180.0 / M_PI)) - 90;
}


void StartIMUDebugTask(void *argument)
{
	if(IMU_DEBUG) {
		osEventFlagsSet(IMUDebugFlagHandle, 0x02);
	}
	osEventFlagsWait(IMUDebugFlagHandle, 0x02, osFlagsWaitAny, osWaitForever);
	printf("RUnning Debug imu\n");
	uint8_t buf[14];
	configIMU(buf);
	if (GCALIBRATE) {
		calibrateGyro(buf);
		osDelay(10000);
	}
	if (ACALIBRATE != -1) {
		calibrateAccel(buf, ACALIBRATE);
		osDelay(10000);
	}

	for(;;)
	{
//		vec3 vals = getAccelVals(buf);
//		printf("x:%.5f\ty:%.5f\tz:%.5f\n", vals.x, vals.y, vals.z);

		osDelay(1);
	}
	printf("Error?\n");
	osThreadTerminate(NULL);

}
vec3 getGyroVals(uint8_t* buf) {
	vec3 vals = {0.0, 0.0, 0.0};
	HAL_StatusTypeDef ret;
	osMutexAcquire(i2cMutexHandle, osWaitForever);
	ret = HAL_I2C_Mem_Read (&hi2c1, MPU6050_ADDR, 0x43, 1, buf, 6, 1000);
	osMutexRelease(i2cMutexHandle);
	if (ret != HAL_OK) {
		printf("Error RX\n");
	} else {

		int rawGyro = (int16_t)((buf[0] << 8) | buf[1]);
		vals.x = (rawGyro /  GYRO_LSB_SENS) - GX_CALIB_OFFSET;
		rawGyro = (int16_t)((buf[2] << 8) | buf[3]);
		vals.y = -1*((rawGyro /  GYRO_LSB_SENS) - GY_CALIB_OFFSET);
		rawGyro = (int16_t)((buf[4] << 8) | buf[5]);
		vals.z = (rawGyro /  GYRO_LSB_SENS) - GZ_CALIB_OFFSET;
	}

	return vals;
//	vec3 vals = {0.0, 0.0, 0.0};
//	HAL_StatusTypeDef ret;
//	buf[0] = REG_VALS_START;
//	osMutexAcquire(i2cMutexHandle, osWaitForever);
//	ret = HAL_I2C_Master_Transmit(&hi2c1, MPU6050_ADDR, buf, 1, customTimeout);
//	if (ret != HAL_OK) {
//		printf("Error TX\n");
//	} else {
//		ret = HAL_I2C_Master_Receive(&hi2c1, MPU6050_ADDR, buf, 14, customTimeout);
//		osMutexRelease(i2cMutexHandle);
//		if (ret != HAL_OK) {
//			printf("Error RX\n");
//		} else {
//			int rawGyro = (int16_t)((buf[8] << 8) | buf[9]);
//			vals.x = (rawGyro /  GYRO_LSB_SENS) - GX_CALIB_OFFSET;
//			rawGyro = (int16_t)((buf[10] << 8) | buf[11]);
//			vals.y = -1*((rawGyro /  GYRO_LSB_SENS) - GY_CALIB_OFFSET);
//			rawGyro = (int16_t)((buf[12] << 8) | buf[13]);
//			vals.z = (rawGyro /  GYRO_LSB_SENS) - GZ_CALIB_OFFSET;
//		}
//	}
//
//	return vals;
}

vec3 getAccelVals(uint8_t* buf) {
	vec3 vals = {0.0, 0.0, 0.0};
	HAL_StatusTypeDef ret;
	osMutexAcquire(i2cMutexHandle, osWaitForever);
	ret = HAL_I2C_Mem_Read (&hi2c1, MPU6050_ADDR, 0x3B, 1, buf, 6, 1000);
	osMutexRelease(i2cMutexHandle);
	if (ret != HAL_OK) {
		printf("Error RX\n");
	} else {
		int rawAccel = (int16_t)((buf[0] << 8) | buf[1]);
		vals.x = (rawAccel /  ACCEL_LSB_SENS) - AX_CALIB_OFFSET(rawAccel /  ACCEL_LSB_SENS);
		rawAccel = (int16_t)((buf[2] << 8) | buf[3]);
		vals.y = (rawAccel /  ACCEL_LSB_SENS) - AY_CALIB_OFFSET(rawAccel /  ACCEL_LSB_SENS);
		rawAccel = (int16_t)((buf[4] << 8) | buf[5]);
		vals.z = (rawAccel /  ACCEL_LSB_SENS) - AZ_CALIB_OFFSET(rawAccel /  ACCEL_LSB_SENS);
	}

	return vals;
//	vec3 vals = {0.0, 0.0, 0.0};
//	HAL_StatusTypeDef ret;
//	buf[0] = REG_VALS_START;
//	osMutexAcquire(i2cMutexHandle, osWaitForever);
//	ret = HAL_I2C_Master_Transmit(&hi2c1, MPU6050_ADDR, buf, 1, customTimeout);
//	if (ret != HAL_OK) {
//		printf("Error TX\n");
//	} else {
//		ret = HAL_I2C_Master_Receive(&hi2c1, MPU6050_ADDR, buf, 14, customTimeout);
//		osMutexRelease(i2cMutexHandle);
//		if (ret != HAL_OK) {
//			printf("Error RX\n");
//		} else {
//			int rawAccel = (int16_t)((buf[0] << 8) | buf[1]);
//			vals.x = (rawAccel /  ACCEL_LSB_SENS) - AX_CALIB_OFFSET(rawAccel /  ACCEL_LSB_SENS);
//			rawAccel = (int16_t)((buf[2] << 8) | buf[3]);
//			vals.y = (rawAccel /  ACCEL_LSB_SENS) - AY_CALIB_OFFSET(rawAccel /  ACCEL_LSB_SENS);
//			rawAccel = (int16_t)((buf[4] << 8) | buf[5]);
//			vals.z = (rawAccel /  ACCEL_LSB_SENS) - AZ_CALIB_OFFSET(rawAccel /  ACCEL_LSB_SENS);
//		}
//	}
//
//	return vals;
}
void configIMU(uint8_t* buf) {
	osDelay(150);
	uint8_t check;
	uint8_t Data;
	HAL_StatusTypeDef ret;
	osMutexAcquire(i2cMutexHandle, osWaitForever);
	ret = HAL_I2C_Mem_Read(&hi2c1, MPU6050_ADDR, 0x75, 1, &check, 1, 1000);
	if (ret == HAL_OK && check == 0x68) {
		Data = 0;
		ret = HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, 0x6B, 1,&Data, 1, 1000);
		if (ret != HAL_OK) {
			printf("Wakeup Failed, ret=%d\n", ret);
		}
		Data = 0x07;
		ret = HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, 0x19, 1, &Data, 1, 1000);
		if (ret != HAL_OK) {
			printf("data rate set Failed, ret=%d\n", ret);
		}
		Data = 1 << 3;
		ret = HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, 0x1B, 1, &Data, 1, 1000);
		if (ret != HAL_OK) {
			printf("gyro config set Failed, ret=%d\n", ret);
		}
		Data = 1 << 3;
		ret = HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, 0x1C, 1, &Data, 1, 1000);
		if (ret != HAL_OK) {
			printf("accel config set Failed, ret=%d\n", ret);
		}
		osMutexRelease(i2cMutexHandle);
	}else {
		printf("Config failed, ret=%d, check=%d\n", ret, check);
	}
//	osDelay(150);
//	HAL_StatusTypeDef ret;
//	buf[0] = 0x6B;
//	buf[1] = 0x01;
//	for (int i = 0; i < MPU6050_I2C_RETRIES; i++) {
//		osMutexAcquire(i2cMutexHandle, osWaitForever);
//		ret = HAL_I2C_Master_Transmit(&hi2c1, MPU6050_ADDR, buf, 2, customTimeout);
//		osMutexRelease(i2cMutexHandle);
//		if (ret == HAL_OK) break;
//		osDelay(10);
//
//	}
//	if (ret != HAL_OK) {
//		printf("Error TX Wakeup: %d\n", ret);
//		osThreadTerminate(NULL);
//	}
//	osDelay(100);
//	buf[0] = REG_CONFIG;
//	buf[1] = 0;
//	buf[2] = 1 << 3;
//	buf[3] = 1 << 3;
//	osMutexAcquire(i2cMutexHandle, osWaitForever);
//	ret = HAL_I2C_Master_Transmit(&hi2c1, MPU6050_ADDR, buf, 4, customTimeout);
//	osMutexRelease(i2cMutexHandle);
//	if (ret != HAL_OK) {
//		printf("Error TX Config: %d\n", ret);
//		osThreadTerminate(NULL);
//	}
//
//	printf("Config Wrote Successfully!\n");
}
void calibrateGyro(uint8_t* buf) {
	HAL_StatusTypeDef ret;
	double xTot = 0;
	double yTot = 0;
	double zTot = 0;
	for (int ct = 1; ct <= 1000; ct++) {
		buf[0] = REG_VALS_START;
		osMutexAcquire(i2cMutexHandle, osWaitForever);
		ret = HAL_I2C_Master_Transmit(&hi2c1, MPU6050_ADDR, buf, 1, customTimeout);
		if (ret != HAL_OK) {
			printf("Error TX\n");
		} else {
			ret = HAL_I2C_Master_Receive(&hi2c1, MPU6050_ADDR, buf, 14, customTimeout);
			osMutexRelease(i2cMutexHandle);
			if (ret != HAL_OK) {
				printf("Error RX\n");
			} else {
				int rawGyroX = (int16_t)((buf[8] << 8) | buf[9]);
				double gyroX = rawGyroX /  65.5;
				int rawGyroY = (int16_t)((buf[10] << 8) | buf[11]);
				double gyroY = rawGyroY /  65.5;
				int rawGyroZ = (int16_t)((buf[12] << 8) | buf[13]);
				double gyroZ = rawGyroZ /  65.5;
				xTot += gyroX;
				yTot += gyroY;
				zTot += gyroZ;
				printf("cnt = %d\n", ct);
			}
		}
		osDelay(50);
	}
	printf("\n\n\n\naverages: x:%.7f\ty:%.7f\tz:%.7f\n\n\n\n\n", xTot/1000, yTot/1000, zTot/1000);
}
void calibrateAccel(uint8_t* buf, int axis) {
	int num_of_points = 0;
	float x_sum = 0;
	float y_sum = 0;
	float x_squared_sum = 0;
	float x_times_y_sum = 0;

	printf("--------------------------------------------------\n");
	printf("Orient the axis upwards against gravity\n");
	osDelay(4000); // 1 second delay

	printf("Beginning to Calibrate Part 1 (Acceleration = +1g)\n");

	for (int i = 0; i < 800; i++) {
		vec3 a = getAccelVals(buf);
		float vals[] = {a.x, a.y, a.z};
		float offset = vals[axis] - 1.0f;

		num_of_points++;
		x_sum += 1.0f;
		y_sum += offset;
		x_squared_sum += 1.0f;
		x_times_y_sum += 1.0f * offset;

		if (num_of_points % 100 == 0) {
			printf("Still Calibrating... %d points so far\n", num_of_points);
		}

		osDelay(10); // Short delay between samples
	}

	printf("--------------------------------------------------\n");
	printf("Orient the axis downwards against gravity\n");
	osDelay(4000);

	printf("Beginning to Calibrate Part 2 (Acceleration = -1g)\n");

	for (int i = 0; i < 800; i++) {
		vec3 a = getAccelVals(buf);
		float vals[] = {a.x, a.y, a.z};
		float offset = vals[axis] + 1.0f;

		num_of_points++;
		x_sum += -1.0f;
		y_sum += offset;
		x_squared_sum += 1.0f; // (-1)^2 = 1
		x_times_y_sum += -1.0f * offset;

		if (num_of_points % 100 == 0) {
			printf("Still Calibrating... %d points so far\tLast one was: %.2f\n", num_of_points, vals[axis]);
		}

		osDelay(10);
	}

	printf("--------------------------------------------------\n");
	printf("Orient the axis perpendicular to gravity\n");
	osDelay(4000);

	printf("Beginning to Calibrate Part 3 (Acceleration = 0g)\n");

	for (int i = 0; i < 800; i++) {
		vec3 a = getAccelVals(buf);
		float vals[] = {a.x, a.y, a.z};
		float offset = vals[axis];

		num_of_points++;
		// x = 0, so x_sum, x_squared_sum, x_times_y_sum don't change
		y_sum += offset;

		if (num_of_points % 100 == 0) {
			printf("Still Calibrating... %d points so far\n", num_of_points);
		}

		osDelay(10);
	}

	// Least squares computation
	float m = (num_of_points * x_times_y_sum - x_sum * y_sum) /
			  (num_of_points * x_squared_sum - x_sum * x_sum);

	float b = (y_sum - m * x_sum) / num_of_points;

	// Output values
	printf("m=%.5f\tb=%.5f\n", m, b);
}
