#ifndef SRC_MOTOR_MOTOR_H_
#define SRC_MOTOR_MOTOR_H_
#include "main.h"
#include "cmsis_os.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdbool.h>

#define MOTOR_DEBUG 0

#define ARR 65535
#define COUNTS_PER_REV  (408)
#define WHEEL_DIA (60)
typedef struct Motor Motor;
struct Motor{
	GPIO_TypeDef *inGpio1;
	uint16_t inPin1;
	GPIO_TypeDef *inGpio2;
	uint16_t inPin2;
	TIM_HandleTypeDef *pwmHtim;
	int pwmChan;
	bool isReversed;
	TIM_HandleTypeDef *encHtim;
	__IO uint32_t *speedVal;
	__IO uint32_t *encoderLoc;

	double pos;
	int16_t lastEnc;

	void (*setSpeed)(struct Motor*, int);
	double (*getDist)(struct Motor*);
};

typedef struct {
	Motor left;
	Motor right;
} MotorSet;
static MotorSet motors;


void StartMotorDebugTask(void *argument);
void StartEncoderTask(void *argument);
void StartMotorTask(void *argument);
void initMotorStruct(Motor* m, GPIO_TypeDef *inGpio1,uint16_t inPin1,GPIO_TypeDef *inGpio2,uint16_t inPin2,
		TIM_HandleTypeDef *pwmHtim,int pwmChan, bool isReversed, TIM_HandleTypeDef *encHtim,__IO uint32_t *speedVal, __IO uint32_t *encoderLoc);
MotorSet initMotors();
void setSpeed(Motor* m, int speed);
double getDist(Motor* m);
#endif
