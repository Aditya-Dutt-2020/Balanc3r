/*
 * Control.c
 *
 *  Created on: Jun 21, 2025
 *      Author: Aditya
 */
#include "Control.h"


void ControlDebugTaskStart(void *argument) {
	if(CONTROL_DEBUG) {
		 osEventFlagsSet(controlDebugFlagHandle, 0x01);
	}
	osEventFlagsWait(controlDebugFlagHandle, 0x01, osFlagsWaitAny, osWaitForever);
	osStatus_t status;
	double receivedAngle;
	int speed;
	vec2 receivedEnc;
	for(;;) {
		status = osMessageQueueGet(gyroQueueHandle, &receivedAngle, NULL, osWaitForever);
		status = osMessageQueueGet(encoderQueueHandle, &receivedEnc, NULL, osWaitForever);
		if (status == osOK) {
			printf("Angle: %.4f\tEncL: %.2f\tEncR: %.2f\n", receivedAngle, receivedEnc.x, receivedEnc.y);
			if (abs(receivedAngle) < 40) {
				speed = 800;
			} else {
				speed = 0;
			}
			osMessageQueuePut(motorSpeedQueueHandle, &speed, 0, 0);
		} else {
			printf("Reading Queue Error: %d\n", status);
		}
		osDelay(10);
	}

}


