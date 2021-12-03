#include <iostream>
#include <fstream>
#include <exception>
#include <string>

#include "gpio_pin.h"
#include "gpio_socket.h"

using namespace GPIO;

std::optional<unsigned> GPIO::GPIOPin::parsePindID(const std::string &pinID) {
	if(pinID.size() < 2 && std::toupper(pinID[0]) != 'P'){
		return std::nullopt;
	}
	const int AValue = (int)'A';

	unsigned num = 0;
	num += std::toupper(pinID[1]) - AValue;
	try {
		num += std::stoi(pinID.data() + 2);
	} catch(std::exception &e){
		printf("Failed to parse %s to number\n", pinID.data() + 2);
		return std::nullopt;
	}

	return num;
}

GPIO::GPIOPin::GPIOPin(const std::string &pinID, DirectionMode dir, EdgeMode edge) : 
						m_direction{dir}, m_edge{edge}{
	const auto parsedPinID = parsePindID(pinID);
	unsigned number = parsedPinID ? *parsedPinID : 0;
	try {
		GPIO::Sockets::GPIOSocket::openGPIO(number);
		
		m_socket = new GPIO::Sockets::GPIOSocket("gpio" + std::to_string(number));

		setDirection(m_direction);
		setEdge(m_edge);
		m_pinNumber = number;
	} catch (const std::exception &e) {
		printf("Failed to create pin\n");
	}
}

GPIO::GPIOPin::GPIOPin(unsigned pinNumber, DirectionMode dir, EdgeMode edge) : 
						m_direction{dir}, m_edge{edge} {
	try {
		GPIO::Sockets::GPIOSocket::openGPIO(pinNumber);
		
		m_socket = new GPIO::Sockets::GPIOSocket("gpio" + std::to_string(pinNumber));

		setDirection(m_direction);
		setEdge(m_edge);
		m_pinNumber = pinNumber;
	} catch(const std::exception &e) {
		printf("Failed to create pin\n");
	}
}

GPIO::GPIOPin::GPIOPin(GPIOPin &&pinToMove) {
	if(m_socket != nullptr) {
		delete m_socket;
	}
	m_socket = pinToMove.m_socket;
	pinToMove.m_socket = nullptr;

	m_direction = pinToMove.m_direction;
	pinToMove.m_direction = GPIO::GPIOPin::DirectionMode::None;

	m_edge = pinToMove.m_edge;
	pinToMove.m_edge = GPIO::GPIOPin::EdgeMode::None;

	m_pinNumber = pinToMove.m_pinNumber;
}

GPIOPin &GPIO::GPIOPin::operator=(GPIOPin &&pinToMove) {
	if(&pinToMove == this){
		return *this;
	}

	if(m_socket != nullptr) {
		delete m_socket;
	}

	m_socket = pinToMove.m_socket;
	pinToMove.m_socket = nullptr;

	m_direction = pinToMove.m_direction;
	pinToMove.m_direction = GPIO::GPIOPin::DirectionMode::None;

	m_edge = pinToMove.m_edge;
	pinToMove.m_edge = GPIO::GPIOPin::EdgeMode::None;

	m_pinNumber = pinToMove.m_pinNumber;

	return *this;
}

GPIO::GPIOPin::~GPIOPin() {
	if(m_socket != nullptr){
		delete m_socket;
		Sockets::GPIOSocket::closeGPIO(m_pinNumber);
	}
}

void GPIO::GPIOPin::setDirection(DirectionMode direction){
	if(m_socket == nullptr){ 
		throw std::runtime_error("Try to reach unexisting socket"); 
	}
	std::string stringToWrite;
	switch (direction) {
		case DirectionMode::None: return;
		case DirectionMode::In: stringToWrite = "in"; break;
		case DirectionMode::Out: stringToWrite = "out"; break;
	}
	std::ofstream directionPath{m_socket->getPath() + GPIOPin::directionPath};
	directionPath << stringToWrite;
}

void GPIO::GPIOPin::setEdge(EdgeMode edgeMode){
	if(m_socket == nullptr){ 
		throw std::runtime_error("Try to reach unexisting socket"); 
	}
	std::string stringToWrite;
	switch (edgeMode) {
		case EdgeMode::None: stringToWrite = "none"; break;
		case EdgeMode::Rising: stringToWrite = "rising"; break;
		case EdgeMode::Falling: stringToWrite = "falling"; break;
		case EdgeMode::Both: stringToWrite = "both"; break;
	}
	std::ofstream edgePath{m_socket->getPath() + GPIOPin::edgePath};
	edgePath << stringToWrite;
}

GPIO::PinValue GPIO::GPIOPin::readValue() {
	if(m_socket == nullptr){ 
		throw std::runtime_error("Try to reach unexisting socket"); 
	}

	return m_socket->read();
}

void GPIO::GPIOPin::writeValue(GPIO::PinValue valueToWrite) {
	if(m_socket == nullptr){ 
		throw std::runtime_error("Try to reach unexisting socket"); 
	}

	m_socket->write(valueToWrite);
}

void GPIO::GPIOPin::pollAllEvets(Sockets::callbackFunc callback) {
	if(m_socket == nullptr) { 
		throw std::runtime_error("Try to reach unexisting socket"); 
	}
	if(m_socket->isReading()) {
		throw std::runtime_error("Pin is already polling");
	}
	m_socket->pollAllEvents(callback);
}

void GPIO::GPIOPin::pollInterrupts(Sockets::callbackFunc callback, EdgeMode eventType) {
	if(m_socket == nullptr) {
		throw std::runtime_error("Try to reach unexisting socket"); 
	}
	if(m_socket->isReading()) {
		throw std::runtime_error("Pin is already polling");
	}
	setEdge(eventType);
	m_socket->pollPriorityEvents(callback);
}
