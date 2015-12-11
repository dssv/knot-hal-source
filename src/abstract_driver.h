/*
 * Copyright (c) 2015, CESAR.
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license. See the LICENSE file for details.
 *
 */
#include <stdint.h>
#include <stddef.h>

#ifndef __ABSTRACT_DRIVER_H__
#define __ABSTRACT_DRIVER_H__

// invalid socket fd
#define DRV_SOCKET_FD_INVALID		-1

// operation status codes
#define DRV_DONE				1
#define DRV_SUCCESS		0
#define DRV_ERROR			-1

#ifdef __cplusplus
extern "C"{
#endif

 /**
 * struct abstract_driver - driver abstraction for the physical layer
 * @name: driver name
 * @probe: function to initialize the driver
 * @remove: function to close the driver and free its resources
 * @socket: function to create a new socket FD
 * @listen: function to enable incoming connections
 * @accept: function that waits for a new connection and returns a 'pollable' FD
 * @connect: function to connect to a server socket
 *
 * This 'driver' intends to be an abstraction for Radio technologies or
 * proxy for other services using TCP or any socket based communication.
 */

typedef struct {
	const char *name;
	const uint8_t valid;

	int (*probe) (void);
	void (*remove) (void);

	int (*socket) (void);
	void (*close) (int sockfd);
	int (*listen) (int srv_sockfd);
	int (*accept) (int srv_sockfd);
	int (*connect) (int cli_sockfd, const void *addr, size_t len);

	int (*available) (int sockfd);
	size_t (*recv) (int sockfd, void *buffer, size_t len);
	size_t (*send) (int sockfd, const void *buffer, size_t len);
} abstract_driver_t;

extern abstract_driver_t abstract_driver;
extern abstract_driver_t nrf24l01_driver;

#ifdef __cplusplus
} // extern "C"
#endif

#endif // __ABSTRACT_DRIVER_H__
