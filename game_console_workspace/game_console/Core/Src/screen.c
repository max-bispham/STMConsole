/*
 * screen.c
 *
 *  Created on: Oct 25, 2026
 *      Author: max
 */
#include "screen.h"


static uint8_t buffer[SCREEN_BYTES] = {0};
void screen_clear(void);
void screen_set_pixel(int x, int y, int on);
void screen_blit_chip8(const uint8_t *video);


const uint8_t *screen_buffer(void) {
		return buffer;
}

void screen_clear(void) {
	memset(buffer, 0, sizeof(buffer));
}

void screen_set_pixel(int x, int y, int on) {
	if (x < 0 || x >= SCREEN_WIDTH || y < 0 || y >= SCREEN_HEIGHT) return;
	int page = y / 8;
	int bit = y % 8;
	int column = x;
	int index = (page * SCREEN_WIDTH) + column;
	buffer[index] = (buffer[index] & ~(1U << bit)) | ((on ? 0x01 : 0) << bit);
}

