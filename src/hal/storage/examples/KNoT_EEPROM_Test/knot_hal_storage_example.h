/*
 * Copyright (c) 2017, CESAR.
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license. See the LICENSE file for details.
 *
 */

#ifndef __KNOT_HAL_STORAGE_EXAMPLE_H__
#define __KNOT_HAL_STORAGE_EXAMPLE_H__

#define UUID_SIZE		36
#define TOKEN_SIZE		40
#define MAC_SIZE		8
#define SCHEMA_FLAG_SIZE	1
#define PRIVATE_KEY_SIZE	32
#define PUBLIC_KEY_SIZE	64
#define FOREIGN_KEY_SIZE	64
#define TYPE_PRIVATE_KEY	0
#define TYPE_PRIVATE_KEY_OUT	1
#define TYPE_PUBLIC_KEY	2
#define TYPE_PUBLIC_KEY_OUT	3
#define TYPE_FOREIGN_KEY	4
#define TYPE_FOREIGN_KEY_OUT	5

#define CONFIG_SIZE 		sizeof(uint16_t)

#define EEPROM_SIZE		(E2END + 1)
#define ADDR_UUID		(EEPROM_SIZE - UUID_SIZE)
#define ADDR_TOKEN		(ADDR_UUID - TOKEN_SIZE)
#define ADDR_MAC		(ADDR_TOKEN - MAC_SIZE)
#define ADDR_SCHEMA_FLAG	(ADDR_MAC - SCHEMA_FLAG_SIZE)
#define ADDR_PRIVATE_KEY	(ADDR_SCHEMA_FLAG - PRIVATE_KEY_SIZE)
#define ADDR_PUBLIC_KEY	(ADDR_PRIVATE_KEY - PUBLIC_KEY_SIZE)
#define ADDR_FOREIGN_KEY	(ADDR_PUBLIC_KEY - FOREIGN_KEY_SIZE)
#define ADDR_OFFSET_CONFIG	(ADDR_FOREIGN_KEY - CONFIG_SIZE)

#endif /* __KNOT_HAL_STORAGE_EXAMPLE_H__ */
