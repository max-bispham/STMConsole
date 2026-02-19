// Header file for stm32 game console!

#ifndef DISPLAY_H

#define DISPLAY_H



#include <stdint.h>

#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 64

// Functions:
// Initialization

void Display_Init(I2C_HandleTypeDef *handle);


// Drawing Tools
void Display_Clear(void);

void Display_DrawPixel(int x, int y, uint8_t color);

void Display_DrawLine(int x, int y, int x1, int y1, uint8_t color);

void Display_DrawRect(int x, int y, int w, int h, uint8_t color);

void Display_FillRect(int x, int y, int w, int h, uint8_t color);

void Display_DrawSprite(int x, int y, const uint8_t *bitmap, uint8_t w, uint8_t h);


// Update
void Display_Update(void);


#endif
