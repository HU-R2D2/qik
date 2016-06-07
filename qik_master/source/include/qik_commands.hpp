//
// Created by martijn on 6/6/16.
//

#ifndef ADT_QIK_COMMANDS_HPP
#define ADT_QIK_COMMANDS_HPP

//Rosbee control using Uart
static const unsigned char uart_forward         = 0x01;   //Rosbee Forward
static const unsigned char uart_backward        = 0x02;   //Rosbee Backward
static const unsigned char uart_set_speed       = 0x03;   //Rosbee Set Speed
static const unsigned char uart_get_speed       = 0x04;   //Rosbee Get Speed
static const unsigned char uart_rotate_left     = 0x05;   //Rosbee rotate left
static const unsigned char uart_rotate_right    = 0x06;   //Rosbee rotate right
static const unsigned char uart_stop            = 0x07;   //Rosbee stop
static const unsigned char uart_version         = 0x0A;   //Firmware Version

#endif //ADT_QIK_COMMANDS_HPP
