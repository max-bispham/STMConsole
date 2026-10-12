/*
 * driver.c
 *
 *  Created on: Oct 12, 2026
 *      Author: max
 */

#include "driver.h"

int drv_nop(void) {
	return DRV_OK;
}

int drv_unsupported_read(uint8_t *buf, size_t len) {
	return DRV_ENOTSUP;
}

int drv_unsupported_write(const uint8_t *buf, size_t len){
	return DRV_ENOTSUP;
}

int drv_unsupported_ioctl(int request, void *arg){
	return DRV_ENOTSUP;
}
