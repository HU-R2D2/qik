//
// Created by martijn on 6/6/16.
//

#ifndef ADT_QIK_MASTER_HPP
#define ADT_QIK_MASTER_HPP

#include <string>
#include <libserial.hpp>
#include "qik_commands.hpp"
class qik_master {
public:
	qik_master(std::string port, int baudrate);
	~qik_master();

	bool forward();
	bool stop();

	bool setSpeed(uint8_t speed);
	int getSpeed();

	bool rotate_left();
	bool rotate_right();

	char get_version();
private:
	bool check_response(unsigned char command);
	LibSerial serial_connection;
};


#endif //ADT_QIK_MASTER_HPP
