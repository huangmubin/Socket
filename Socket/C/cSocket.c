//
//  cSocket.c
//  Mac
//
//  Created by Myron on 2018/1/31.
//  Copyright © 2018年 Myron. All rights reserved.
//

#include "cSocket.h"

// MARK: - Tools

/** Get the input form Control */
void cSocket_input(void * input, int size) {
    bzero(input, size);
    fgets(input, size, stdin);
}

/** Get the ipv4 address size is 46 */
void cSocket_host_address(void *address) {
    bzero(address, INET6_ADDRSTRLEN);
    
    struct ifaddrs *interfaces = NULL;
    void *temp_addr = NULL;
    
    // retrieve the current interfaces - returns 0 on success
    if (getifaddrs(&interfaces) == 0) {
        while (interfaces != NULL) {
            // en0 is the local address
            if (strcmp(interfaces->ifa_name, "en0") == 0) {
                // iPv4
                if (interfaces->ifa_addr->sa_family == AF_INET) {
                    temp_addr = &((struct sockaddr_in *)interfaces->ifa_addr)->sin_addr;
                    inet_ntop(AF_INET, temp_addr, address, INET_ADDRSTRLEN);
                    //printf("cSocket get the host address is iPv4 %s - %s;\n", interfaces->ifa_name, address);
                }
                // iPv6
                else if (interfaces->ifa_addr->sa_family == AF_INET6) {
                    temp_addr = &((struct sockaddr_in *)interfaces->ifa_addr)->sin_addr;
                    inet_ntop(AF_INET6, temp_addr, address, INET6_ADDRSTRLEN);
                    //printf("cSocket get the host address is iPv4 %s - %s;\n", interfaces->ifa_name, address);
                }
            }
            // next
            interfaces = interfaces->ifa_next;
        }
    }
    
    freeifaddrs(interfaces);
}

// MARK: - Timer

/** 0 is no block */
void cSocket_isBlock(int socket, int on) {
    int flags = fcntl(socket, F_GETFL, 0);
    if (on == 0) {
        fcntl(socket, F_SETFL, flags | O_NONBLOCK);
    } else {
        flags &= ~ O_NONBLOCK;
        fcntl(socket, F_SETFL, flags);
    }
}

// MAKR: - Close

/**
 success: 0, error: -1
 错误码errno：EBADF表示fd不是一个有效描述符；EINTR表示close函数被信号中断；EIO表示一个IO错误。
 close(sockfd);使用close中止一个连接，但它只是减少描述符的参考数，并不直接关闭连接，只有当描述符的参考数为0时才关闭连接。所以在多进程/线程程序中，close只是确保了对于某个特定的进程或线程来说，该连接是关闭的。使用 client_fd = accept() 后 fork() 以在子进程中处理请求，此时在父进程中使用 close() 关闭该连接，子进程仍可以继续使用该连接。
 */
int cSocket_close(int socket) {
    return close(socket);
}

/**
 success: 0, error: -1
 how的方式有三种分别是：
 　　　　SHUT_RD（0）：关闭sockfd上的读功能，此选项将不允许sockfd进行读操作。即该套接字不再接受数据，任何当前在套接字接受缓冲区的数据将被丢弃。进程将不能对该套接字发出任何读操作。对TCP套接字该调用之后接受到的任何数据将被确认然后无声的丢弃掉。
 　　　　SHUT_WR（1）：关闭sockfd的写功能，此选项将不允许sockfd进行写操作，即进程不能在对此套接字发出写操作。
 　　　　SHUT_RDWR（2）：关闭sockfd的读写功能，相当于调用shutdown两次：首先是以SHUT_RD,然后以SHUT_WR。
 
 　　成功则返回0，错误返回-1，错误码errno：EBADF表示sockfd不是一个有效描述符；ENOTCONN表示sockfd未连接；ENOTSOCK表示sockfd是一个文件描述符而不是socket描述符。
 
 　　shutdown()的效果是累计的，不可逆转的。既如果关闭了一个方向数据传输，那么这个方向将会被关闭直至完全被关闭或删除，而不能重新被打开。如果第一次调用了shutdown(0)，第二次调用了shutdown(1)，那么这时的效果就相当于shutdown(2)，也就是双向关闭socket。
 */
int cSocket_shutdown(int socket, int howto) {
    return shutdown(socket, howto);
}

// MARK: - Send

size_t cSocket_send(int socket, const void * buffer, size_t size, int sec, int usec, int flags) {
    if (sec > 0 || usec > 0) {
        fd_set socket_fd_set;
        struct timeval tv;
        tv.tv_sec = sec;
        tv.tv_usec = usec;
        
        FD_ZERO(&socket_fd_set);
        FD_SET(socket, &socket_fd_set);
        int ret = select(socket + 1, NULL, &socket_fd_set, NULL, &tv);
        
        if (ret < 0) {
            printf("send select error: %s; sec = %d, usec = %d;\n", buffer, sec, usec);
            perror("send select error");
            return ERROR_SELECT; // select 出错
        } else if (ret == 0) {
            printf("send timeout error: %s; sec = %d, usec = %d;\n", buffer, sec, usec);
            perror("send timeout error");
            return ERROR_TIMEOUT; // select 超时
        }
    }
    
    size_t w_size = send(socket, buffer, size, flags);
    if (w_size != size) {
        printf("send error: %s; sec = %d, usec = %d;\n", buffer, sec, usec);
        perror("send error");
        return ERROR_WRITE;
    } else {
        return w_size;
    }
}

// MARK: - Recv

size_t cSocket_recv(int socket, void * buffer, size_t size, int sec, int usec, int flags) {
    if (sec > 0 || usec > 0) {
        fd_set socket_fd_set;
        struct timeval tv;
        tv.tv_sec = sec;
        tv.tv_usec = usec;
        
        FD_ZERO(&socket_fd_set);
        FD_SET(socket, &socket_fd_set);
        int ret = select(socket + 1, &socket_fd_set, NULL, NULL, &tv);
        
        if (ret < 0) {
            perror("recv select error");
            return ERROR_SELECT; // select 出错
        } else if (ret == 0) {
            perror("recv timeout error");
            return ERROR_TIMEOUT; // select 超时
        }
    }
    
    size_t r_size = recv(socket, buffer, size, flags);
    if (r_size == -1) {
        perror("recv error");
        return ERROR_WRITE;
    } else {
        return r_size;
    }
}
