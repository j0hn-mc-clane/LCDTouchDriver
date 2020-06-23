/*
 Library:					SPI LCD - HX8357D
 Written by:				Joachim Cardoen
 Date Written:			23/02/2020
 Last modified:		-/-
 Description:			This is an STM32 device driver library for the Adafruit 2050 SPI LCD display, using STM HAL libraries

 * Copyright (C) 2020 - Joachim Cardoen
 This is a free software under the GNU license, you can redistribute it and/or modify it under the terms
 of the GNU General Public Licenseversion 3 as published by the Free Software Foundation.

 This software library is shared with puplic for educational purposes, without WARRANTY and Author is not liable for any damages caused directly
 or indirectly by this software, read more about this on the GNU General Public License.
 */

//Header files
#include <LCD_Driver.h>

static uint8_t rotationNum = 1;
static bool _cp437 = false;
static SPI_HandleTypeDef lcdSPIhandle;
//Chip Select pin
static GPIO_TypeDef *tftCS_GPIO;
static uint16_t tftCS_PIN;
//Data Command pin
static GPIO_TypeDef *tftDC_GPIO;
static uint16_t tftDC_PIN;
//Reset pin
static GPIO_TypeDef *tftRESET_GPIO;
static uint16_t tftRESET_PIN;


//***** Functions prototypes *****//

//1. Write Command to LCD
void LCD_SendCommand(uint8_t com) {
	//*(__IO uint8_t *)(0x60000000) = com;
	uint8_t tmpCmd = com;
	//Set DC HIGH for COMMAND mode
	HAL_GPIO_WritePin(tftDC_GPIO, tftDC_PIN, GPIO_PIN_RESET);
	//Put CS LOW
	HAL_GPIO_WritePin(tftCS_GPIO, tftCS_PIN, GPIO_PIN_RESET);
	//Write byte using SPI
	HAL_SPI_Transmit(&lcdSPIhandle, &tmpCmd, 1, 5);
	//Bring CS HIGH
	HAL_GPIO_WritePin(tftCS_GPIO, tftCS_PIN, GPIO_PIN_SET);
}

//2. Write data to LCD
void LCD_SendData(uint8_t data) {
	//*(__IO uint8_t *)(0x60040000) = data;
	uint8_t tmpCmd = data;
	//Set DC LOW for DATA mode
	HAL_GPIO_WritePin(tftDC_GPIO, tftDC_PIN, GPIO_PIN_SET);
	//Put CS LOW
	HAL_GPIO_WritePin(tftCS_GPIO, tftCS_PIN, GPIO_PIN_RESET);
	//Write byte using SPI
	HAL_SPI_Transmit(&lcdSPIhandle, &tmpCmd, 1, 5);
	//Bring CS HIGH
	HAL_GPIO_WritePin(tftCS_GPIO, tftCS_PIN, GPIO_PIN_SET);
}
//2.2 Write multiple/DMA
void LCD_SendDataMulti(uint16_t Colordata, uint32_t size) {
	uint8_t colorL, colorH;

	//Set DC LOW for DATA mode
	HAL_GPIO_WritePin(tftDC_GPIO, tftDC_PIN, GPIO_PIN_SET);
	//Put CS LOW
	HAL_GPIO_WritePin(tftCS_GPIO, tftCS_PIN, GPIO_PIN_RESET);
	//Write byte using SPI
	HAL_SPI_Transmit(&lcdSPIhandle, (uint8_t*) &Colordata, size, 1000);
	//Wait for end of DMA transfer
	//Bring CS HIGH
	HAL_GPIO_WritePin(tftCS_GPIO, tftCS_PIN, GPIO_PIN_SET);
}

