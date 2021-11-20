#ifndef ORANGEPI_LITE_GPIO_SOCKET
#define ORANGEPI_LITE_GPIO_SOCKET

#define GPIO_PATH "/sys/class/gpio/"

#include <optional>
#include <string>
#include <vector>
#include <thread>

namespace GPIOSockets {
	using byte = unsigned char;
	using callbackFunc = bool(*)(char);

	constexpr const char* SYS_GPIO_PATH = GPIO_PATH;
	constexpr const char* SYS_GPIO_EXPORT_PATH = GPIO_PATH "export";
	constexpr const char* SYS_GPIO_UNEXPORT_PATH = GPIO_PATH "unexport";

	class GPIOSocket {
	public:
		GPIOSocket(std::string path);
		~GPIOSocket();
		
		std::optional<char> read() noexcept;
		void continiousRead(callbackFunc callback);
		bool write(const std::vector<char> &arrayToWrite) noexcept;
		bool write(char charToWrite) noexcept;

		inline std::string getPath() const noexcept { return m_path; }
		inline bool isReading() const noexcept { return m_runningThread != nullptr; }

		inline void joinReading(){
			if(isReading() && m_runningThread->joinable()){
				m_runningThread->join();
			}
		}
		void stopReading();

		static bool openGPIO(int portToOpen) noexcept;
		static bool closeGPIO(int portToOpen) noexcept;
	protected:
	private:
		static void readFunction(callbackFunc callback, const bool& shouldStop, const std::string &path);
		std::string m_path;

		std::thread *m_runningThread{nullptr};
		bool m_shouldStop{false};
	};
}

#endif
