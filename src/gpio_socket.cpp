#include <exception>
#include <fstream>
#include <iostream>

#include <poll.h>
#include <unistd.h>
#include <stdio.h>

#include "gpio_socket.h"

using namespace GPIOSockets;

GPIOSocket::GPIOSocket(std::string path) {
	m_path = SYS_GPIO_PATH + path;
}
GPIOSocket::~GPIOSocket() {
	if(isReading()){
		m_shouldStop = true;
		if(m_runningThread->joinable()){
			m_runningThread->join();
		}
		delete m_runningThread;
	}
}

std::optional<char> GPIOSocket::read() noexcept {
	std::optional<byte> readData = std::nullopt;

	if(isReading()){
		printf("Tried to run second read on single socket\n");
		return readData;
	}

	try { 
		std::ifstream socketToRead{m_path + "/value"};
		char byteToRead;
		socketToRead.get(byteToRead);
		readData = byteToRead;
	} catch(std::exception&) {
		printf("Error while reading single byte\n");
	}

	return readData;
}

void GPIOSocket::readFunction(callbackFunc callback, const bool& shouldStop, const std::string &path) {
	FILE* socketToRead = fopen(path.data(), "r");
	if(socketToRead == nullptr) {
		printf("Failed to open file\nPath: %s\n", path.data());
		return;
	}
	pollfd gpioPollFD;
	gpioPollFD.fd = fileno(socketToRead);
	gpioPollFD.events = POLLIN;

	char readByte;
	bool callbackStop = false;
	int pollValue;

	do {
		pollValue = poll(&gpioPollFD, 1, -1);
		if(pollValue < 0){ continue; }
			
		if( (gpioPollFD.revents & POLLIN) == POLLIN){
			lseek(gpioPollFD.fd, 0, SEEK_SET);
			if(std::fread(&readByte, sizeof(readByte), 1, socketToRead) &&
				readByte != '\0' && readByte != '\n'){
				callbackStop = callback(readByte);
			}

		}
	} while(!shouldStop && !callbackStop);
}

void GPIOSocket::continiousRead(callbackFunc callback){
	if(isReading()){
		printf("Tried to run second continious read on single socket\n");
		return;
	}
	m_shouldStop = false;
	m_runningThread = new std::thread(readFunction, callback, this->m_shouldStop, getPath() + "/value");
}

void GPIOSocket::stopReading() {
	if(isReading()){
		m_shouldStop = true;
		m_runningThread->join();
		delete m_runningThread;
	} else {
		printf("Tried to stop reading that didn't start\n");
	}
}

bool GPIOSocket::write(const std::vector<char> &arraToWrite) noexcept{
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

bool GPIOSocket::write(char charToWrite) noexcept{
	try {
		std::ofstream socketToWrite{m_path + "/value"};

		socketToWrite.put(charToWrite);
		socketToWrite.flush();
	} catch (std::exception&) {
		return false;
	}
	return true;
}

bool GPIOSocket::openGPIO(int portToOpen) noexcept {
	try {
		std::ofstream exportSocket{SYS_GPIO_EXPORT_PATH};
		std::string socketNumber = std::to_string(portToOpen); // To work with file sockets we need to write string value
		exportSocket.write(reinterpret_cast<char*>(socketNumber.data()), sizeof(char) * socketNumber.length());
	} catch (std::exception&) {
		return false;
	}
	return true;
}

bool GPIOSocket::closeGPIO(int portToClose) noexcept {
	try {
		std::ofstream unexportSocket{SYS_GPIO_UNEXPORT_PATH};
		std::string socketNumber = std::to_string(portToClose); // To work with file sockets we need to write string value
		unexportSocket.write(reinterpret_cast<char*>(socketNumber.data()), sizeof(char) * socketNumber.length());
	} catch (std::exception&) {
		return false;
	}
	return true;
}

