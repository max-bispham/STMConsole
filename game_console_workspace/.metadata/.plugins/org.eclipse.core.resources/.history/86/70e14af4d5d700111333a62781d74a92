/*
 * screen.h
 *
 *  Created on: Oct 25, 2026
 *      Author: max
 */

#ifndef INC_SCREEN_H_
#define INC_SCREEN_H_

#include "stdint.h"
#include <string.h>

#define SCREEN_WIDTH   128
#define SCREEN_HEIGHT  64
#define SCREEN_BYTES   1024

void screen_clear(void);
void screen_set_pixel(int x, int y, int on);    /* (0,0) is top left; out of range is ignored */
void screen_blit_chip8(const uint8_t *video);   /* 64 x 32 bytes, nonzero = on, drawn at 2x */
const uint8_t *screen_buffer(void);             /* pass this to dev_display.write */

#endif /* INC_SCREEN_H_ */
