/*
 * Copyright (c) 2016, CESAR.
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license. See the LICENSE file for details.
 *
 */
#include <avr/eeprom.h>
#include "avr_errno.h"
#include "storage.h"

#define UUID_SIZE		36
#define TOKEN_SIZE		40
#define MAC_SIZE		8

#define CONFIG_SIZE 		sizeof(uint16_t)

// Address where each data is stored at the end of EEPROM
#define EEPROM_SIZE		(E2END + 1)
#define ADDR_UUID		(EEPROM_SIZE - UUID_SIZE)
#define ADDR_TOKEN		(ADDR_UUID - TOKEN_SIZE)
#define ADDR_MAC		(ADDR_TOKEN - MAC_SIZE)
#define ADDR_OFFSET_CONFIG	(ADDR_MAC - CONFIG_SIZE)

#define EEPROM_SIZE_FREE	ADDR_OFFSET_CONFIG

size_t hal_storage_read(void *addr, uint8_t *value, size_t len)
{
	size_t i;
	union {
		uint8_t *offset;
		uint16_t address;
	} config;

	uint8_t	 *offset = (uint8_t *)addr;

        config.address = eeprom_read_word((const uint16_t*) ADDR_OFFSET_CONFIG);

	/* E2END represents the last EEPROM address */
	for (i = 0; i < len && offset < config.offset; ++i, ++offset)
		*value++ = eeprom_read_word((const uint16_t*) offset);

	return i;
}

size_t hal_storage_write(void *addr, const uint8_t *value, size_t len)
{
	size_t i;
	union {
		uint8_t *offset;
		uint16_t address;
	} config;
	uint8_t	 *offset = (uint8_t *)addr;

        config.address = eeprom_read_word((const uint16_t*) ADDR_OFFSET_CONFIG);

	/* E2END represents the last EEPROM address */
	for (i = 0; i < len && offset < config.offset; ++i, ++offset)
		eeprom_write_word((uint16_t*) offset, *value++);

	return i;
}

size_t hal_storage_write_end(void *value, size_t len, uint8_t id)
{
	/* Position where the data will be stored */
	uint16_t dst;

	/* Calculate different addresses to store the
	 * value according to the parameter passed.
	 */
	switch (id) {
	case HAL_STORAGE_ID_UUID:
		if(len != UUID_SIZE)
			return -EINVAL;

		dst = ADDR_UUID;
		break;
	case HAL_STORAGE_ID_TOKEN:
		if(len != TOKEN_SIZE)
			return -EINVAL;

		dst = ADDR_TOKEN;
		break;
	case HAL_STORAGE_ID_MAC:
		if(len != MAC_SIZE)
			return -EINVAL;

		dst = ADDR_MAC;
		break;
	case HAL_STORAGE_ID_CONFIG:
		if(len > EEPROM_SIZE_FREE)
			return -EINVAL;

		dst = ADDR_OFFSET_CONFIG - len;
		/* Store the size of the config, 2 bytes, to know
		 * where it end in the EEPROM.
		 */
		eeprom_write_block(&dst, (void *)ADDR_OFFSET_CONFIG,
					CONFIG_SIZE);
		break;
	default:
		return -EINVAL;
	}

	/*Store all the block in the calculated position*/
	eeprom_write_block(value, (void *)dst, len);

	return len;
}

int hal_storage_read_end(void *value, int16_t len, uint8_t type)
{
	/* Position where the data will be stored */
	uint16_t dst;

	uint16_t *config_size = 0;

	/* Calculate different addresses to read the
	 * value according to the parameter passed.
	 */
	switch (type) {
	case DATA_UUID:
		dst = E2END - ADDR_UUID;
		break;
	case DATA_TOKEN:
		dst = E2END - ADDR_TOKEN;
		break;
	case DATA_MAC:
		dst = E2END - ADDR_MAC;
		break;
	case DATA_CONFIG:
		/* Read the size of the config, 2 bytes, to know
		 * where it end in the EEPROM.
		 */
		hal_storage_read(E2END - ADDR_SIZE_CONFIG, config_size, 1);
		dst = E2END - (uint16_t)config_size - ADDR_CONFIG;
		break;
	default:
		return -1;
	}

	/*Read all the block in the calculated position*/
	eeprom_read_block(value, (const void*) dst, len);

	return len;
}