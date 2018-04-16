//
//  cSocket_TCP.c
//  Mac
//
//  Created by Myron on 2018/1/31.
//  Copyright © 2018年 Myron. All rights reserved.
//

#include "cSocket_TCP.h"

/** */
int cSocket_TCP_new_server(int port, int listen_count) {
    // Create Socket TCP
    int server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server == -1) {
        perror("TCP server socket create error");
        return ERROR_SOCKET;
    }
    
    // Create the address
    struct sockaddr_in address;
    address.sin_len         = sizeof(struct sockaddr_in);
    address.sin_family      = AF_INET;
    address.sin_port        = htons(port);
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    bzero(&(address.sin_zero), 8);
    
    // Bind the address
    int result = bind(server, (struct sockaddr *)&address, sizeof(address));
    if (result == -1) {
        perror("TCP server socket bind error");
        return ERROR_BIND;
    }
    
    // Listen
    result = listen(server, listen_count);
    if (result == -1) {
        perror("TCP server socket listen error");
        return ERROR_LISTION;
    }
    
    // SO_NOSIGPIPE remove the iOS SIGPIPE error
    int set = 1;
    setsockopt(server, SOL_SOCKET, SO_NOSIGPIPE, &set, sizeof(int));
    return server;
}

/** */
int cSocket_TCP_new_client_ipv4(const char * address, int port, int sec, int usec) {
    // Create Client socket
    int client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (client == -1) {
        perror("TCP client socket create error");
        return ERROR_SOCKET;
    }
    
    // Create address
    struct sockaddr_in server_address;
    server_address.sin_len = sizeof(server_address);
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = inet_addr(address);
    bzero(&(server_address.sin_zero),8);
    
    // SO_NOSIGPIPE
    int set = 1;
    setsockopt(client, SOL_SOCKET, SO_NOSIGPIPE, &set, sizeof(int));
    
    // No time limite
    if (sec <= 0 && usec <= 0) {
        int result = connect(client, (struct sockaddr *)&server_address, sizeof(server_address));
        if (result == 0) {
            return client;
        } else {
            perror("TCP client socket connect error");
            return ERROR_CONNECT;
        }
    } else { // time limite
        cSocket_isBlock(client, BLOCK_OFF);
        int result = connect(client, (struct sockaddr *)&server_address, sizeof(server_address));
        // can't connect immediately
        if (result != 0) {
            fd_set client_fd_set;
            struct timeval time_value;
            time_value.tv_sec  = sec;
            time_value.tv_usec = usec;
            
            FD_ZERO(&client_fd_set);
            FD_SET(client, &client_fd_set);
            
            int ret = select(client + 1, NULL, &client_fd_set, NULL, &time_value);
            
            if (ret < 0) { // select error
                perror("TCP client socket select error");
                return ERROR_SELECT;
            } else if (ret == 0) { // time out
                perror("TCP client socket select timeout error");
                return ERROR_TIMEOUT;
            } else {
                int error = -1;
                int length = sizeof(int);
                getsockopt(client, SOL_SOCKET, SO_ERROR, &error, (socklen_t *)&length);
                if (error != 0) {
                    close(client);
                    perror("TCP client socket select connect error");
                    return ERROR_CONNECT;
                }
            }
        }
        cSocket_isBlock(client, BLOCK_ON);
        return client;
    }
}

/** address port is use to putout the client address and port */
int cSocket_TCP_accept(int server_socket, char * address, int * port, int sec, int usec) {
    if (sec > 0 || usec > 0) {
        fd_set server_fd_set;
        struct timeval time_value;
        time_value.tv_sec  = sec;
        time_value.tv_usec = usec;
        
        FD_ZERO(&server_fd_set);
        FD_SET(server_socket, &server_fd_set);
        
        int ret = select(server_socket + 1, &server_fd_set, NULL, NULL, &time_value);
        
        if (ret < 0) {
            perror("TCP socket accept select error");
            return ERROR_SELECT; // select 出错
        } else if(ret == 0){
            perror("TCP socket accept timeout error");
            return ERROR_TIMEOUT; // select 超时
        }
    }
    
    struct sockaddr_in client_address;
    socklen_t address_len;
    int accept_result = accept(server_socket, (struct sockaddr *)&client_address, &address_len);
    
    // Error
    if (accept_result < 3) {
        perror("TCP socket accept error");
        return ERROR_ACCEPT;
    } else {
        // SO_NOSIGPIPE
        int set = 1;
        setsockopt(accept_result, SOL_SOCKET, SO_NOSIGPIPE, &set, sizeof(int));
        printf("New client accept success %s:%d; \n",inet_ntoa(client_address.sin_addr),ntohs(client_address.sin_port));
        memcpy(address, inet_ntoa(client_address.sin_addr), strlen(inet_ntoa(client_address.sin_addr)));
        *port = client_address.sin_port;
        return accept_result;
    }
}

// MARK: - Write

size_t cSocket_TCP_write(int socket, const void * buffer, size_t size, int sec, int usec) {
    if (sec > 0 || usec > 0) {
        fd_set socket_fd_set;
        struct timeval tv;
        tv.tv_sec = sec;
        tv.tv_usec = usec;
        
        FD_ZERO(&socket_fd_set);
        FD_SET(socket, &socket_fd_set);
        int ret = select(socket + 1, NULL, &socket_fd_set, NULL, &tv);
        
        if (ret < 0) { // select error
            perror("TCP write select error");
            return ERROR_SELECT;
        } else if (ret == 0) { // select time out
            perror("TCP write timeout error");
            return ERROR_TIMEOUT;
        }
    }
    
    size_t w_size = write(socket, buffer, size);
    if (w_size != size) {
        perror("TCP write error");
        return ERROR_WRITE;
    } else {
        return w_size;
    }
}

// MARK: - Read

size_t cSocket_read(int socket, void * buffer, size_t size, int sec, int usec) {
    if (sec > 0 || usec > 0) {
        fd_set socket_fd_set;
        struct timeval tv;
        tv.tv_sec = sec;
        tv.tv_usec = usec;
        
        FD_ZERO(&socket_fd_set);
        FD_SET(socket, &socket_fd_set);
        int ret = select(socket + 1, &socket_fd_set, NULL, NULL, &tv);
        
        if (ret < 0) {
            perror("TCP read select error");
            return ERROR_SELECT;
        } else if (ret == 0) {
            perror("TCP read timeout error");
            return ERROR_TIMEOUT;
        }
    }
    size_t r_size = read(socket, buffer, size);
    if (r_size == -1) {
        perror("TCP read error");
        return ERROR_WRITE;
    } else {
        return r_size;
    }
}
