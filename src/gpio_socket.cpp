#include <exception>
#include <fstream>
#include <iostream>

#include "gpio_socket.h"

using namespace GPIOSockets;

GPIOSocket::GPIOSocket(std::string path) {
	m_path = SYS_GPIO_PATH + path;
}

std::optional<char> GPIOSocket::read() {
	std::optional<byte> readData = std::nullopt;
	try { 
		std::ifstream socketToRead{m_path + "/value"};
		char byteToRead;
		if(!socketToRead.get(byteToRead).eof()){
			readData = byteToRead;
		}
	} catch(std::exception&) {

	}

	return readData;
}

bool GPIOSocket::write(const std::vector<char> &arraToWrite) {
	try {
		std::ofstream socketToWrite{m_path + "/value"};

		for(char charToWrite : arraToWrite){
			socketToWrite.put(charToWrite);
		}
		socketToWrite.flush();
	} catch (std::exception&) {
		return false;
	}
	return true;
}

bool GPIOSocket::openGPIO(int portToOpen){
	try {
		std::ofstream exportSocket{SYS_GPIO_EXPORT_PATH};
		std::string socketNumber = std::to_string(portToOpen); // To work with file sockets we need to write string value
		exportSocket.write(reinterpret_cast<char*>(socketNumber.data()), sizeof(char) * socketNumber.length());
	} catch (std::exception&) {
		return false;
	}
	return true;
}

bool GPIOSocket::closeGPIO(int portToClose){
	try {
		std::ofstream unexportSocket{SYS_GPIO_UNEXPORT_PATH};
		std::string socketNumber = std::to_string(portToClose); // To work with file sockets we need to write string value
		unexportSocket.write(reinterpret_cast<char*>(socketNumber.data()), sizeof(char) * socketNumber.length());
	} catch (std::exception&) {
		return false;
	}
	return true;

}