//3. Set cursor position
void LCD_SetCursorPosition(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {

	LCD_SendCommand(HX8357_CASET);
	LCD_SendData(x1 >> 8);
	LCD_SendData(x1 & 0xFF);
	LCD_SendData(x2 >> 8);
	LCD_SendData(x2 & 0xFF);

	LCD_SendCommand(HX8357_PASET);
	LCD_SendData(y1 >> 8);
	LCD_SendData(y1 & 0xFF);
	LCD_SendData(y2 >> 8);
	LCD_SendData(y2 & 0xFF);
	LCD_SendCommand(HX8357_RAMWR);
}
//4. Initialise function
void LCD_Init(SPI_HandleTypeDef *spiLcdHandle, GPIO_TypeDef *csPORT,
		uint16_t csPIN, GPIO_TypeDef *dcPORT, uint16_t dcPIN,
		GPIO_TypeDef *resetPORT, uint16_t resetPIN) {
	//Copy SPI settings
	memcpy(&lcdSPIhandle, spiLcdHandle, sizeof(*spiLcdHandle));
	//CS pin
	tftCS_GPIO = csPORT;
	tftCS_PIN = csPIN;
	//DC pin
	tftDC_GPIO = dcPORT;
	tftDC_PIN = dcPIN;
	HAL_GPIO_WritePin(tftCS_GPIO, tftCS_PIN, GPIO_PIN_SET);
	//RESET pin
	tftRESET_GPIO = resetPORT;
	tftRESET_PIN = resetPIN;
	HAL_GPIO_WritePin(resetPORT, resetPIN, GPIO_PIN_SET);  //Turn LCD ON

	LCD_SendCommand(HX8357_SWRESET);
	LCD_SendCommand(0x80);
	HAL_Delay(10);
	LCD_SendCommand(HX8357D_SETC);
	LCD_SendData(0xFF);
	LCD_SendData(0x83);
	LCD_SendData(0x57);
	LCD_SendCommand(0xFF);
	LCD_SendCommand(0x80);
	HAL_Delay(300);
	LCD_SendCommand(HX8357_SETRGB);
	LCD_SendData(0x80);
	LCD_SendData(0x00);
	LCD_SendData(0x06);
	LCD_SendData(0x06);
	LCD_SendCommand(HX8357D_SETCOM);
	LCD_SendData(0x25);
	LCD_SendCommand(HX8357_SETOSC);
	LCD_SendData(0x68);
	LCD_SendCommand(HX8357_SETPANEL);
	LCD_SendData(0x05);
	LCD_SendCommand(HX8357_SETPWR1);
	LCD_SendData(0x00);
	LCD_SendData(0x15);
	LCD_SendData(0x1C);
	LCD_SendData(0x1C);
	LCD_SendData(0x83);
	LCD_SendData(0xAA);
	LCD_SendCommand(HX8357D_SETSTBA);
	LCD_SendData(0x50);
	LCD_SendData(0x50);
	LCD_SendData(0x01);
	LCD_SendData(0x3C);
	LCD_SendData(0x1E);
	LCD_SendData(0x08);
	LCD_SendCommand(HX8357D_SETCYC);
	LCD_SendData(0x02);
	LCD_SendData(0x40);
	LCD_SendData(0x00);
	LCD_SendData(0x2A);
	LCD_SendData(0x2A);
	LCD_SendData(0x0D);
	LCD_SendData(0x78);
	LCD_SendCommand(HX8357D_SETGAMMA);

	LCD_SendData(0x02);
	LCD_SendData(0x0A);
	LCD_SendData(0x11);
	LCD_SendData(0x1D);
	LCD_SendData(0x23);
	LCD_SendData(0x35);
	LCD_SendData(0x41);
	LCD_SendData(0x4b);
	LCD_SendData(0x4b);
	LCD_SendData(0x42);
	LCD_SendData(0x3A);
	LCD_SendData(0x27);
	LCD_SendData(0x1B);
	LCD_SendData(0x08);
	LCD_SendData(0x09);
	LCD_SendData(0x03);
	LCD_SendData(0x02);
	LCD_SendData(0x0A);
	LCD_SendData(0x11);
	LCD_SendData(0x1d);
	LCD_SendData(0x23);
	LCD_SendData(0x35);
	LCD_SendData(0x41);
	LCD_SendData(0x4b);
	LCD_SendData(0x4b);
	LCD_SendData(0x42);
	LCD_SendData(0x3A);
	LCD_SendData(0x27);
	LCD_SendData(0x1B);
	LCD_SendData(0x08);
	LCD_SendData(0x09);
	LCD_SendData(0x03);
	LCD_SendData(0x00);
	LCD_SendData(0x01);

	LCD_SendCommand(HX8357_COLMOD);
	LCD_SendData(0x55);
	LCD_SendCommand(HX8357_MADCTL);
	LCD_SendData(0xC0);
	LCD_SendCommand(HX8357_TEON);
	LCD_SendData(0x00);
	LCD_SendCommand(HX8357_TEARLINE);
	LCD_SendDataMulti(0x00, 0x02);
	LCD_SendCommand(HX8357_SLPOUT);
	LCD_SendCommand(0x80);
	HAL_Delay(150);
	LCD_SendCommand(HX8357_DISPON);
	LCD_SendCommand(0x80);
	HAL_Delay(50);
}

//5. Write data to a single pixel
void LCD_DrawPixel(uint16_t x, uint16_t y, uint16_t color) {
	LCD_SetCursorPosition(x, y, x, y);
	LCD_SendData(color >> 8);
	LCD_SendData(color & 0xFF);
}
//6. Fill the entire screen with a background color
void LCD_Fill(uint16_t color) {
	uint32_t n = LCD_PIXEL_COUNT;
	uint16_t myColor = 0xFF;

	if (rotationNum == 1 || rotationNum == 3) {
		LCD_SetCursorPosition(0, 0, HX8357_TFTWIDTH - 1, HX8357_TFTHEIGHT - 1);
	} else if (rotationNum == 2 || rotationNum == 4) {
		LCD_SetCursorPosition(0, 0, HX8357_TFTHEIGHT - 1, HX8357_TFTWIDTH - 1);
	}

	while (n) {
		n--;
		LCD_SendData(color >> 8);
		LCD_SendData(color & 0xff);
	}
}
//7. Rectangle drawing functions
void LCD_Fill_Rect(unsigned int x0, unsigned int y0, unsigned int x1,
		unsigned int y1, uint16_t color) {
	uint32_t n = ((x1 + 1) - x0) * ((y1 + 1) - y0);
	if (n > LCD_PIXEL_COUNT)
		n = LCD_PIXEL_COUNT;
	LCD_SetCursorPosition(x0, y0, x1, y1);
	while (n) {
		n--;
		LCD_SendData(color >> 8);
		LCD_SendData(color & 0xff);
	}
}

//8. Circle drawing functions
void LCD_drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color) {
	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;

	LCD_DrawPixel(x0, y0 + r, color);
	LCD_DrawPixel(x0, y0 - r, color);
	LCD_DrawPixel(x0 + r, y0, color);
	LCD_DrawPixel(x0 - r, y0, color);

	while (x < y) {
		if (f >= 0) {
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;

		LCD_DrawPixel(x0 + x, y0 + y, color);
		LCD_DrawPixel(x0 - x, y0 + y, color);
		LCD_DrawPixel(x0 + x, y0 - y, color);
		LCD_DrawPixel(x0 - x, y0 - y, color);
		LCD_DrawPixel(x0 + y, y0 + x, color);
		LCD_DrawPixel(x0 - y, y0 + x, color);
		LCD_DrawPixel(x0 + y, y0 - x, color);
		LCD_DrawPixel(x0 - y, y0 - x, color);
	}
}
static void drawCircleHelper(int16_t x0, int16_t y0, int16_t r,
		uint8_t cornername, uint16_t color) {
	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;

	while (x < y) {
		if (f >= 0) {
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;
		if (cornername & 0x4) {
			LCD_DrawPixel(x0 + x, y0 + y, color);
			LCD_DrawPixel(x0 + y, y0 + x, color);
		}
		if (cornername & 0x2) {
			LCD_DrawPixel(x0 + x, y0 - y, color);
			LCD_DrawPixel(x0 + y, y0 - x, color);
		}
		if (cornername & 0x8) {
			LCD_DrawPixel(x0 - y, y0 + x, color);
			LCD_DrawPixel(x0 - x, y0 + y, color);
		}
		if (cornername & 0x1) {
			LCD_DrawPixel(x0 - y, y0 - x, color);
			LCD_DrawPixel(x0 - x, y0 - y, color);
		}
	}
}
static void fillCircleHelper(int16_t x0, int16_t y0, int16_t r,
		uint8_t cornername, int16_t delta, uint16_t color) {
	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;

	while (x < y) {
		if (f >= 0) {
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;

		if (cornername & 0x1) {
			LCD_drawFastVLine(x0 + x, y0 - y, 2 * y + 1 + delta, color);
			LCD_drawFastVLine(x0 + y, y0 - x, 2 * x + 1 + delta, color);
		}
		if (cornername & 0x2) {
			LCD_drawFastVLine(x0 - x, y0 - y, 2 * y + 1 + delta, color);
			LCD_drawFastVLine(x0 - y, y0 - x, 2 * x + 1 + delta, color);
		}
	}
}
void LCD_fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color) {
	LCD_drawFastVLine(x0, y0 - r, 2 * r + 1, color);
	fillCircleHelper(x0, y0, r, 3, 0, color);
}

//9. Line drawing functions
void LCD_drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
		uint16_t color) {
	int16_t steep = abs(y1 - y0) > abs(x1 - x0);
	if (steep) {
		swap(x0, y0);
		swap(x1, y1);
	}

	if (x0 > x1) {
		swap(x0, x1);
		swap(y0, y1);
	}

	int16_t dx, dy;
	dx = x1 - x0;
	dy = abs(y1 - y0);

	int16_t err = dx / 2;
	int16_t ystep;

	if (y0 < y1) {
		ystep = 1;
	} else {
		ystep = -1;
	}

	for (; x0 <= x1; x0++) {
		if (steep) {
			LCD_DrawPixel(y0, x0, color);
		} else {
			LCD_DrawPixel(x0, y0, color);
		}
		err -= dy;
		if (err < 0) {
			y0 += ystep;
			err += dx;
		}
	}
}

void LCD_drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) {
	LCD_drawLine(x, y, x + w - 1, y, color);
}

