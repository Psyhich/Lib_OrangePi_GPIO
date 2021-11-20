#include <fstream>

#include "gpio_socket.h"

using namespace GPIOSockets;

GPIOSocket::GPIOSocket(std::string path) {
	m_path = SYS_GPIO_PATH + path;
}

std::optional<char> GPIOSocket::read() {
	std::ifstream socketToRead(m_path);
	std::optional<byte> readData = std::nullopt;
	char byteToRead;
	if(!socketToRead.get(byteToRead).eof()){
		readData = byteToRead;
	}

	return readData;
}

void GPIOSocket::write(std::vector<char> arraToWrite) {

}

void GPIOSocket::openGPIO(const int portToOpen){

}

void GPIOSocket::closeGPIO(const int portToOpen){

}


