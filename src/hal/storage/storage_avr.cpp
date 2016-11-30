/*
 * Copyright (c) 2016, CESAR.
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license. See the LICENSE file for details.
 *
 */
#include <avr/eeprom.h>
#include "storage.h"

// Address where each data is stored at the end of EEPROM
#define ADDR_UUID		41
#define ADDR_TOKEN		81
#define ADDR_MAC		91
#define ADDR_SIZE_CONFIG	93
#define ADDR_CONFIG		94

// Identifier of data type to be stored
#define DATA_UUID		0
#define DATA_TOKEN		1
#define DATA_MAC		2
#define DATA_CONFIG		3

int hal_storage_read(uint16_t addr, uint16_t *value, uint16_t len)
{
	uint16_t i;

	/* E2END represents the last EEPROM address */
	for (i = 0; i < len; i++) {
		if((addr + i) > (E2END + 1))
			break;

		value[i] = eeprom_read_word((const uint16_t*) addr + i);
	}

	return i;
}

int hal_storage_write(uint16_t addr, const uint16_t *value, uint16_t len)
{
	uint16_t i;

	/* E2END represents the last EEPROM address */
	for (i = 0; i < len; i++) {
		if ((addr + i) > (E2END + 1))
			break;

		eeprom_write_word((uint16_t*) addr + i, value[i]);
	}

	return i;
}

int hal_storage_write_end(void *value, int16_t len, uint8_t type)
{
	/* Position where the data will be stored */
	uint16_t dst;

	/* Calculate different addresses to store the
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
		dst = E2END - len - ADDR_CONFIG;
		/* Store the size of the config, 2 bytes, to know
		 * where it end in the EEPROM.
		 */
		hal_storage_write(E2END - ADDR_SIZE_CONFIG,
						(const uint16_t*)dst, 1);
		break;
	default:
		return -1;
	}

	/*Store all the block in the calculated position*/
	eeprom_write_block(value, (void*) dst, len);

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
