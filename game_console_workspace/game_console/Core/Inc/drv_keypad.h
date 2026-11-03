/*
 * drv_keypad.h
 *
 *  Created on: Nov 1, 2026
 *      Author: max
 */

#ifndef INC_DRV_KEYPAD_H_
#define INC_DRV_KEYPAD_H_

#include "driver.h"

#ifdef __cplusplus
extern "C" {
#endif

	extern const driver_t dev_keypad;

	void keypad_tick(void);

#ifdef __cplusplus
}
#endif

#endif /* INC_DRV_KEYPAD_H_ */
