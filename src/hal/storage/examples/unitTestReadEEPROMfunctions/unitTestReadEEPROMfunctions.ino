/*
 * Copyright (c) 2016, CESAR.
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license. See the LICENSE file for details.
 *
 */

#include "KNoTThing.h"
#include "knot_protocol.h"
#include "include/storage.h"
#include "include/printf_serial.h"

#include <string.h>
#include <EEPROM.h>

#define UUID_SIZE		36
#define TOKEN_SIZE		40
#define MAC_SIZE		8

#define CONFIG_SIZE 		sizeof(uint16_t)

#define EEPROM_SIZE		(E2END + 1)
#define ADDR_UUID		(EEPROM_SIZE - UUID_SIZE)
#define ADDR_TOKEN		(ADDR_UUID - TOKEN_SIZE)
#define ADDR_MAC		(ADDR_TOKEN - MAC_SIZE)
#define ADDR_OFFSET_CONFIG	(ADDR_MAC - CONFIG_SIZE)

/* Sample values */
static char value_UUID[UUID_SIZE] = "361ff48d-c534-4ac6-a7b7-70e648a8000";
static char value_UUID_out[UUID_SIZE+1];
static char value_TOKEN[TOKEN_SIZE] = "ad798840028f9055e061256f3d59a150ddee045";
static char value_TOKEN_out[TOKEN_SIZE+1];
static uint64_t mac = 0x1122334455667788;
static uint8_t value_PRIVATE_KEY[PRIVATE_KEY_SIZE] = {
	0xA8, 0x78, 0x51, 0x3A, 0x5D, 0xDA, 0x9C, 0x33, 0x35, 0xB4, 0x05, 0x06, 0xE1,
	 0x1E, 0x88, 0xD9, 0x52, 0xBF, 0x4F, 0x98, 0xF3, 0xB3, 0x0E, 0xB8, 0x64, 0x35,
	  0x0F, 0x9D, 0xB7, 0x35, 0x0C, 0xAE
};
static uint8_t value_PRIVATE_KEY_out[PRIVATE_KEY_SIZE + 1];
static uint8_t value_PUBLIC_KEY[PUBLIC_KEY_SIZE] = {
	0x1F, 0x37, 0x66, 0xC9, 0x8A, 0xDB, 0x2D, 0x0C, 0xAF, 0x8C, 0x02, 0xE4, 0x47,
	 0x05, 0x9C, 0xB0, 0xFE, 0x6F, 0x1A, 0x40, 0x3D, 0x57, 0x70, 0x0D, 0x9B, 0x3F,
	  0x34, 0xCD, 0xD4, 0xCF, 0xDC, 0x8C, 0x36, 0x6D, 0x48, 0x58, 0xAD, 0x64, 0xE5, 0xE4, 0xAE, 0x9A, 0xC1, 0x49, 0xEF, 0xD9, 0x94, 0x75,
  0xF8, 0x24, 0x7B, 0x9A, 0xCF, 0x18, 0x53, 0xF9, 0x41, 0xB0, 0xC5, 0x5A, 0x86, 0xB9, 0x60, 0xF4
};
static uint8_t value_PUBLIC_KEY_out[PUBLIC_KEY_SIZE + 1];

static union {
	uint64_t dw;
	struct {
		uint32_t low,
		high;
	} w;
} mac_out;

typedef struct {
	uint8_t		sensor_id;
	knot_config	values;
} data_config_store;

static data_config_store conf[2];
static data_config_store conf_out[2];

static char str[50], *pstr;

static uint16_t address = 0, index = 0;
static byte value;

static uint16_t	config_offset = 0;

