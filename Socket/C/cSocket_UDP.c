//
//  cSocket_UDP.c
//  Mac
//
//  Created by Myron on 2018/2/1.
//  Copyright © 2018年 Myron. All rights reserved.
//

#include "cSocket_UDP.h"

/***/
int cSocket_UDP_new_server(int port) {
    // Create UDP
    int server_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (server_socket == -1) {
        perror("UDP create server error");
        return ERROR_SOCKET;
    }
    
    // Create Address
    struct sockaddr_in server_addr;
    server_addr.sin_len         = sizeof(struct sockaddr_in);
    server_addr.sin_family      = AF_INET;
    server_addr.sin_port        = htons(port);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    bzero(&(server_addr.sin_zero), 8);
    
    // Address reuse
    int set_opt = 1;
    int opt_result = setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &set_opt, sizeof(set_opt));
    if (opt_result != 0) {
        perror("UDP server reuse address error");
        return ERROR_REUSEADDR;
    }
    
    // Bind
    int bind_result = bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (bind_result == -1) {
        perror("UDP server bind address error");
        return ERROR_BIND;
    }
    
    return server_socket;
}

/**  */
int cSocket_UDP_client() {
    // Create socket
    int client_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (client_socket == -1) {
        perror("UDP client create error");
        return ERROR_SOCKET;
    }
    
    // Address reuse
    int set_opt = 1;
    int opt_result = setsockopt(client_socket, SOL_SOCKET, SO_REUSEADDR, &set_opt, sizeof(set_opt));
    if (opt_result != 0) {
        perror("UDP client bind address error");
        return ERROR_REUSEADDR;
    }
    
    return client_socket;
}


/** broadcast open: 1 close: 0 */
void cSocket_UDP_opt(int socket, int open) {
    setsockopt(socket, SOL_SOCKET, SO_BROADCAST, &open, sizeof(open));
}

// MARK: - Send

size_t cSocket_sendto(int socket, const void * buffer, size_t size, const void * address, int port, int sec, int usec) {
    if (sec > 0 || usec > 0) {
        fd_set socket_fd_set;
        struct timeval tv;
        tv.tv_sec = sec;
        tv.tv_usec = usec;
        
        FD_ZERO(&socket_fd_set);
        FD_SET(socket, &socket_fd_set);
        int ret = select(socket + 1, NULL, &socket_fd_set, NULL, &tv);
        
        if (ret < 0) {
            printf("UDP sendto select error: %s; address: %s; port: %d; sec = %d, usec = %d;\n", buffer, address, port, sec, usec);
            perror("UDP sendto select error");
            return ERROR_SELECT;
        } else if (ret == 0) {
            printf("UDP sendto timeout error: %s; address: %s; port: %d; sec = %d, usec = %d;\n", buffer, address, port, sec, usec);
            perror("UDP sendto timeout error");
            return ERROR_TIMEOUT;
        }
    }
    
    struct sockaddr_in to_address;
    to_address.sin_len         = sizeof(struct sockaddr_in);
    to_address.sin_family      = AF_INET;
    to_address.sin_port        = htons(port);
    to_address.sin_addr.s_addr = inet_addr(address);
    bzero(&(to_address.sin_zero), 8);
    
    size_t w_size = sendto(socket, buffer, size, 0, (struct sockaddr *)&to_address, sizeof(to_address));
    if (w_size != size) {
        printf("UDP sendto error: %s; address: %s; port: %d; sec = %d, usec = %d;\n", buffer, address, port, sec, usec);
        perror("UDP sendto write error");
        return ERROR_WRITE;
    } else {
        return w_size;
    }
}

// MARK: - Recv

size_t cSocket_recvfrom(int socket, void * buffer, size_t size, char * fromAddress, int * fromPort, int sec, int usec) {
    if (sec > 0 || usec > 0) {
        fd_set socket_fd_set;
        struct timeval tv;
        tv.tv_sec = sec;
        tv.tv_usec = usec;
        
        FD_ZERO(&socket_fd_set);
        FD_SET(socket, &socket_fd_set);
        int ret = select(socket + 1, &socket_fd_set, NULL, NULL, &tv);
        
        if (ret < 0) {
            perror("UDP recvfrom select error");
            return ERROR_SELECT; // select 出错
        } else if (ret == 0) {
            perror("UDP recvfrom timeout error");
            return ERROR_TIMEOUT; // select 超时
        }
    }
    
    struct sockaddr_in address;
    socklen_t port = sizeof(struct sockaddr);
    size_t r_size = recvfrom(socket, buffer, size, 0, (struct sockaddr *)&address, &port);
    if (r_size == -1) {
        perror("UDP recvfrom error");
        return ERROR_WRITE;
    } else {
        memcpy(fromAddress, inet_ntoa(address.sin_addr), strlen(inet_ntoa(address.sin_addr)));
        *fromPort = port;
        return r_size;
    }
}

