/*
 * Control.h
 *
 *  Created on: Jun 21, 2025
 *      Author: Aditya
 */

#ifndef SRC_CONTROL_CONTROL_H_
#define SRC_CONTROL_CONTROL_H_
#include "main.h"
#include "cmsis_os.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>

#define CONTROL_DEBUG 1

void ControlDebugTaskStart(void *argument);
#endif /* SRC_CONTROL_CONTROL_H_ */
