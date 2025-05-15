#include "IMU.h"

void StartIMUDebugTask(void *argument)
{
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
	buf[0] = REG_VALS_START;
	ret = HAL_I2C_Master_Transmit(&hi2c1, MPU6050_ADDR, buf, 1, customTimeout);
	if (ret != HAL_OK) {
		printf("Error TX\n");
	} else {
		ret = HAL_I2C_Master_Receive(&hi2c1, MPU6050_ADDR, buf, 14, customTimeout);
		if (ret != HAL_OK) {
			printf("Error RX\n");
		} else {
			int rawGyro = (int16_t)((buf[8] << 8) | buf[9]);
			vals.x = (rawGyro /  GYRO_LSB_SENS) - GX_CALIB_OFFSET;
			rawGyro = (int16_t)((buf[10] << 8) | buf[11]);
			vals.y = (rawGyro /  GYRO_LSB_SENS) - GY_CALIB_OFFSET;
			rawGyro = (int16_t)((buf[12] << 8) | buf[13]);
			vals.z = (rawGyro /  GYRO_LSB_SENS) - GZ_CALIB_OFFSET;
		}
	}

	return vals;
}

vec3 getAccelVals(uint8_t* buf) {
	vec3 vals = {0.0, 0.0, 0.0};
	HAL_StatusTypeDef ret;
	buf[0] = REG_VALS_START;
	ret = HAL_I2C_Master_Transmit(&hi2c1, MPU6050_ADDR, buf, 1, customTimeout);
	if (ret != HAL_OK) {
		printf("Error TX\n");
	} else {
		ret = HAL_I2C_Master_Receive(&hi2c1, MPU6050_ADDR, buf, 14, customTimeout);
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
	}

	return vals;
}
void configIMU(uint8_t* buf) {
	HAL_StatusTypeDef ret;
	buf[0] = 0x6B;
	buf[1] = 0x01;
	ret = HAL_I2C_Master_Transmit(&hi2c1, MPU6050_ADDR, buf, 2, customTimeout);
	if (ret != HAL_OK) {
		printf("Error TX Wakeup\n");
		osThreadTerminate(NULL);
	}
	osDelay(100);
	buf[0] = REG_CONFIG;
	buf[1] = 0;
	buf[2] = 1 << 3;
	buf[3] = 1 << 3;
	ret = HAL_I2C_Master_Transmit(&hi2c1, MPU6050_ADDR, buf, 4, customTimeout);
	if (ret != HAL_OK) {
		printf("Error TX Config\n");
		osThreadTerminate(NULL);
	}

	printf("Config Wrote Successfully!\n");
}
void calibrateGyro(uint8_t* buf) {
	HAL_StatusTypeDef ret;
	double xTot = 0;
	double yTot = 0;
	double zTot = 0;
	for (int ct = 1; ct <= 1000; ct++) {
		buf[0] = REG_VALS_START;
		ret = HAL_I2C_Master_Transmit(&hi2c1, MPU6050_ADDR, buf, 1, customTimeout);
		if (ret != HAL_OK) {
			printf("Error TX\n");
		} else {
			ret = HAL_I2C_Master_Receive(&hi2c1, MPU6050_ADDR, buf, 14, customTimeout);
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
	printf("\n\n\n\naverages: x:%.3f\ty:%.3f\tz:%.3f\n\n\n\n\n", xTot/1000, yTot/1000, zTot/1000);
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
			printf("Still Calibrating... %d points so far\n", num_of_points);
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
