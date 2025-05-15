#ifndef SRC_MOTOR_MOTOR_H_
#define SRC_MOTOR_MOTOR_H_
#include "main.h"
#include "cmsis_os.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>

#define ARR 65535
typedef struct Motor Motor;
struct Motor{
	GPIO_TypeDef *inGpio1;
	uint16_t inPin1;
	GPIO_TypeDef *inGpio2;
	uint16_t inPin2;
	TIM_HandleTypeDef *pwmHtim;
	int pwmChan;
	TIM_HandleTypeDef *encHtim;
	__IO uint32_t *speedVal;
	__IO uint32_t *encoderLoc;

	void (*setSpeed)(struct Motor*, int);
	long (*getEncoder)(struct Motor*);
};

typedef struct {
	Motor left;
	Motor right;
} MotorSet;

void StartMotorDebugTask(void *argument);
void initMotorStruct(Motor* m, GPIO_TypeDef *inGpio1,uint16_t inPin1,GPIO_TypeDef *inGpio2,uint16_t inPin2,
		TIM_HandleTypeDef *pwmHtim,int pwmChan,TIM_HandleTypeDef *encHtim,__IO uint32_t *speedVal, __IO uint32_t *encoderLoc);
MotorSet initMotors();
void setSpeed(Motor* m, int speed);
long getEncoder(Motor* m);
#endif
