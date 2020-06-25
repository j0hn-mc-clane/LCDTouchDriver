#include <Touch.h>
#include "stm32f1xx_hal.h"

#define YPLUS_Pin GPIO_PIN_2
#define YPLUS_GPIO_Port GPIOC
#define XMIN_Pin GPIO_PIN_3
#define XMIN_GPIO_Port GPIOC
#define XPLUS_Pin GPIO_PIN_5
#define XPLUS_GPIO_Port GPIOB
#define YMIN_Pin GPIO_PIN_10
#define YMIN_GPIO_Port GPIOA

// Scale from ~0->1000 to tft.width using the calibration #'s
//  p.x = map(p.x, TS_MINX, TS_MAXX, 0, tft.width());
//  p.y = map(p.y, TS_MINY, TS_MAXY, 0, tft.height());

/**
 * Pin configurations
 */
static GPIO_TypeDef *ym = YMIN_GPIO_Port;
static GPIO_TypeDef *xm = XMIN_GPIO_Port;
static GPIO_TypeDef *yp = YPLUS_GPIO_Port;
static GPIO_TypeDef *xp = XPLUS_GPIO_Port;
static ADC_HandleTypeDef adcHandle;

static uint16_t ym_PIN = YMIN_Pin;
static uint16_t xm_PIN = XMIN_Pin;
static uint16_t yp_PIN = YPLUS_Pin;
static uint16_t xp_PIN = XPLUS_Pin;

int map(int x, int in_min, int in_max, int out_min, int out_max) {
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void Touch_Init(ADC_HandleTypeDef *adc) {
	memcpy(&adcHandle, adc, sizeof(*adc));
}

bool isTouch();
uint16_t getPressure();

void setupForReadY() {
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.Pin = yp_PIN;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Pull = GPIO_PULLUP;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init( yp , &GPIO_InitStructure );
	HAL_GPIO_WritePin(yp, yp_PIN, GPIO_PIN_SET);

	GPIO_InitStructure.Pin = ym_PIN;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Pull = GPIO_PULLDOWN;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init( ym , &GPIO_InitStructure );
	HAL_GPIO_WritePin(ym, ym_PIN, GPIO_PIN_RESET);

	GPIO_InitStructure.Pin = xp_PIN;
	GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
	GPIO_InitStructure.Pull = GPIO_PULLDOWN;
	HAL_GPIO_Init( xp , &GPIO_InitStructure );
	HAL_GPIO_WritePin(xp, xp_PIN, GPIO_PIN_RESET);

	GPIO_InitStructure.Pin = xm_PIN;
	GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	HAL_GPIO_Init( xm , &GPIO_InitStructure );
}

void setupForReadX() {
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.Pin = xp_PIN;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Pull = GPIO_PULLUP;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init( xp , &GPIO_InitStructure );
	HAL_GPIO_WritePin(xp, xp_PIN, GPIO_PIN_SET);

	GPIO_InitStructure.Pin = xm_PIN;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Pull = GPIO_PULLDOWN;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;

	HAL_GPIO_Init( xm , &GPIO_InitStructure );
	HAL_GPIO_WritePin(xm, xm_PIN, GPIO_PIN_RESET);

	GPIO_InitStructure.Pin = ym_PIN;
	GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
	GPIO_InitStructure.Pull = GPIO_PULLDOWN;
	HAL_GPIO_Init( ym , &GPIO_InitStructure );
	HAL_GPIO_WritePin(ym, ym_PIN, GPIO_PIN_RESET);

	GPIO_InitStructure.Pin = yp_PIN;
	GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	HAL_GPIO_Init( yp , &GPIO_InitStructure );
}

int readChannelXMin() {
	ADC_ChannelConfTypeDef sConfig;
	sConfig.Channel = ADC_CHANNEL_13;
	sConfig.Rank = ADC_REGULAR_RANK_1;
	sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
	int config = HAL_ADC_ConfigChannel(&adcHandle, &sConfig);
	if (config != HAL_OK)
	{
		//
	}
	HAL_Delay(10);
	HAL_ADC_Start(&adcHandle);
	uint32_t ADCValue;
	if(HAL_ADC_PollForConversion(&adcHandle, 5) == HAL_OK) {
		ADCValue = HAL_ADC_GetValue(&adcHandle);
	}
	HAL_ADC_Stop(&adcHandle);
	return ADCValue;
}

int readTouchY() {
	setupForReadY();
	//return readChannelXMin();
	return map(readChannelXMin(), TS_MINY, TS_MAXY, 0, 480);
}

int readChannelYPlus() {
	ADC_ChannelConfTypeDef sConfig;
	sConfig.Channel = ADC_CHANNEL_12;
	sConfig.Rank = ADC_REGULAR_RANK_1;
	sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
	int config = HAL_ADC_ConfigChannel(&adcHandle, &sConfig);
	if (config != HAL_OK)
	{
		//
	}
	HAL_Delay(10);
	HAL_ADC_Start(&adcHandle);
	uint32_t ADCValue;
	if(HAL_ADC_PollForConversion(&adcHandle, 5) == HAL_OK) {
		ADCValue = HAL_ADC_GetValue(&adcHandle);
	}
	HAL_ADC_Stop(&adcHandle);
	return ADCValue;
}

int readTouchX() {
	setupForReadX();
	//return readChannelYPlus();
	return map(readChannelYPlus(), TS_MINX, TS_MAXX, 0, 320);
}