void LCD_drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) {
	LCD_drawLine(x, y, x, y + h - 1, color);
}
//10. Triangle drawing
void LCD_drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
		int16_t x2, int16_t y2, uint16_t color) {
	LCD_drawLine(x0, y0, x1, y1, color);
	LCD_drawLine(x1, y1, x2, y2, color);
	LCD_drawLine(x2, y2, x0, y0, color);
}
void LCD_fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
		int16_t x2, int16_t y2, uint16_t color) {
	int16_t a, b, y, last;

	// Sort coordinates by Y order (y2 >= y1 >= y0)
	if (y0 > y1) {
		swap(y0, y1);
		swap(x0, x1);
	}
	if (y1 > y2) {
		swap(y2, y1);
		swap(x2, x1);
	}
	if (y0 > y1) {
		swap(y0, y1);
		swap(x0, x1);
	}

	if (y0 == y2) { // Handle awkward all-on-same-line case as its own thing
		a = b = x0;
		if (x1 < a)
			a = x1;
		else if (x1 > b)
			b = x1;
		if (x2 < a)
			a = x2;
		else if (x2 > b)
			b = x2;
		LCD_drawFastHLine(a, y0, b - a + 1, color);
		return;
	}

	int16_t dx01 = x1 - x0, dy01 = y1 - y0, dx02 = x2 - x0, dy02 = y2 - y0,
			dx12 = x2 - x1, dy12 = y2 - y1, sa = 0, sb = 0;

	// For upper part of triangle, find scanline crossings for segments
	// 0-1 and 0-2.  If y1=y2 (flat-bottomed triangle), the scanline y1
	// is included here (and second loop will be skipped, avoiding a /0
	// error there), otherwise scanline y1 is skipped here and handled
	// in the second loop...which also avoids a /0 error here if y0=y1
	// (flat-topped triangle).
	if (y1 == y2)
		last = y1;   // Include y1 scanline
	else
		last = y1 - 1; // Skip it

	for (y = y0; y <= last; y++) {
		a = x0 + sa / dy01;
		b = x0 + sb / dy02;
		sa += dx01;
		sb += dx02;

		if (a > b)
			swap(a, b);
		LCD_drawFastHLine(a, y, b - a + 1, color);
	}

	// For lower part of triangle, find scanline crossings for segments
	// 0-2 and 1-2.  This loop is skipped if y1=y2.
	sa = dx12 * (y - y1);
	sb = dx02 * (y - y0);
	for (; y <= y2; y++) {
		a = x1 + sa / dy12;
		b = x0 + sb / dy02;
		sa += dx12;
		sb += dx02;

		if (a > b)
			swap(a, b);
		LCD_drawFastHLine(a, y, b - a + 1, color);
	}
}

