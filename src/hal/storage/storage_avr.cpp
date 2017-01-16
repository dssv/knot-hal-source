/*
 * Copyright (c) 2016, CESAR.
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license. See the LICENSE file for details.
 *
 */
#include <avr/eeprom.h>
#include "include/avr_errno.h"
#include <string.h>
#include "include/storage.h"

#define UUID_SIZE		36
#define TOKEN_SIZE		40
#define MAC_SIZE		8
#define SCHEMA_FLAG_SIZE	1
#define PRIVATE_KEY_SIZE	32
#define PUBLIC_KEY_SIZE		64

#define CONFIG_SIZE 		sizeof(uint16_t)

// Address where each data is stored at the end of EEPROM
#define EEPROM_SIZE		(E2END + 1)
#define ADDR_UUID		(EEPROM_SIZE - UUID_SIZE)
#define ADDR_TOKEN		(ADDR_UUID - TOKEN_SIZE)
#define ADDR_MAC		(ADDR_TOKEN - MAC_SIZE)
#define ADDR_SCHEMA_FLAG	(ADDR_MAC - SCHEMA_FLAG_SIZE)
#define ADDR_PRIVATE_KEY	(ADDR_SCHEMA_FLAG - PRIVATE_KEY_SIZE)
#define ADDR_PUBLIC_KEY		(ADDR_PRIVATE_KEY - PUBLIC_KEY_SIZE)
#define ADDR_OFFSET_CONFIG	(ADDR_PUBLIC_KEY - CONFIG_SIZE)

#define EEPROM_SIZE_FREE	ADDR_OFFSET_CONFIG

ssize_t hal_storage_read(uint16_t addr, uint8_t *value, size_t len)
{
	uint8_t *offset = (uint8_t *) addr;
	union {
		uint8_t *offset;
		uint16_t size;
	} config;
	size_t i;

	/* Safe guard to avoid reading 'protected' EEPROM area: config/uuid/token */
	config.size = eeprom_read_word((const uint16_t*) ADDR_OFFSET_CONFIG);
	if (config.size > EEPROM_SIZE_FREE)
		return 0;

	/* Compute config address offset at eeprom */
	config.size = ADDR_OFFSET_CONFIG - config.size;

	for (i = 0; i < len && offset < config.offset; ++i, ++offset)
		value[i] = eeprom_read_byte((const uint8_t *) offset);

	return i;
}

ssize_t hal_storage_write(uint16_t addr, const uint8_t *value, size_t len)
{
	uint8_t *offset = (uint8_t *) addr;
	union {
		uint8_t *offset;
		uint16_t size;
	} config;
	size_t i;

	/* Safe guard to avoid writing 'protected' EEPROM area: config/uuid/token */
	config.size = eeprom_read_word((const uint16_t*) ADDR_OFFSET_CONFIG);
	if (config.size > EEPROM_SIZE_FREE)
		return 0;

	/* Compute config address offset at eeprom */
	config.size = ADDR_OFFSET_CONFIG - config.size;

	for (i = 0; i < len && offset < config.offset; ++i, ++offset)
		eeprom_write_byte(offset, value[i]);

	return i;
}

ssize_t hal_storage_write_end(uint8_t id, void *value, size_t len)
{
	/* Position where the data will be stored */
	uint16_t dst;
	/* Number of bytes written in the EEPROM with success */
	size_t i = 0;

	/* Calculate different addresses to store the
	 * value according to the parameter passed.
	 */
	switch (id) {
	case HAL_STORAGE_ID_UUID:
		dst = ADDR_UUID;
		break;

	case HAL_STORAGE_ID_TOKEN:
		dst = ADDR_TOKEN;
		break;

	case HAL_STORAGE_ID_MAC:
		dst = ADDR_MAC;
		break;

	case HAL_STORAGE_ID_SCHEMA_FLAG:
		dst = ADDR_SCHEMA_FLAG;
		break;

	case HAL_STORAGE_ID_PRIVATE_KEY:
		dst = ADDR_PRIVATE_KEY;
		break;

	case HAL_STORAGE_ID_PUBLIC_KEY:
		dst = ADDR_PUBLIC_KEY;
		break;

	case HAL_STORAGE_ID_CONFIG:
		/*
		 * Stores the size of the config,
		 * 2 bytes, to know where it end in the EEPROM.
		 */
		eeprom_write_word((uint16_t *) ADDR_OFFSET_CONFIG, len);

		/* Compute config address offset at eeprom */
		dst = ADDR_OFFSET_CONFIG - len;
		break;

	default:
		return -EINVAL;
	}

	/*Store all the block in the calculated position*/
	if (len != 0)
		for (i = 0; i < len ; ++i, ++dst)
			eeprom_write_block(((uint8_t*) value)+i, (void *) dst, 1);

	else if (len > EEPROM_SIZE)
		return -EINVAL;

	return i;
}

