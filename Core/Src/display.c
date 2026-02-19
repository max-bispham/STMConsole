// Display source code file for STM32 game console!

#include <stdint.h>
#include <string.h>
#include "stm32f1xx_hal.h"
#include "main.h"
#include <stdio.h>
#include <math.h>




// framebuffer for the whole thing
static uint8_t framebuffer[1024];
static uint8_t data_message[1025];

const uint8_t ssdADDR = 0x3C << 1;

static I2C_HandleTypeDef *disp_i2c = NULL;


// local helper functions
static void ssd1306_SendCommand(const uint8_t cmd)
{
	HAL_StatusTypeDef status;
	uint8_t cmd_message[2] = {0x00, cmd};

	if (disp_i2c != NULL) {
		status = HAL_I2C_Master_Transmit(disp_i2c, ssdADDR, cmd_message, 2, HAL_MAX_DELAY);
		if (status == HAL_OK) {
			printf("Message Received!\r\n");
		} else {
			printf("Error\r\n");
		}
	}
}

static void ssd1306_SendCommands(const uint8_t *cmd, uint8_t size)
{
	if (size > 11 || cmd == NULL){return;}
	HAL_StatusTypeDef status;
	uint8_t cmd_message[12];
	cmd_message[0] = 0x00;
	memcpy(&cmd_message[1], cmd, size);
	if (disp_i2c != NULL) {
		status = HAL_I2C_Master_Transmit(disp_i2c, ssdADDR, cmd_message, size + 1, HAL_MAX_DELAY);
	}
}

static void ssd1306_SendData(const uint8_t *data, uint16_t size)
	{
//		uint8_t data_message[129];
		data_message[0] = 0x40;
		memcpy(&data_message[1], data, size);


		if (disp_i2c != NULL) {
			HAL_I2C_Master_Transmit(disp_i2c, ssdADDR, data_message, size + 1, HAL_MAX_DELAY);
		}
	}


static void ssd1306_SetPage(uint8_t page)
{
	if (page <= 7) {
			ssd1306_SendCommand(0xB0 | page);
	}
}

static void ssd1306_SetColumn(uint8_t col)
{
	if (col <= 127) {
		ssd1306_SendCommand(0x00 | (col & 0x0F));
		ssd1306_SendCommand(0x10 | ((col >> 4) & 0x0F));
	}
}


void Display_Update(void)
{

	uint8_t resetcursor[6] = {0x21, 0x00, 0x7F, 0x22, 0x00, 0x07};
	ssd1306_SendCommands(resetcursor, 6);
	ssd1306_SendData(framebuffer, 1024);

}



void Display_Init(I2C_HandleTypeDef *handle)
{
	disp_i2c = handle;

	// turn display off
	ssd1306_SendCommand(0xAE);

	// clock divide / oscillator
	ssd1306_SendCommand(0xD5);
	ssd1306_SendCommand(0x80);

	// Multiplex ratio (64 rows)
	ssd1306_SendCommand(0xA8);
	ssd1306_SendCommand(0x3F);

	// Display offset
	ssd1306_SendCommand(0xD3);
	ssd1306_SendCommand(0x00);

	// start line
	ssd1306_SendCommand(0x40);

	// Power / Charge pump
	ssd1306_SendCommand(0x8D);
	ssd1306_SendCommand(0x14);

	// Memory Addressing mode set to page addressing mode
	ssd1306_SendCommand(0x20);
	ssd1306_SendCommand(0x00);

	// set range commands
	// Column range
	ssd1306_SendCommand(0x21);
	ssd1306_SendCommand(0x00);
	ssd1306_SendCommand(0x7F);

	// Page range
	ssd1306_SendCommand(0x22);
	ssd1306_SendCommand(0x00);
	ssd1306_SendCommand(0x07);



	// Orientation mapping (top left is 0,0)
	ssd1306_SendCommand(0xA1);
	ssd1306_SendCommand(0xC8);

	// COM pins hardware configuration
	ssd1306_SendCommand(0xDA);
	ssd1306_SendCommand(0x12);

	// Contrast and precharge / VCOM
	// Contrast
	ssd1306_SendCommand(0x81);
	ssd1306_SendCommand(0x7F);

	// Precharge period
	ssd1306_SendCommand(0xD9);
	ssd1306_SendCommand(0xF1);

	// VCOMH deselect level
	ssd1306_SendCommand(0xDB);
	ssd1306_SendCommand(0x40);

	// RAM Mode
	ssd1306_SendCommand(0xA4);
	ssd1306_SendCommand(0xA6);
	// Turn display ON
	ssd1306_SendCommand(0xAF);

	Display_Clear();

	Display_Update();
}


