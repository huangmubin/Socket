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

#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <stdlib.h>

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

// MARK: - Timer

/** 0 is no block */
void cSocket_isBlock(int socket, int on);

// MAKR: - Close

/**
 success: 0, error: -1
 错误码errno：EBADF表示fd不是一个有效描述符；EINTR表示close函数被信号中断；EIO表示一个IO错误。
 close(sockfd);使用close中止一个连接，但它只是减少描述符的参考数，并不直接关闭连接，只有当描述符的参考数为0时才关闭连接。所以在多进程/线程程序中，close只是确保了对于某个特定的进程或线程来说，该连接是关闭的。使用 client_fd = accept() 后 fork() 以在子进程中处理请求，此时在父进程中使用 close() 关闭该连接，子进程仍可以继续使用该连接。
 */
int cSocket_close(int socket);

/**
 success: 0, error: -1
 how的方式有三种分别是：
 　　　　SHUT_RD（0）：关闭sockfd上的读功能，此选项将不允许sockfd进行读操作。即该套接字不再接受数据，任何当前在套接字接受缓冲区的数据将被丢弃。进程将不能对该套接字发出任何读操作。对TCP套接字该调用之后接受到的任何数据将被确认然后无声的丢弃掉。
 　　　　SHUT_WR（1）：关闭sockfd的写功能，此选项将不允许sockfd进行写操作，即进程不能在对此套接字发出写操作。
 　　　　SHUT_RDWR（2）：关闭sockfd的读写功能，相当于调用shutdown两次：首先是以SHUT_RD,然后以SHUT_WR。
 
 　　成功则返回0，错误返回-1，错误码errno：EBADF表示sockfd不是一个有效描述符；ENOTCONN表示sockfd未连接；ENOTSOCK表示sockfd是一个文件描述符而不是socket描述符。
 
 　　shutdown()的效果是累计的，不可逆转的。既如果关闭了一个方向数据传输，那么这个方向将会被关闭直至完全被关闭或删除，而不能重新被打开。如果第一次调用了shutdown(0)，第二次调用了shutdown(1)，那么这时的效果就相当于shutdown(2)，也就是双向关闭socket。
 */
int cSocket_shutdown(int socket, int howto);


#endif /* cSocket_h */
