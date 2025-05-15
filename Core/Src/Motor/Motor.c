#include "Motor.h"

void StartMotorDebugTask(void *argument)
{
	MotorSet motors = initMotors();
	Motor debugMotor = motors.left;
	int speed = 0;
	for(;;) {
		while(speed < 1023) {
			speed += 1;
			debugMotor.setSpeed(&debugMotor, speed);
			printf("Speed: %d, Encoder: %ld\n", speed, debugMotor.getEncoder(&debugMotor));
			osDelay(5);
		}
		while(speed >= -1023) {
			speed -= 1;
			debugMotor.setSpeed(&debugMotor, speed);
			printf("Speed: %d, Encoder: %ld\n", speed, debugMotor.getEncoder(&debugMotor));
			osDelay(5);
		}

	}
}
MotorSet initMotors() {
	MotorSet ret;
	Motor left;
	Motor right;
	initMotorStruct(&left, GPIOA, GPIO_PIN_11, GPIOA, GPIO_PIN_12, &htim2, TIM_CHANNEL_4, &htim1, &(TIM2->CCR4), &(TIM1->CNT));
	initMotorStruct(&right, GPIOC, GPIO_PIN_6, GPIOC, GPIO_PIN_5, &htim8, TIM_CHANNEL_3, &htim3, &(TIM8->CCR3), &(TIM3->CNT));
	ret.left = left;
	ret.right = right;
	return ret;
}
void initMotorStruct(Motor* m, GPIO_TypeDef *inGpio1,uint16_t inPin1,GPIO_TypeDef *inGpio2,uint16_t inPin2,
		TIM_HandleTypeDef *pwmHtim,int pwmChan,TIM_HandleTypeDef *encHtim, __IO uint32_t *speedVal, __IO uint32_t *encoderLoc) {
	m->inGpio1 = inGpio1;
	m->inPin1 = inPin1;
	m->inGpio2 = inGpio2;
	m->inPin2 = inPin2;
	m->pwmHtim = pwmHtim;
	m->pwmChan = pwmChan;
	m->encHtim = encHtim;
	m->speedVal = speedVal;
	m->encoderLoc = encoderLoc;
	m->setSpeed = &setSpeed;
	m->getEncoder = &getEncoder;
	HAL_TIM_PWM_Start(pwmHtim, pwmChan);
	HAL_TIM_Encoder_Start(encHtim, TIM_CHANNEL_ALL);
}

void setSpeed(Motor *m, int speed) {
	if(speed < 0) {
		HAL_GPIO_WritePin(m->inGpio1, m->inPin1, GPIO_PIN_SET);
		HAL_GPIO_WritePin(m->inGpio2, m->inPin2, GPIO_PIN_RESET);
	} else {
		HAL_GPIO_WritePin(m->inGpio1, m->inPin1, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(m->inGpio2, m->inPin2, GPIO_PIN_SET);
	}
	*(m->speedVal) = mapRange(0,1023,0,ARR,abs(speed));
}
long getEncoder(Motor *m) {
	return ((*(m->encoderLoc))>>2);
}