void setup()
{
	printf_serial_init();
	printf("Unit Test Read EEPROM Functions\n\n");

	conf[0].sensor_id = 30;
	conf[0].values.event_flags = KNOT_EVT_FLAG_CHANGE;
	conf[0].values.time_sec = 25;
	conf[0].values.lower_limit.val_i.value = 5;
	conf[0].values.upper_limit.val_i.value = 32;

	conf[1].sensor_id = 99;
	conf[1].values.event_flags = KNOT_EVT_FLAG_TIME;
	conf[1].values.time_sec = 11;
	conf[1].values.lower_limit.val_i.value = 7;
	conf[1].values.upper_limit.val_i.value = 17;

	/* White Functions (natives) */
	eeprom_write_block(&value_UUID, (void *) ADDR_UUID, sizeof(value_UUID));

	eeprom_write_block(&value_TOKEN, (void *) ADDR_TOKEN, sizeof(value_TOKEN));

	eeprom_write_block(&mac, (void *) ADDR_MAC, sizeof(mac));

	eeprom_write_block(&value_PRIVATE_KEY, (void *) ADDR_PRIVATE_KEY,
	 sizeof(value_PRIVATE_KEY));

	eeprom_write_block(&value_PUBLIC_KEY, (void *) ADDR_PUBLIC_KEY,
 	 sizeof(value_PUBLIC_KEY));

	eeprom_write_word((uint16_t *) ADDR_OFFSET_CONFIG, sizeof(conf));
	eeprom_write_block(conf, (void *) (ADDR_OFFSET_CONFIG - sizeof(conf)), sizeof(conf));

	/* Read Functions */
	hal_storage_read_end(HAL_STORAGE_ID_UUID, value_UUID_out, sizeof(value_UUID));
	value_UUID_out[sizeof(value_UUID)] = '\0';

	hal_storage_read_end(HAL_STORAGE_ID_TOKEN, value_TOKEN_out, sizeof(value_TOKEN));
	value_TOKEN_out[sizeof(value_TOKEN)] = '\0';

	hal_storage_read_end(HAL_STORAGE_ID_MAC, &mac_out, sizeof(mac));

	hal_storage_read_end(HAL_STORAGE_ID_PRIVATE_KEY, (void *) value_PRIVATE_KEY_out,
		sizeof(value_PRIVATE_KEY));
   value_PRIVATE_KEY_out[sizeof(value_PRIVATE_KEY)] = '\0';

	 hal_storage_read_end(HAL_STORAGE_ID_PUBLIC_KEY, (void *) value_PUBLIC_KEY_out,
 		sizeof(value_PUBLIC_KEY));
    value_PUBLIC_KEY_out[sizeof(value_PUBLIC_KEY)] = '\0';

	hal_storage_read_end(HAL_STORAGE_ID_CONFIG, conf_out, sizeof(conf_out));

	/* Comparisons */
	/* UUID */
	if((memcmp(value_UUID, value_UUID_out, sizeof(value_UUID))) == 0)
		printf("UUID: \t\tOk\n");
	else
		printf("Problems reading UUID\n");
	printf("UUID:\t\t'%s'\n", value_UUID);
	printf("UUID read:\t'%s'\n\n", value_UUID_out);

	/* TOKEN */
	if((memcmp(value_TOKEN, value_TOKEN_out, sizeof(value_TOKEN))) == 0)
		printf("TOKEN: \t\tOk\n");
	else
		printf("Problems reading TOKEN\n");
	value_TOKEN[sizeof(value_TOKEN)] = '\0';
	printf("TOKEN:\t\t'%s'\n", value_TOKEN);
	printf("TOKEN read:\t'%s'\n\n", value_TOKEN_out);

	/* MAC */
	if (mac == mac_out.dw)
		printf("MAC: \t\tOk\n");
	else
		printf("Problems reading MAC\n");
	printf("MAC:\t\t%#lx", mac>>32);
	printf("%lx\n", mac);
	printf("MAC read:\t%#lx%lx\n\n", mac_out.w.high, mac_out.w.low);

	/* PRIVATE KEY */
	if ((memcmp(value_PRIVATE_KEY, value_PRIVATE_KEY_out, sizeof(value_PRIVATE_KEY))) == 0)
		printf("PRIVATE KEY: \tOk\n");
	else
		printf("Problems reading PRIVATE KEY\n");
	printf("PRIVATE KEY: \t\t'%s'\n", value_PRIVATE_KEY);
	printf("PRIVATE KEY read:\t'%s'\n\n", value_PRIVATE_KEY_out);

	/* PUBLIC KEY */
	if ((memcmp(value_PUBLIC_KEY, value_PUBLIC_KEY_out, sizeof(value_PUBLIC_KEY))) == 0)
		printf("PUBLIC KEY: \tOk\n");
	else
		printf("Problems reading PUBLIC KEY\n");
	printf("PUBLIC KEY: \t\t'%s'\n", value_PUBLIC_KEY);
	printf("PUBLIC KEY read:\t'%s'\n\n", value_PUBLIC_KEY_out);

	/* Config */
	if((memcmp(conf, conf_out, sizeof(conf))) == 0)
		printf("Config: \tOk\n");
	else
		printf("Problems reading Config\n");
	printf("Config0:\tID0=%d\tflag0=%d\tTime0=%d\tLower limit0=%d\tUpper limit0=%d\n", conf[0].sensor_id, conf[0].values.event_flags, conf[0].values.time_sec, conf[0].values.lower_limit.val_i.value, conf[0].values.upper_limit.val_i.value, 10);
	printf("Config out0:\tID0=%d\tflag0=%d\tTime0=%d\tLower limit0=%d\tUpper limit0=%d\n", conf_out[0].sensor_id, conf_out[0].values.event_flags, conf_out[0].values.time_sec, conf_out[0].values.lower_limit.val_i.value, conf_out[0].values.upper_limit.val_i.value, 10);

	printf("Config1:\tID1=%d\tflag1=%d\tTime1=%d\tLower limit1=%d\tUpper limit1=%d\n", conf[1].sensor_id, conf[1].values.event_flags, conf[1].values.time_sec, conf[1].values.lower_limit.val_i.value, conf[1].values.upper_limit.val_i.value);
	printf("Config out1:\tID1=%d\tflag1=%d\tTime1=%d\tLower limit1=%d\tUpper limit1=%d\n\n", conf_out[1].sensor_id, conf_out[1].values.event_flags, conf_out[1].values.time_sec, conf_out[1].values.lower_limit.val_i.value, conf_out[1].values.upper_limit.val_i.value);

	printf("EEPROM(%d) Dumping...\n\n", EEPROM.length());
}

void loop()
{
	/* Dump EEPROM */
	value = EEPROM.read(address);

	if ((address % 32) == 0) {
		if (address != 0) {
			printf(" - ");
			for(pstr=str; index != 0; --index, ++pstr)
				printf("%c", *pstr < ' ' || *pstr > 0x7f ? '.' : *pstr);
			printf("\n");
		}

		if (address == EEPROM.length()) {
			printf("[END]\n");
			while(true);
		}

		printf("[%04d]:", address);
		index = 0;
	}

	printf(" %02X", value);
	str[index++] = value;

	address++;
}