ssize_t hal_storage_read_end(uint8_t id, void *value, size_t len)
{
	/* Position where the data will be stored */
	uint16_t src;
	/* Number of bytes from EEPROM with success */
	size_t i = 0;

	/* Calculate different addresses to read the
	 * value according to the parameter passed.
	 */
	switch (id) {
	case HAL_STORAGE_ID_UUID:
		src = ADDR_UUID;
		break;

	case HAL_STORAGE_ID_TOKEN:
		src = ADDR_TOKEN;
		break;

	case HAL_STORAGE_ID_MAC:
		src = ADDR_MAC;
		break;

	case HAL_STORAGE_ID_SCHEMA_FLAG:
		src = ADDR_SCHEMA_FLAG;
		break;

	case HAL_STORAGE_ID_PRIVATE_KEY:
		src = ADDR_PRIVATE_KEY;
		break;

	case HAL_STORAGE_ID_PUBLIC_KEY:
		src = ADDR_PUBLIC_KEY;
		break;

	case HAL_STORAGE_ID_CONFIG:
		/*
		 * Read the size of the config,
		 * 2 bytes, to know where it end in the EEPROM.
		 */
		src = eeprom_read_word((const uint16_t*) ADDR_OFFSET_CONFIG);
		if (src > EEPROM_SIZE_FREE)
			return -EFAULT;

		if (src == 0)
			return 0;

		/* Compute config address offset at eeprom */
		src = ADDR_OFFSET_CONFIG - src;

		break;

	default:
		return -EINVAL;
	}

	/* Read all the block in the calculated position */
	if (len != 0)
		for (i = 0; i < len ; ++i, ++src)
			eeprom_read_block(((uint8_t*) value)+i, (void *) src, 1);

	else if (len > EEPROM_SIZE)
		return -EINVAL;

	return i;
}

void hal_storage_reset_end(void)
{
	union {
		uint8_t uuid[UUID_SIZE];
		uint8_t token[TOKEN_SIZE];
		uint8_t mac[MAC_SIZE];
		uint8_t schema_flag[SCHEMA_FLAG_SIZE];
		uint8_t private_key[PRIVATE_KEY_SIZE];
		uint8_t public_key[PUBLIC_KEY_SIZE];
	} data;

	memset(&data, 0, sizeof(data));

	hal_storage_write_end(HAL_STORAGE_ID_UUID, data.uuid, UUID_SIZE);
	hal_storage_write_end(HAL_STORAGE_ID_TOKEN, data.token, TOKEN_SIZE);
	hal_storage_write_end(HAL_STORAGE_ID_MAC, data.mac, MAC_SIZE);
	hal_storage_write_end(HAL_STORAGE_ID_SCHEMA_FLAG, data.schema_flag,
							SCHEMA_FLAG_SIZE);
	hal_storage_write_end(HAL_STORAGE_ID_PRIVATE_KEY, data.private_key,
							PRIVATE_KEY_SIZE);
	hal_storage_write_end(HAL_STORAGE_ID_PUBLIC_KEY, data.public_key,
							PUBLIC_KEY_SIZE);
	eeprom_write_word((uint16_t *) ADDR_OFFSET_CONFIG, 0);
}

uint16_t hal_config_get_size(void)
{

	return eeprom_read_word((const uint16_t*) ADDR_OFFSET_CONFIG);
}
