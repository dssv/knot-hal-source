/*
 * Copyright (c) 2016, CESAR.
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license. See the LICENSE file for details.
 *
 */

#ifndef __STORAGE_H__
#define __STORAGE_H__

#ifdef __cplusplus
extern "C" {
#endif

// Identifier of data type to be stored
#define HAL_STORAGE_ID_UUID	0
#define HAL_STORAGE_ID_TOKEN	1
#define HAL_STORAGE_ID_MAC	2
#define HAL_STORAGE_ID_CONFIG	3

size_t hal_storage_read(void *addr, uint8_t *value, size_t len);
size_t hal_storage_write(void *addr, const uint8_t *value, size_t len);
size_t hal_storage_write_end(void *value, size_t len, uint8_t id);
size_t hal_storage_read_end(void *value, size_t len, uint8_t id);

#ifdef __cplusplus
}
#endif

#endif /* __STORAGE_H__ */
