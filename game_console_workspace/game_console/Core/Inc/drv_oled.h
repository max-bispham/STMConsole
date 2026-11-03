/*
 * drv_oled.h
 *
 *  Created on: Oct 24, 2026
 *      Author: max
 */

#ifndef INC_DRV_OLED_H_
#define INC_DRV_OLED_H_

#include "driver.h"

#ifdef __cplusplus
extern "C" {
#endif

	extern const driver_t dev_display;

	#define DISPLAY_SET_CONTRAST  1
	#define DISPLAY_SET_POWER     2
	#define DISPLAY_SET_INVERT    3
	#define DISPLAY_TEST_ALL_ON   4

#ifdef __cplusplus
}
#endif

#endif /* INC_DRV_OLED_H_ */
