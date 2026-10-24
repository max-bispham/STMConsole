/*
 * drv_oled.c
 *
 *  Created on: Oct 24, 2026
 *      Author: max
 */

#include "drv_oled.h"
#include "main.h"

extern SPI_HandleTypeDef hspi1;

static const uint8_t init_seq[] = {
    0xAE, 0xD5, 0x80, 0xA8, 0x3F, 0xD3, 0x00, 0x40,
    0x8D, 0x14, 0x20, 0x00, 0xA1, 0xC8, 0xDA, 0x12,
    0x81, 0xCF, 0xD9, 0xF1, 0xDB, 0x40, 0xA4, 0xA6, 0xAF
};


static int oled_open(void);
static int oled_close(void);
//static int oled_write(const uint8_t *buf, size_t len);
static int oled_ioctl(int request, void *arg);

static void oled_reset(void);
static int oled_cmd(uint8_t cmd);

const driver_t dev_display = {
		.open = oled_open,
		.close = oled_close,
		.read = drv_unsupported_read,
		.write = drv_unsupported_write,
		.ioctl = oled_ioctl
};


static int oled_open(void) {

	oled_reset();

	size_t len = sizeof(init_seq) / sizeof(init_seq[0]);
	for (int i = 0; i < len; i++) {
		if (oled_cmd(init_seq[i])) return DRV_ERR;
	}
	return DRV_OK;
}

static int oled_close(void) {
	return oled_cmd(0xAE);
}

//static int oled_write(const uint8_t *buf, size_t len) {
//
//}

static int oled_ioctl(int request, void *arg) {
	if (arg == NULL) return DRV_EINVAL;
	const uint8_t *p = arg;
	uint8_t value = *p;
	switch (request) {
	case DISPLAY_SET_CONTRAST:
		if (oled_cmd(0x81) != DRV_OK) {
			return DRV_ERR;
		}
		return oled_cmd(value);
	case DISPLAY_SET_POWER:
		return oled_cmd(value ? 0xAF : 0xAE);
	case DISPLAY_SET_INVERT:
		return oled_cmd(value ? 0xA7 : 0xA6);
	case DISPLAY_TEST_ALL_ON:
		return oled_cmd(value ? 0xA5 : 0xA4);
	default:
		return DRV_EINVAL;
	}
}

static void oled_reset(void) {
	HAL_GPIO_WritePin(OLED_RES_GPIO_Port, OLED_RES_Pin, GPIO_PIN_RESET);
	HAL_Delay(10);
	HAL_GPIO_WritePin(OLED_RES_GPIO_Port, OLED_RES_Pin, GPIO_PIN_SET);
	HAL_Delay(10);
}

static int oled_cmd(uint8_t cmd) {
	// Pull DC low to signal command to oled
	HAL_GPIO_WritePin(OLED_DC_GPIO_Port, OLED_DC_Pin, GPIO_PIN_RESET);

	// Pull CS low to initiate SPI communication with oled
	HAL_GPIO_WritePin(OLED_CS_GPIO_Port, OLED_CS_Pin, GPIO_PIN_RESET);

	// Sent command
	HAL_StatusTypeDef err = HAL_SPI_Transmit(&hspi1, &cmd, 1, 500);

	// set CS back high
	HAL_GPIO_WritePin(OLED_CS_GPIO_Port, OLED_CS_Pin, GPIO_PIN_SET);

	if (err) {
		return DRV_ERR;
	} else {
		return DRV_OK;
	}
}
