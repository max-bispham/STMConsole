/*
 * screen.c
 *
 *  Created on: Oct 25, 2026
 *      Author: max
 */
#include "screen.h"
#include <string.h>

#define CHIP8_WIDTH  64
#define CHIP8_HEIGHT 32
#define CHIP8_SIZE   2048

static uint8_t buffer[SCREEN_BYTES] = {0};

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

void screen_blit_chip8(const uint8_t *video) {
	for (int i =  0; i < CHIP8_SIZE; i++) {
		int x = i % CHIP8_WIDTH;
		int y = i / CHIP8_WIDTH;
		int state = (video[i] ? 1 : 0);

		screen_set_pixel(2 * x, 2 * y, state);
		screen_set_pixel(2 * x + 1, 2 * y, state);
		screen_set_pixel(2 * x, 2 * y + 1, state);
		screen_set_pixel(2 * x + 1, 2 * y + 1, state);
	}
}
