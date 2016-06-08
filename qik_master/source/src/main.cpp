//
// Created by martijn on 6/6/16.
//

#include <iostream>
#include <libserial.hpp>
#include "../include/qik_master.hpp"
#include <chrono>
#include <thread>
void sleep_seconds(int seconds){
	std::this_thread::sleep_for(std::chrono::seconds(seconds));
}
int main()
{
	try {
		qik_master qik = qik_master("/dev/ttyUSB0", 115200);
		std::cout << "Sending forward command" << std::endl;
		if(!qik.forward()){
			std::cout << "oops" << std::endl;
		}
		std::cout << "wait 5 seconds" << std::endl;
		sleep_seconds(5);
		std::cout << "Sending stop command";
//		qik.stop();
		qik.backward();
		sleep_seconds(5);
		qik.stop();
	}
	catch(char const * e){
		std::cout << e << std::endl;
		return 1;
	}
	return 0;
}