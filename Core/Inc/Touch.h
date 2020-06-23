#include "stm32f1xx_hal.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void Touch_Init();
bool isTouch();
uint16_t getPressure();
int readTouchY();
int readTouchX();

// 0: x, 1: y, 2: z
int * getPoint();
int pressureThreshold;
