#include <exception>
#include <fstream>
#include <iostream>

#include <unistd.h>
#include <stdio.h>

#include "gpio_socket.h"

using namespace GPIO::Sockets;

GPIOSocket::GPIOSocket(std::string path) {
	m_path = GPIO_PATH + path;
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

std::optional<char> GPIOSocket::read() {
	std::optional<byte> readData = std::nullopt;

	if(isReading()){
		printf("Tried to run second read on single socket\n");
		return readData;
	}

	std::ifstream socketToRead{m_path + "/value"};
	char byteToRead;
	socketToRead.get(byteToRead);
	readData = byteToRead;

	return readData;
}

void GPIOSocket::pollRead(callbackFunc callback, const bool& shouldStop, short flagToUse, const std::string &path) {
	FILE* socketToRead = fopen(path.data(), "r");
	if(socketToRead == nullptr) {
		printf("Failed to open file\nPath: %s\n", path.data());
		return;
	}
	pollfd gpioPollFD;
	gpioPollFD.fd = fileno(socketToRead);
	gpioPollFD.events = flagToUse;

	char readByte;
	bool callbackStop = false;
	int pollValue;

	do {
		pollValue = poll(&gpioPollFD, 1, -1);
		if(pollValue < 0){ continue; }
			
		if( (gpioPollFD.revents & flagToUse) == flagToUse){
			lseek(gpioPollFD.fd, 0, SEEK_SET);
			if(std::fread(&readByte, sizeof(readByte), 1, socketToRead) &&
				readByte != '\0' && readByte != '\n'){
				callbackStop = callback(readByte);
			}

		}
	} while(!shouldStop && !callbackStop);
}

void GPIOSocket::pollAllEvents(callbackFunc callback) {
	if(isReading()){
		printf("Tried to run second continious read on single socket\n");
		return;
	}

	m_shouldStop = false;
	m_runningThread = new std::thread(pollAll, callback, this->m_shouldStop, getPath() + "/value");
}

void GPIOSocket::pollPriorityEvents(callbackFunc callback){
	if(isReading()){
		printf("Tried to run second continious read on single socket\n");
		return;
	}

	m_shouldStop = false;
	m_runningThread = new std::thread(pollInterupts, callback, this->m_shouldStop, getPath() + "/value");
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

void GPIOSocket::write(char charToWrite) {
	std::ofstream socketToWrite{m_path + "/value"};

	socketToWrite.put(charToWrite);
	socketToWrite.flush();
}

void GPIOSocket::openGPIO(int portToOpen) {
	std::ofstream exportSocket{SYS_GPIO_EXPORT_PATH};
	std::string socketNumber = std::to_string(portToOpen); // To work with file sockets we need to write string value
	exportSocket.write(reinterpret_cast<char*>(socketNumber.data()), sizeof(char) * socketNumber.length());
}

void GPIOSocket::closeGPIO(int portToClose) {
	std::ofstream unexportSocket{SYS_GPIO_UNEXPORT_PATH};
	std::string socketNumber = std::to_string(portToClose); // To work with file sockets we need to write string value
	unexportSocket.write(reinterpret_cast<char*>(socketNumber.data()), sizeof(char) * socketNumber.length());
}

