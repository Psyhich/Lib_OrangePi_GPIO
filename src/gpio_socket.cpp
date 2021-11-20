#include <exception>
#include <fstream>

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
		exportSocket.write(reinterpret_cast<char*>(&portToOpen), sizeof(portToOpen));
	} catch (std::exception&) {
		return false;
	}
	return true;
}

bool GPIOSocket::closeGPIO(int portToClose){
	try {
		std::ofstream unexportSocket{SYS_GPIO_UNEXPORT_PATH};
		unexportSocket.write(reinterpret_cast<char*>(&portToClose), sizeof(portToClose));
	} catch (std::exception&) {
		return false;
	}
	return true;

}


