#include "IMU.h"
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
void StartDefaultTask(void *argument)
{
	HAL_StatusTypeDef ret;
	uint8_t buf[14];
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
	buf[3] = 0;
	ret = HAL_I2C_Master_Transmit(&hi2c1, MPU6050_ADDR, buf, 4, customTimeout);
	if (ret != HAL_OK) {
		printf("Error TX Config\n");
		osThreadTerminate(NULL);
	}

	printf("Config Wrote Successfully!\n");
	if (CALIBRATE) {
		calibrateGyro(buf);
	}

	for(;;)
	{
		Gval vals = getGyroVals(buf);
		printf("x:%.3f\ty:%.3f\tz:%.3f\n", vals.x, vals.y, vals.z);
		osDelay(30);
	}
	printf("Error?\n");
	osThreadTerminate(NULL);

}
Gval getGyroVals(uint8_t* buf) {
	Gval vals = {0.0, 0.0, 0.0};
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
			vals.x = (rawGyro /  65.5) - X_CALIB_OFFSET;
			rawGyro = (int16_t)((buf[10] << 8) | buf[11]);
			vals.y = (rawGyro /  65.5) - Y_CALIB_OFFSET;
			rawGyro = (int16_t)((buf[12] << 8) | buf[13]);
			vals.z = (rawGyro /  65.5) - Z_CALIB_OFFSET;
		}
	}

	return vals;
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
