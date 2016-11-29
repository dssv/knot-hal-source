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
#define ADDR_UUID		0
#define ADDR_TOKEN		41
#define ADDR_MAC		81
#define ADDR_SIZE_CONFIG	92
#define ADDR_CONFIG		93

// Identifier of data type to be stored
#define DATA_UUID		0
#define DATA_TOKEN		1
#define DATA_MAC		2
#define DATA_CONFIG		3

int hal_storage_read(uint16_t addr, uint8_t *value, uint16_t len)
{
	uint8_t i;

	/* E2END represents the last EEPROM address */
	for (i = 0; i < len; i++) {
		if((addr + i) > (E2END + 1))
			break;

		value[i] = eeprom_read_byte((const uint8_t*) addr + i);
	}

	return i;
}

int hal_storage_write(uint16_t addr, const uint8_t *value, uint16_t len)
{
	uint8_t i;

	/* E2END represents the last EEPROM address */
	for (i = 0; i < len; i++) {
		if ((addr + i) > (E2END + 1))
			break;

		eeprom_write_byte((uint8_t*) addr + i, value[i]);
	}

	return i;
}

int hal_storage_write_end(void *value, int8_t len, uint8_t data)
{
	/* Position where the data will be stored */
	uint16_t dst;

	/* Pointer to the size of data, used as parameter */
	const uint8_t *lenght = (const uint8_t*) &len;

	/* Calculate different addresses to store the
	 * value according to the parameter passed.
	 */
	switch (data) {
	case DATA_UUID:
		dst = E2END - len - ADDR_UUID;
		break;
	case DATA_TOKEN:
		dst = E2END - len - ADDR_TOKEN;
		break;
	case DATA_MAC:
		dst = E2END - len - ADDR_MAC;
		break;
	case DATA_CONFIG:
		dst = E2END - len - ADDR_CONFIG;
		/* Store the size of the config, 2 bytes, to know
		 * where it end in the EEPROM.
		 */
		hal_storage_write(E2END - ADDR_SIZE_CONFIG, lenght, 2);
		break;
	default:
		return -1;
	}

	/*Store all the block in the calculated position*/
	eeprom_write_block(value, (void*) dst, len);

	return len;
}

int hal_storage_read_end(void *value, int8_t len, uint8_t data)
{
	/* Position where the data will be stored */
	uint16_t dst;

	/* Pointer to the size of data, used as parameter */
	uint8_t *lenght = (uint8_t*) &len;

	/* Calculate different addresses to read the
	 * value according to the parameter passed.
	 */
	switch (data) {
	case DATA_UUID:
		dst = E2END - len - ADDR_UUID;
		break;
	case DATA_TOKEN:
		dst = E2END - len - ADDR_TOKEN;
		break;
	case DATA_MAC:
		dst = E2END - len - ADDR_MAC;
		break;
	case DATA_CONFIG:
		/* Read the size of the config, 2 bytes, to know
		 * where it end in the EEPROM.
		 */
		hal_storage_read(E2END - ADDR_SIZE_CONFIG, lenght, 2);
		dst = E2END - len - ADDR_CONFIG;
		break;
	default:
		return -1;
	}

	/*Read all the block in the calculated position*/
	eeprom_read_block(value, (const void*) dst, len);

	return len;
}
