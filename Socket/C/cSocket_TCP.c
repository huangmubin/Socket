//
//  cSocket_TCP.c
//  Mac
//
//  Created by Myron on 2018/1/31.
//  Copyright © 2018年 Myron. All rights reserved.
//

#include "cSocket_TCP.h"

int cSocket_TCP_new_server(int port, int listen_count) {
    // Create Socket TCP
    int server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server == -1) {
        perror("TCP server socket create error");
        return ERROR_SOCKET;
    }
    
    // Create the address
    
    return server;
}
