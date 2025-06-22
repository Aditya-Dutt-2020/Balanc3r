#include "Motor.h"

void StartMotorTask(void *argument) {
	osEventFlagsWait(motorDebugFlagHandle, 0x04, osFlagsWaitAny, osWaitForever);
	printf("Started startMotorTask\n");
	Motor leftMotor = motors.left;
	Motor rightMotor = motors.right;
	osStatus_t status;
	int receivedSpeed;

	for(;;) {
		status = osMessageQueueGet(motorSpeedQueueHandle, &receivedSpeed, NULL, osWaitForever);
		if (status == osOK) {
			printf("setSpeed to %d\n", receivedSpeed);
			leftMotor.setSpeed(&leftMotor, receivedSpeed);
			rightMotor.setSpeed(&rightMotor, receivedSpeed);
		} else {
			printf("Reading speed Queue Error: %d\n", status);
		}
		osDelay(10);
	}

}
void StartMotorDebugTask(void *argument)
{
	if(MOTOR_DEBUG) {
		 osEventFlagsSet(motorDebugFlagHandle, 0x01);
	}
	osEventFlagsWait(motorDebugFlagHandle, 0x01, osFlagsWaitAny, osWaitForever);
	motors = initMotors();
	Motor leftMotor = motors.left;
	Motor rightMotor = motors.right;
	rightMotor.setSpeed(&rightMotor, 500);
	for (;;) {
		osDelay(1);
	}

}
void StartEncoderTask(void *argument) {
	if(!MOTOR_DEBUG) {
		 osEventFlagsSet(motorDebugFlagHandle, 0x02);
	}
	osEventFlagsWait(motorDebugFlagHandle, 0x02, osFlagsWaitAny, osWaitForever);
	motors = initMotors();
	osEventFlagsSet(motorDebugFlagHandle, 0x04);
	printf("SetFlags\n");
	Motor leftMotor = motors.left;
	Motor rightMotor = motors.right;
	osStatus_t status;
	vec2 ret;
	for (;;) {
		ret.x = getDist(&leftMotor);
		ret.y = getDist(&rightMotor);
		status = osMessageQueuePut(encoderQueueHandle, &ret, 0, 0);
//		if (status != osOK) {
//			printf("ERROR IN MESSAGE encoder PUT: %d\n", status);
//		}
		osDelay(10);
	}

}
MotorSet initMotors() {
	MotorSet ret;
	Motor left;
	Motor right;
	initMotorStruct(&left, GPIOA, GPIO_PIN_11, GPIOA, GPIO_PIN_12, &htim2, TIM_CHANNEL_4, true, &htim1, &(TIM2->CCR4), &(TIM1->CNT));
	initMotorStruct(&right, GPIOC, GPIO_PIN_6, GPIOC, GPIO_PIN_5, &htim8, TIM_CHANNEL_3, false, &htim3, &(TIM8->CCR3), &(TIM3->CNT));
	ret.left = left;
	ret.right = right;
	return ret;
}
void initMotorStruct(Motor* m, GPIO_TypeDef *inGpio1,uint16_t inPin1,GPIO_TypeDef *inGpio2,uint16_t inPin2,
		TIM_HandleTypeDef *pwmHtim,int pwmChan, bool isReversed, TIM_HandleTypeDef *encHtim, __IO uint32_t *speedVal, __IO uint32_t *encoderLoc) {
	m->inGpio1 = inGpio1;
	m->inPin1 = inPin1;
	m->inGpio2 = inGpio2;
	m->inPin2 = inPin2;
	m->pwmHtim = pwmHtim;
	m->pwmChan = pwmChan;
	m->isReversed = isReversed;
	m->encHtim = encHtim;
	m->speedVal = speedVal;
	m->encoderLoc = encoderLoc;
	m->setSpeed = &setSpeed;
	m->getDist = &getDist;
	m->pos = 0;
	m->lastEnc = 0;
	HAL_TIM_PWM_Start(pwmHtim, pwmChan);
	HAL_TIM_Encoder_Start(encHtim, TIM_CHANNEL_ALL);
	__HAL_TIM_SET_COUNTER(encHtim, 0x8000);
}

void setSpeed(Motor *m, int speed) {
	if (m->isReversed) speed = -speed;
	if(speed < 0) {
		HAL_GPIO_WritePin(m->inGpio1, m->inPin1, GPIO_PIN_SET);
		HAL_GPIO_WritePin(m->inGpio2, m->inPin2, GPIO_PIN_RESET);
	} else {
		HAL_GPIO_WritePin(m->inGpio1, m->inPin1, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(m->inGpio2, m->inPin2, GPIO_PIN_SET);
	}
	*(m->speedVal) = mapRange(0,1023,0,ARR,abs(speed));
}
double getDist(Motor *m) {
	int16_t rawEnc = (int)((*(m->encoderLoc))>>2);
	int delta = rawEnc - m->lastEnc;
	if (abs(delta) > 4000) delta = 0;

	double deltaMM = ((WHEEL_DIA*M_PI)/COUNTS_PER_REV)*delta;
	m->pos += deltaMM;
	m->lastEnc = rawEnc;
	return m->pos;
}

