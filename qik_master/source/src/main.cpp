//
// Created by martijn on 6/6/16.
//

#include <iostream>
#include <libserial.hpp>
#include "../include/qik_master.hpp"
#include <chrono>
#include <thread>

int main()
{
	qik_master qik = qik_master("/dev/ttyUSB0", 115200);
	std::cout << "Sending forward command" << std::endl;
	qik.forward();
	std::cout << "wait 5 seconds" << std::endl;
	std::this_thread::sleep_for(std::chrono::seconds(5));
	std::cout << "Sending stop command";
	qik.stop();
	
	return 0;
}