#ifndef ORANGEPI_LITE_GPIO_SOCKET
#define ORANGEPI_LITE_GPIO_SOCKET

#define GPIO_PATH "/sys/class/gpio/"

#include <optional>
#include <string>
#include <vector>

namespace GPIOSockets {
	using byte = unsigned char;

	constexpr const char* SYS_GPIO_PATH = GPIO_PATH;
	constexpr const char* SYS_GPIO_EXPORT_PATH = GPIO_PATH "export";
	constexpr const char* SYS_GPIO_UNEXPORT_PATH = GPIO_PATH "unexport";

	class GPIOSocket {
	public:
		GPIOSocket(std::string path);
		
		std::optional<char> read();
		bool write(const std::vector<char> &arrayToWrite);

		inline std::string getPath() const noexcept { return m_path; }

		static bool openGPIO(int portToOpen);
		static bool closeGPIO(int portToOpen);
	protected:
	private:
		std::string m_path;
	};
}

#endif
