//
//  main.swift
//  Mac
//
//  Created by Myron on 2018/1/30.
//  Copyright © 2018年 Myron. All rights reserved.
//

import Foundation

print("Hello, World!")

var uint8s: [UInt8] = [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,]
cSocket_host_address(address: uint8s)




print(String(cString: uint8s))
