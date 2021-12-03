#include <unistd.h>
#include <stdio.h>
#include <sys/epoll.h>
#include <fcntl.h>

#include <exception>
#include <fstream>
#include <iostream>

#include "gpio_socket.h"

using namespace GPIO::Sockets;

GPIO::PinValue GPIO::fromChar(char charToParse) noexcept {
	switch (charToParse) {
		case '1': return GPIO::PinValue::High;
		case '0': return GPIO::PinValue::Low;
		default: return GPIO::PinValue::None;
	}
}

char GPIO::toChar(GPIO::PinValue value) noexcept {
	switch (value) {
		case GPIO::PinValue::High: return '1';
		case GPIO::PinValue::Low: return '0';
		default: return '\0';
	}

}

GPIOSocket::GPIOSocket(const std::string &path) {
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

GPIO::PinValue GPIOSocket::read() {
	if(isReading()){
		printf("Tried to run second read on single socket\n");
		return GPIO::PinValue::None;
	}

	std::ifstream socketToRead{m_path + "/value"};
	char byteToRead;
	socketToRead.get(byteToRead);
	socketToRead.close();

	return fromChar(byteToRead);
}

void GPIOSocket::pollRead(callbackFunc callback, const bool& shouldStop, short flagToUse, const std::string &path) {
	int epollFD = epoll_create(1);
	if(epollFD == -1){
		throw std::runtime_error("Failed to create epoll");
	}

	int socketToRead = open(path.c_str(), O_RDONLY | O_NONBLOCK);
	if(socketToRead < 0) {
		throw std::runtime_error(std::string("Failed to open file\nPath: ") + path.data());
	}

	// Creating epoll struct
	struct epoll_event gpioPollFD;
	gpioPollFD.data.fd = socketToRead;
	gpioPollFD.events = EPOLLET;

	// Adding gpio fd to epoll
	if(epoll_ctl(epollFD, EPOLL_CTL_ADD, socketToRead, &gpioPollFD) == -1) {
		throw std::runtime_error("Failed to add custom file descriptor to epoll");
	}
	
	char readByte;
	bool callbackStop = false;
	int pollValue;

	do {
		struct epoll_event events;
		pollValue = epoll_wait(epollFD, &events, 1, 100);
		if(pollValue == 0) { continue; }
		if(pollValue == -1) { break; }
			
		lseek(events.data.fd, 0, SEEK_SET);
		if(::read(events.data.fd, &readByte, 1)){
			callbackStop = callback(fromChar(readByte));
		}
	} while(!shouldStop && !callbackStop);
	close(epollFD);
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

void GPIOSocket::write(GPIO::PinValue valueToWrite) {
	std::ofstream socketToWrite{m_path + "/value"};

	socketToWrite.put(toChar(valueToWrite));
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

