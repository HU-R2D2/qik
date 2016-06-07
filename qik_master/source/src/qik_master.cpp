//
// Created by martijn on 6/6/16.
//

#include "../include/qik_master.hpp"
#include "../include/qik_commands.hpp"
qik_master::qik_master(std::string port, int baudrate) {
	serial_connection.open(port.c_str(), baudrate);
}

qik_master::~qik_master() {
	serial_connection.close();
}

void qik_master::forward(){
	serial_connection.writeChar(uart_forward);
}

void qik_master::stop(){
	serial_connection.writeChar(uart_stop);
}


void qik_master::setSpeed(uint8_t speed) {
	serial_connection.writeChar(uart_set_speed);
	serial_connection.writeChar(speed);
}

int qik_master::getSpeed() {
	serial_connection.writeChar(uart_get_speed);
	char result;
	serial_connection.readChar(&result);
	return result;
}

void qik_master::rotate_left() {
	serial_connection.writeChar(uart_rotate_left);
}

void qik_master::rotate_right() {
	serial_connection.writeChar(uart_rotate_right);
}

char qik_master::get_version() {
	serial_connection.writeChar(uart_version);
	char result;
	serial_connection.readChar(&result);
	return result;
}

