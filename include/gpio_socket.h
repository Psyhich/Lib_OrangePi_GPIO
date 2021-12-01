#ifndef ORANGEPI_LITE_GPIO_SOCKET
#define ORANGEPI_LITE_GPIO_SOCKET

#include <poll.h>

#include <optional>
#include <string>
#include <vector>
#include <thread>
#include <functional>

namespace GPIO::Sockets {
	using byte = unsigned char;
	using callbackFunc = std::function<bool(char)>;

	class GPIOSocket {
	public:
		GPIOSocket(const std::string &path);
		~GPIOSocket();
		static inline const std::string GPIO_PATH = "/sys/class/gpio/";
		static inline const std::string SYS_GPIO_EXPORT_PATH = GPIO_PATH + "export";
		static inline const std::string SYS_GPIO_UNEXPORT_PATH = GPIO_PATH + "unexport";
		
		std::optional<char> read();
		void pollAllEvents(callbackFunc callback);
		void pollPriorityEvents(callbackFunc callback);

		void write(char charToWrite);

		inline const std::string &getPath() const noexcept { return m_path; }
		inline bool isReading() const noexcept { return m_runningThread != nullptr; }

		inline void joinReading(){
			if(isReading() && m_runningThread->joinable()){
				m_runningThread->join();
			}
		}
		void stopReading();

		static void openGPIO(int portToOpen);
		static void closeGPIO(int portToOpen);
	protected:
	private:
		static void pollRead(callbackFunc callback, const bool& shouldStop, short flagToUse, const std::string &path);

		static inline void pollAll(callbackFunc callback, const bool& shouldStop, const std::string &path){
			pollRead(callback, shouldStop, POLLIN, path);
		}

		static inline void pollInterupts(callbackFunc callback, const bool& shouldStop, const std::string &path) {
			pollRead(callback, shouldStop, POLLPRI, path);
		}
		std::string m_path;

		std::thread *m_runningThread{nullptr};
		bool m_shouldStop{false};

		static inline constexpr int MAX_EPOLL_EVENTS = 100;
	};
}

#endif
