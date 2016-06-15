////
//  ██████╗  ██████╗ ██████╗  ██████╗ ██████╗ ███████╗███████╗ ██████╗██╗   ██╗███████╗
//  ██╔══██╗██╔═══██╗██╔══██╗██╔═══██╗██╔══██╗██╔════╝██╔════╝██╔════╝██║   ██║██╔════╝
//  ██████╔╝██║   ██║██████╔╝██║   ██║██████╔╝█████╗  ███████╗██║     ██║   ██║█████╗  
//  ██╔══██╗██║   ██║██╔══██╗██║   ██║██╔══██╗██╔══╝  ╚════██║██║     ██║   ██║██╔══╝  
//  ██║  ██║╚██████╔╝██████╔╝╚██████╔╝██║  ██║███████╗███████║╚██████╗╚██████╔╝███████╗
//  ╚═╝  ╚═╝ ╚═════╝ ╚═════╝  ╚═════╝ ╚═╝  ╚═╝╚══════╝╚══════╝ ╚═════╝ ╚═════╝ ╚══════╝
//                                                                                                                                          
//
// @file Commands.h
// @date Created: 15-06-2016
// @version 1.0
//
// @author Bouke Stam, Scott Mackay
//
// @section LICENSE
// License: newBSD
//
// Copyright © 2016, HU University of Applied Sciences Utrecht.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
// - Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
// - Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
// - Neither the name of the HU University of Applied Sciences Utrecht nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE HU UNIVERSITY OF APPLIED SCIENCES UTRECHT
// BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
// GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
// LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
// OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
////

#ifndef qik_commands
#define qik_commands

//Motor control via Qik motor driver
static const unsigned char command_sync         = 0xAA;   //Synchronisation
static const unsigned char command_version      = 0x81;   //Firmware Version
static const unsigned char command_error        = 0x82;   //Error

static const unsigned char command_forward_M0   = 0x88;   //Motor M0 Forward 
static const unsigned char command_reverse_M0   = 0x8A;   //Motor M0 reverse 
static const unsigned char command_brake_M0     = 0x86;   //Motor M0 Brake

static const unsigned char command_forward_M1   = 0x8C;   //Motor M1 Forward 
static const unsigned char command_reverse_M1   = 0x8E;   //Motor M1 reverse
static const unsigned char command_brake_M1     = 0x87;   //Motor M1 Brake

//Rosbee control using Uart
static const unsigned char uart_forward         = 0x30;   //Rosbee Forward 
static const unsigned char uart_backward        = 0x31;   //Rosbee Backward

static const unsigned char uart_rotate_left     = 0x32;   //Rosbee rotate left
static const unsigned char uart_rotate_right    = 0x33;   //Rosbee rotate right

static const unsigned char uart_stop            = 0x34;   //Rosbee stop

static const unsigned char uart_set_speed       = 0x35;   //Rosbee Set Speed

static const unsigned char uart_get_speed0      = 0x36;   //Rosbee Get Speed of left wheel
static const unsigned char uart_get_speed1      = 0x37;   //Rosbee Get Speed of right wheel

static const unsigned char uart_get_distance0   = 0x38;   //Rosbee Get Distance of left wheel
static const unsigned char uart_get_distance1   = 0x39;   //Rosbee Get Distance of right wheel

static const unsigned char uart_version         = 0x3B;   //Firmware Version

static const unsigned char uart_ok              = 0x3A;   //reply ok

#endif