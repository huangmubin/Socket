//
//  cSocket.h
//  Mac
//
//  Created by Myron on 2018/1/31.
//  Copyright © 2018年 Myron. All rights reserved.
//

#ifndef cSocket_h
#define cSocket_h

// MARK: - Include

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <ifaddrs.h>
#include <arpa/inet.h>

//#include <errno.h>
//#include <netinet/in.h>
//#include <sys/socket.h>
//#include <sys/time.h>
//#include <arpa/inet.h>
//#include <stdlib.h>

// MARK: - Define

// MARK: Error

#define ERROR_SOCKET        -1
#define ERROR_BIND          -2
#define ERROR_LISTION       -3
#define ERROR_ACCEPT        -4
#define ERROR_CONNECT       -5
#define ERROR_REUSEADDR     -6

#define ERROR_READ          -7
#define ERROR_WRITE         -8

#define ERROR_SELECT        -9
#define ERROR_TIMEOUT       -10

// MARK: Block

#define BLOCK_ON    1
#define BLOCK_OFF   0

// MARK: Close

#define SHUTDOWN_READ       0
#define SHUTDOWN_WRITE      1
#define SHUTDOWN_READ_WRITE 2

// MARK: - Interface

// MARK: - Tools

/** Get the input form Control */
void cSocket_input(void * input, int size);
/** Get the ipv4 address  */
void cSocket_host_address(void *address);

#endif /* cSocket_h */