// Drawing Tools
void Display_Clear(void)
{
	memset(framebuffer, 0, sizeof(framebuffer));
}

void Display_DrawPixel(int x, int y, uint8_t color)
{
	if ( x > 127 || x < 0 || y > 63 || y < 0) { return; }
	int page_num = y / 8;
	int bit = y % 8;
	int byte_num = (128 * page_num) + x;

	uint8_t old = framebuffer[byte_num];
	uint8_t mask = 0x01 << bit;
	if (color == 1) {
		// color is white
		framebuffer[byte_num] = old | mask;
	} else if (color == 0) {
		// color is black
		framebuffer[byte_num] = old & ~mask;
	}
}

void Display_DrawLine(int x0, int y0, int x1, int y1, uint8_t color)
{
	if (y0 == y1) {
		int lower = (x0 < x1) ? x0 : x1;
		int higher = (x0 > x1) ? x0 : x1;
		for (int xi = lower; xi <= higher; xi++) {
			Display_DrawPixel(xi, y0, color);
		}
		return;
	} else if (x0 == x1) {
		int lower = (y0 < y1) ? y0 : y1;
		int higher = (y0 > y1) ? y0 : y1;
		for (int yi = lower; yi <= higher; yi++) {
			Display_DrawPixel(x0, yi, color);
		}
		return;
	} else {
		// Bresenham
		int dx = abs(x1 - x0);
		int dy = abs(y1 - y0);

		int sx = (x0 < x1) ? 1 : -1; // step x direction
		int sy = (y0 < y1) ? 1 : -1; // step y direction

		int x = x0;
		int y = y0;

		if (dy < dx) {

			int p = 2 * dy - dx;

			for (int i = 0; i <= dx; i++) {

				Display_DrawPixel(x, y, color);

				if (p >= 0) {
					y += sy;
					p -= 2 * dx;
				}
				p += 2 * dy;
				x += sx;
			}
		} else {
			// Steep line: y is the major axis
			int p = 2 * dx - dy;

			for (int i = 0; i <= dy; i++) {
				Display_DrawPixel(x, y, color);

				if (p >= 0) {
					x += sx;
					p -= 2 * dy;
				}
				p += 2 * dx;
				y += sy;
			}
		}
	}
}

void Display_DrawRect(int x, int y, int w, int h, uint8_t color)
{
	int x_offset = w/2;
	int y_offset = h/2;

	int left_x = x - x_offset;
	int right_x = x + x_offset;

	int top_y = y - y_offset;
	int bottom_y = y + y_offset;

	Display_DrawLine(left_x, top_y, right_x, top_y, color);
	Display_DrawLine(right_x, top_y, right_x, bottom_y, color);
	Display_DrawLine(right_x, bottom_y, left_x, bottom_y, color);
	Display_DrawLine(left_x, bottom_y, left_x, top_y, color);

}

void Display_FillRect(int x, int y, int w, int h, uint8_t color)
{
	if (w <= 0 || h <= 0) {
		return;
	}

	// obtain coordinate boundaries
	int x_half = w/2;
	int y_half = h/2;

	int left_x = 0;
	int right_x = 0;
	int top_y = 0;
	int bottom_y = 0;
	int unclipped_left_x = x - x_half;

	// define left_x;
	// checks for partial over on left side and fully over on right side
	if (unclipped_left_x < 0) {
		left_x = 0;
	} else if (unclipped_left_x > 127) {
		return;
	} else {
		left_x = unclipped_left_x;
	}

	// define right_x;
	// checks for partial over on right side and fully over on left side
	if (unclipped_left_x + w - 1 > 127) {
		right_x = 127;
	} else if (unclipped_left_xx + w - 1 < 0) {
		return;
	} else {
		right_x = unclipped_left_x + w - 1;
	}

	// reset the width
	w = right_x - left_x + 1;

	// define top_y;
	if (y - y_half < 0) {
		top_y = 0;
	} else if (y - y_half > 63) {
		return;
	} else {
		top_y = y - y_half;
	}

	// define bottom_y;
	if (top_y + h - 1 > 63) {
		bottom_y = 63;
	} else if (top_y + h - 1 < 0) {
		return;
	} else {
		bottom_y = top_y + h - 1;
	}

	// reset the height
	h = bottom_y - top_y + 1;

	// get page numbers
	int start_page = top_y / 8;
	int stop_page = bottom_y / 8;
	int pages = stop_page - start_page + 1;
	// bit offsets
	int start_bit = top_y % 8;
	int stop_bit = bottom_y % 8;

	uint8_t top_mask = 0xFF << start_bit;
	uint8_t bottom_mask = stop_bit < 7 ? ~(0xff << (stop_bit + 1)) : 0xFF;

	// for lengths less than 9 tall
	if (start_page == stop_page) {
		uint8_t mask = top_mask & bottom_mask;
		for (int j = 0; j < w; j++) {
			uint8_t old = framebuffer[(128 * start_page) + left_x + j];
			framebuffer[(128 * start_page) + left_x + j] = color == 1 ? old | mask : old & ~mask;
			}
		return;
	}

	for (int i = 0; i < pages; i++){
		uint8_t mask = 0xFF;

		// mask selection
		if (i == 0) {
			mask = top_mask;
			for (int j = 0; j < w; j++) {
				uint8_t old = framebuffer[(128 * (start_page + i)) + left_x + j];
				framebuffer[(128 * (start_page + i)) + left_x + j] = color == 1 ? old | mask : old & ~mask;
				}
		} else if (i == pages - 1){
			mask = bottom_mask;
			for (int j = 0; j < w; j++) {
				uint8_t old = framebuffer[(128 * (start_page + i)) + left_x + j];
				framebuffer[(128 * (start_page + i)) + left_x + j] = color == 1 ? old | mask : old & ~mask;
			}
		} else {
			for (int j = 0; j < w; j++) {
				framebuffer[(128 * (start_page + i)) + left_x + j] = color == 1 ? (uint8_t)0xFF : 0;
			}
		}
	}
}

