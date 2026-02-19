// Implementation of input.h!

#include <stdint.h>
#include "input.h"
#include <string.h>
#include "stm32f1xx_hal.h"
#include "main.h"

#define NUM_BUTTONS 6

#define pressed 0
#define held 1
#define released 2

// Game state variables
// InputState diagram
// |
static uint32_t InputState;
static uint8_t debounce_time = 5;

// input state variables
static uint8_t raw_state[NUM_BUTTONS];
static uint8_t debounced_state[NUM_BUTTONS];
static uint8_t last_state[NUM_BUTTONS];
static uint32_t last_change_ms[NUM_BUTTONS];




void Input_Init(void)
{
	memset(raw_state, 0, sizeof(raw_state));
	memset(debounced_state, 0, sizeof(debounced_state));
	memset(last_state, 0, sizeof(last_state));
	memset(last_change_ms, 0, sizeof(last_change_ms));
	InputState = 0;
}


void Input_OnExtiInterrupt(ButtonId button_id)
{
	// Getting current time
	uint32_t now = HAL_GetTick();
	if (now - last_change_ms[button_id] > debounce_time)
	{
		// creating the proper pin mask by shifting an unsigned 1 the amount of the pin number
		uint16_t pin_mask = (uint16_t)(1U << button_id);
		raw_state[button_id] = HAL_GPIO_ReadPin(GPIOA, pin_mask);
		debounced_state[button_id] = raw_state[button_id];

		// resetting the most recent input to calculate later
		last_change_ms[button_id] = now;

	}

}

void Input_UpdatePerFrame(void)
{


	for ( int i = 0; i < NUM_BUTTONS; i++ )
	{
		if (debounced_state[i] == 0 && last_state[i] == 0)
		{
			InputState &= ~(1U << (i * 3 + pressed));
			InputState &= ~(1U << (i * 3 + held));
			InputState &= ~(1U << (i * 3 + released));
		} else if (debounced_state[i] == 0 && last_state[i] == 1)
		{
			InputState &= ~(1U << (i * 3 + pressed));
			InputState &= ~(1U << (i * 3 + held));
			InputState |= (1U << (i * 3 + released));
		} else if (debounced_state[i] == 1 && last_state[i] == 0)
		{
			InputState |= (1U << (i * 3 + pressed));
			InputState |= (1U << (i * 3 + held));
			InputState &= ~(1U << (i * 3 + released));
		} else if (debounced_state[i] == 1 && last_state[i] == 1)
		{
			InputState &= ~(1U << (i * 3 + pressed));
			InputState |= (1U << (i * 3 + held));
			InputState &= ~(1U << (i * 3 + released));
		}
		last_state[i] = debounced_state[i];
	}
}

uint32_t Input_GetState(void)
{
	return InputState;
}

void Input_ClearState(void)
{
	// this bit mask preserves the held state for each button
	//0000 0000 0000 0001 0010 0100 1001 0010
	uint32_t clear_bitmask = 0x0012492;
	InputState &= clear_bitmask;
}