//12. Image print (RGB 565, 2 bytes per pixel)
void LCD_printImage(uint16_t y, uint16_t w, uint8_t *data,
		uint32_t size) {
	uint32_t n = size;
	LCD_SetCursorPosition(0, y, w-1, y);

	for (uint32_t i = 0; i < n; i++) {
		LCD_SendData(data[i]);
	}

	LCD_SendData(data[size-1] & 0xFF);
}

//13. Set screen rotation
void LCD_setRotation(uint8_t rotate) {
	switch (rotate) {
	case 1:
		rotationNum = 1;
		LCD_SendCommand(HX8357_MADCTL);
		LCD_SendData(MADCTL_MX | MADCTL_MY | MADCTL_RGB);
		break;
	case 2:
		rotationNum = 2;
		LCD_SendCommand(HX8357_MADCTL);
		LCD_SendData(MADCTL_MV | MADCTL_MY | MADCTL_RGB);
		break;
	case 3:
		rotationNum = 3;
		LCD_SendCommand(HX8357_MADCTL);
		LCD_SendData(MADCTL_RGB);
		break;
	case 4:
		rotationNum = 4;
		LCD_SendCommand(HX8357_MADCTL);
		LCD_SendData(MADCTL_MX | MADCTL_MV | MADCTL_RGB);
		break;
	default:
		rotationNum = 1;
		LCD_SendCommand(HX8357_MADCTL);
		LCD_SendData(MADCTL_MY | MADCTL_BGR);
		break;
	}
}
//14. Get screen rotation
uint8_t LCD_getRotation(void) {
	return rotationNum;
}
