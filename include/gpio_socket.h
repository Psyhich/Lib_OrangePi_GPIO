#ifndef ORANGEPI_LITE_GPIO_SOCKET
#define ORANGEPI_LITE_GPIO_SOCKET

#define GPIO_PATH "/sys/class/gpio/"

#include <optional>
#include <string>
#include <vector>

namespace GPIOSockets {
	using byte = unsigned char;

	constexpr const char* SYS_GPIO_PATH = GPIO_PATH;

	class GPIOSocket {
	public:
		GPIOSocket(std::string path);
		
		std::optional<char> read();
		void write(std::vector<char> arrayToWrite);

		inline std::string getPath() const noexcept { return m_path; }

		static void openGPIO(const int portToOpen);
		static void closeGPIO(const int portToOpen);
	protected:
	private:
	private:
		std::string m_path;
	};
}

#endif
