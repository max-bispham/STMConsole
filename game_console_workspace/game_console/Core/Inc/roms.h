/*
 * roms.h
 *
 *  Created on: Nov 3, 2026
 *      Author: max
 */

#ifndef INC_ROMS_H_
#define INC_ROMS_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

	typedef struct {
		const char    *name;
		const uint8_t *data;
		uint16_t       size;
		uint8_t        keymap[8];   /* CHIP-8 key (0x0 to 0xF) sent by each button, 0 to 7 */
	} rom_t;

	extern const rom_t    roms[];
	extern const unsigned rom_count;

#ifdef __cplusplus
}
#endif

#endif /* INC_ROMS_H_ */
