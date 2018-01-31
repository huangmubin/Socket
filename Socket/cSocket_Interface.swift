//
//  cSocket_Interface.swift
//  Mac
//
//  Created by Myron on 2018/1/31.
//  Copyright © 2018年 Myron. All rights reserved.
//

import Foundation

 @_silgen_name("cSocket_input") func cSocket_input(input: UnsafePointer<UInt8>, length: Int32) -> Void
 @_silgen_name("cSocket_host_address") func cSocket_host_address(address: UnsafePointer<UInt8>) -> Void
