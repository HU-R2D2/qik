//
// Created by martijn on 6/6/16.
//

#include "../include/qik_commands.hpp"
#include "../include/qik_master.hpp"

bool qik_master::check_response(unsigned char command) {
	char buf[2];
	serial_connection.read(&buf, 2);
	return buf[0] == uart_forward && buf[1] == uart_ok;
}

bool qik_master::backward() {
	serial_connection.writeChar(uart_backward);
	return check_response(uart_backward);
}

qik_master::qik_master(std::string port, unsigned int baudrate) {
	if(serial_connection.open(port.c_str(), baudrate) != 1){
		throw "failed to open serial port";
	};
}

qik_master::~qik_master() {
	serial_connection.close();
}

bool qik_master::forward(){
	serial_connection.writeChar(uart_forward);
	return check_response(uart_forward);
}

bool qik_master::stop(){
	serial_connection.writeChar(uart_stop);
	return check_response(uart_stop);
}


bool qik_master::setSpeed(uint8_t speed) {
	serial_connection.writeChar(uart_set_speed);
	serial_connection.writeChar(speed);
	return check_response(uart_set_speed);
}

int qik_master::getSpeed() {
	serial_connection.writeChar(uart_get_speed);
	char result;
	serial_connection.readChar(&result);
	return result;
}

bool qik_master::rotate_left() {
	serial_connection.writeChar(uart_rotate_left);
	return check_response(uart_rotate_left);
}

bool qik_master::rotate_right() {
	serial_connection.writeChar(uart_rotate_right);
	return check_response(uart_rotate_right);
}

char qik_master::get_version() {
	serial_connection.writeChar(uart_version);
	char result;
	serial_connection.readChar(&result);
	return result;
}

union uart_integer_decoder{
	unsigned char as_char_array[4];
	int as_int;
};

int qik_master::get_encoder_speed(int encoder) {
	if(encoder == 1){
		serial_connection.writeChar(uart_encoder_speed0);
		check_response(uart_encoder_speed0);
	}
	else{
		serial_connection.writeChar(uart_encoder_speed1);
		check_response(uart_encoder_speed1);
	}

	uart_integer_decoder result;
	serial_connection.read(&result.as_char_array, 4);
	return result.as_int;
}



