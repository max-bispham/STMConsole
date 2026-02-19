// Input subsystem API for the STM32 Game Console Project!
#ifndef INPUT_H
#define INPUT_H

# include <stdint.h>

typedef enum {
	BUTTON_CONFIRM,
	BUTTON_MENU,
	BUTTON_RIGHT,
	BUTTON_LEFT,
	BUTTON_DOWN,
	BUTTON_UP,
} ButtonId;



void Input_Init(void);

/*

GPIO pins for your 4 buttons

EXTI triggers for rising + falling edges

Internal tracking variables (raw state, debounced state, last_frame state, timestamps)

*/


void Input_OnExtiInterrupt(ButtonId button_id);

/*

reads the GPIO level

updates the raw state

sets up or updates debouncing

stores timestamps if needed

does NOT produce pressed/held/released yet

*/


void Input_UpdatePerFrame(void);

/*

look at the debounced button states

compare with previous frame

build the new InputState

store it in a static variable inside input.c

update last frame’s state for next frame

*/


uint32_t Input_GetState(void);

/*

 Returns a copy of the current per-frame InputState struct

*/

void Input_ClearState(void);

/// Clears pressed/held/release states

#endif