void Display_DrawSprite(int x, int y, const uint8_t *bitmap, int w, int h)
{
	if (w <= 0 || h <= 0) {
		return;
	}

	//int length = w * h;

	// obtain coordinate boundaries
	int x_half = w/2;
	int y_half = h/2;

	int left_x = 0;
	int right_x = 0;
	int top_y = 0;
	int bottom_y = 0;

	//int x_offset = 0;
//	int left_clipped = 0;
	//int right_clipped = 0;

	int sprite_left_bound = 0;

	// define left_x;
	// checks for partial over on left side and fully over on right side

	int unclipped_left_x = x - x_half;

	if (unclipped_left_x < 0) {
		left_x = 0;
		sprite_left_bound = -unclipped_left_x;
	} else if (unclipped_left_x > 127) {
		return;
	} else {
		left_x = unclipped_left_x;
	}

	// define right_x;
	// checks for partial over on right side and fully over on left side
	if (unclipped_left_x + w - 1 > 127) {
		right_x = 127;
	} else if (unclipped_left_x + w - 1 < 0) {
		return;
	} else {
		right_x = unclipped_left_x + w - 1;
	}

	// get the copy width
	int copy_w = right_x - left_x + 1;

	// define top_y;
	if (y - y_half < 0) {
		top_y = 0;
	} else if (y - y_half > 63) {
		return;
	} else {
		top_y = y - y_half;
	}

	// define bottom_y;
	if (top_y + h - 1 > 63) {
		bottom_y = 63;
	} else if (top_y + h - 1 < 0) {
		return;
	} else {
		bottom_y = top_y + h - 1;
	}

	// get the copy height
	//int copy_h = bottom_y - top_y + 1;

	// get page numbers
	int start_page = top_y / 8;
	//int stop_page = bottom_y / 8;
	//int pages = stop_page - start_page + 1;

	// bit offsets
	int start_bit = top_y % 8;
	//int stop_bit = bottom_y % 8;



	// clipping still needed, this one assumes it is page aligned

	int sprite_pages = h / 8;
	if (h % 8 != 0) {
		sprite_pages++;
	}
	if (start_bit == 0) {
		for (int i = 0; i < sprite_pages; i++) {
			for (int j = 0; j < copy_w; j++) {
				framebuffer[(start_page + i) * 128 + left_x + j] |= bitmap[i * w + sprite_left_bound + j];
			}
		}
	} else if (start_bit != 0 ) { // not page aligned
		for (int i = 0; i < sprite_pages; i++) {
			if (i == sprite_pages - 1) {
				for (int j = 0; j < copy_w; j++) {
				uint8_t top_part = bitmap[i * w + sprite_left_bound + j] << start_bit;
				framebuffer[(start_page + i) * 128 + left_x + j] |= top_part;
				}
			} else {
				for (int j = 0; j < copy_w; j++) {
					// split the current sprite byte into two parts, put the top byte in the bottom of the current framebuffer page, and the bottom byte in the top of the next framebuffer page
					uint8_t top_part = bitmap[i * w + sprite_left_bound + j] << start_bit;
					framebuffer[(start_page + i) * 128 + left_x + j] |= top_part;
					uint8_t bottom_part = bitmap[i * w + sprite_left_bound + j] >> (8 - start_bit);
					framebuffer[(start_page + i + 1) * 128 + left_x + j] |= bottom_part;
				}
			}
		}
	}
}








