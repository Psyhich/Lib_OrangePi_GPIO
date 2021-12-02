#ifndef GPIO_PIN_H
#define GPIO_PIN_H 

#include "gpio_socket.h"

namespace GPIO {



class GPIOPin {
public:
	enum class DirectionMode{
		None,
		Out,
		In
	};

	enum class EdgeMode{
		None,
		Rising,
		Falling,
		Both
	};

	enum class PinValue{
		None,
		High,
		Low
	};

	GPIOPin(std::string pinID, DirectionMode dir=DirectionMode::None, EdgeMode edge=EdgeMode::None);
	GPIOPin(unsigned pinNumber, DirectionMode dir=DirectionMode::None, EdgeMode edge=EdgeMode::None);

	GPIOPin(GPIOPin &&pinToMove);
	GPIOPin &operator=(GPIOPin &&pinToMove);

	~GPIOPin();

	GPIOPin(const GPIOPin &copyPin) = delete;
	GPIOPin operator=(const GPIOPin &copyPin) = delete;

	inline unsigned getNumber() const noexcept { return m_pinNumber; }

	void setDirection(DirectionMode direction);
	inline DirectionMode getDirection() const noexcept { return m_direction; }
	
	void setEdge(EdgeMode edgeMode);
	inline EdgeMode getEdge() const noexcept { return m_edge; }

	PinValue readValue();
	void writeValue(PinValue valueToWrite);

	void pollAllEvets(Sockets::callbackFunc callback);
	void pollInterrupts(Sockets::callbackFunc callback, EdgeMode eventType);

	inline bool isPolling() const noexcept { 
		return m_socket != nullptr && m_socket->isReading(); 
	}
	inline void awaitPolling() {
		if(isPolling()){
			m_socket->joinReading();
		}
	}
	
protected:
private:
	static inline const std::string directionPath{"/direction"};
	static inline const std::string edgePath{"/edge"};
	static inline const std::string valuePath{"/value"};

	static std::optional<unsigned> parsePindID(const std::string &pinID);
	unsigned m_pinNumber;
	DirectionMode m_direction;
	EdgeMode m_edge;
	Sockets::GPIOSocket *m_socket;
};


} // GPIO 



#endif //GPIO_PIN_H
