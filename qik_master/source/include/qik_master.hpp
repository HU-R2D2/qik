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

	void forward();
	void stop();

	void setSpeed(uint8_t speed);
	int getSpeed();

	void rotate_left();
	void rotate_right();

	char get_version();
private:
	LibSerial serial_connection;
};


#endif //ADT_QIK_MASTER_HPP
