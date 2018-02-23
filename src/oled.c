/*
 * oled.c
 *
 *  Created on: 19.02.2018
 *      Author: jhbru
 */

#include "oled.h"
#include "i2c.h"

static uint8_t buffer_all[128 * 32 / 8 + 1], *buffer = buffer_all + 1;

static uint16_t currentX = 0, currentY = 0;

void OLED_Command(uint8_t command) {
	I2C_WriteTwoBytes(OLED_ADDR, 0x00, command);
}

void OLED_Init() {
	OLED_Command(SSD1306_DISPLAYOFF); //display off
	OLED_Command(SSD1306_MEMORYMODE); //Set Memory Addressing Mode
	OLED_Command(0x10); //00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
	//OLED_Command(0xB0); //Set Page Start Address for Page Addressing Mode,0-7
	OLED_Command(SSD1306_COMSCANDEC);

	OLED_Command(SSD1306_SETCONTRAST); //--set contrast control register
	OLED_Command(0x81);
	OLED_Command(0xA1); //--set segment re-map 0 to 127
	OLED_Command(SSD1306_SETSTARTLINE | 0x0);
	OLED_Command(SSD1306_NORMALDISPLAY); //--set normal display
	OLED_Command(SSD1306_SETMULTIPLEX); //--set multiplex ratio(1 to 64)
	OLED_Command(31); //
	OLED_Command(SSD1306_SETDISPLAYOFFSET); //-set display offset
	OLED_Command(0x00); //-not offset
	OLED_Command(SSD1306_SETDISPLAYCLOCKDIV); //--set display clock divide ratio/oscillator frequency
	OLED_Command(0xF0); //--set divide ratio
	OLED_Command(SSD1306_SETPRECHARGE); //--set pre-charge period
	OLED_Command(0x22); //
	OLED_Command(SSD1306_SETCOMPINS); //--set com pins hardware configuration
	OLED_Command(0x02);
	OLED_Command(SSD1306_SETVCOMDETECT); //--set vcomh
	OLED_Command(0x20); //0x20,0.77xVcc
	OLED_Command(SSD1306_CHARGEPUMP); //--set DC-DC enable
	OLED_Command(0x14); //
	OLED_Command(SSD1306_SEGREMAP | 0x1);
	OLED_Command(SSD1306_DISPLAYALLON_RESUME); //0xa4,Output follows RAM content;0xa5,Output ignores RAM content
	OLED_Command(SSD1306_DEACTIVATE_SCROLL);

	OLED_Command(SSD1306_DISPLAYON);
}

void OLED_Invert() {
	OLED_Command(SSD1306_INVERTDISPLAY);
}

void OLED_UnInvert() {
	OLED_Command(SSD1306_NORMALDISPLAY);
}

void OLED_Fill(uint8_t color) {
	memset(buffer, color == 0 ? 0x00 : 0xFF, 128 * 32 / 8);
}

void OLED_Display() {
	buffer_all[0] = 0x40;
	I2C_Write(buffer_all, (128 * 32 / 8) + 1, OLED_ADDR);
}

void OLED_DrawPixel(uint16_t x, uint16_t y, uint8_t color) {
	if (x >= 128 || y >= 32) {
		/* Error */
		return;
	}

	/* Set color */
	if (color == 0xFF) {
		buffer[x + (y / 8) * 128] |= 1 << (y % 8);
	} else {
		buffer[x + (y / 8) * 128] &= ~(1 << (y % 8));
	}
}

char OLED_Putc(char ch, FontDef_t* Font, uint8_t color) {
	uint32_t i, b, j;

	b = 0;

	for (i = 0; i < Font->FontHeight; i++) {
		for (j = 0; j < Font->FontWidth; j++) {
			if ((Font->data[ch * Font->CharBytes + b / 8] >> b % 8) & 1) {
				OLED_DrawPixel(currentX + j, (currentY + i), (uint8_t) color);
			} else {
				OLED_DrawPixel(currentX + j, (currentY + i), (uint8_t) !color);
			}
			b++;
		}
	}

	currentX += Font->FontWidth;
	return ch;
}

char OLED_Puts(char* str, FontDef_t* Font, uint8_t color) {
	while (*str) {
		if (OLED_Putc(*str, Font, color) != *str) {
			return *str;
		}

		str++;
	}

	return *str;
}

void OLED_PutPen(uint16_t x, uint16_t y) {
	currentX = x;
	currentY = y;
}

void OLED_DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,
		uint8_t c) {
	int16_t dx, dy, sx, sy, err, e2, i, tmp;

	/* Check for overflow */
	if (x0 >= 128) {
		x0 = 128 - 1;
	}
	if (x1 >= 128) {
		x1 = 128 - 1;
	}
	if (y0 >= 32) {
		y0 = 32 - 1;
	}
	if (y1 >= 32) {
		y1 = 32 - 1;
	}

	dx = (x0 < x1) ? (x1 - x0) : (x0 - x1);
	dy = (y0 < y1) ? (y1 - y0) : (y0 - y1);
	sx = (x0 < x1) ? 1 : -1;
	sy = (y0 < y1) ? 1 : -1;
	err = ((dx > dy) ? dx : -dy) / 2;

	if (dx == 0) {
		if (y1 < y0) {
			tmp = y1;
			y1 = y0;
			y0 = tmp;
		}

		if (x1 < x0) {
			tmp = x1;
			x1 = x0;
			x0 = tmp;
		}

		/* Vertical line */
		for (i = y0; i <= y1; i++) {
			OLED_DrawPixel(x0, i, c);
		}

		/* Return from function */
		return;
	}

	if (dy == 0) {
		if (y1 < y0) {
			tmp = y1;
			y1 = y0;
			y0 = tmp;
		}

		if (x1 < x0) {
			tmp = x1;
			x1 = x0;
			x0 = tmp;
		}

		/* Horizontal line */
		for (i = x0; i <= x1; i++) {
			OLED_DrawPixel(i, y0, c);
		}

		/* Return from function */
		return;
	}

	while (1) {
		OLED_DrawPixel(x0, y0, c);
		if (x0 == x1 && y0 == y1) {
			break;
		}
		e2 = err;
		if (e2 > -dx) {
			err -= dy;
			x0 += sx;
		}
		if (e2 < dy) {
			err += dx;
			y0 += sy;
		}
	}
}
