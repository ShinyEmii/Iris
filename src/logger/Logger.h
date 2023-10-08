#pragma once
#include <vector>
#include <chrono>
#include <unordered_map>
#include <format>

#ifdef _WIN32
#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#elif
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif

namespace Iris {
	namespace Logger {
		struct TimerData {
			double length;
			const char* fileName;
			uint32_t line;
			uint32_t count;
		};
		struct LogData {
			std::chrono::system_clock::time_point time;
			const char* fileName, * level;
			std::string message;
			uint32_t line;
		};
		static std::unordered_map<const char*, TimerData> s_timerData;
		static std::vector<LogData> s_logData;
		void save(const char* fileName) {
			FILE* f;
			fopen_s(&f, fileName, "w");
			if (f == nullptr) {
				printf("Failed to save logger data to %s!", fileName);
				return;
			}
			if (s_timerData.size() > 0) {
				fprintf_s(f, "-Timers-\n");
				for (auto& data : s_timerData) {
					if (data.second.count > 1)
						fprintf_s(f, "%s : %d | %s | %.2fms in %u samples\n", data.second.fileName, data.second.line, data.first, data.second.length / data.second.count, data.second.count);
					else
						fprintf_s(f, "%s : %d | %s | %.2fms\n", data.second.fileName, data.second.line, data.first, data.second.length);
				}
			}
			if (s_logData.size() > 0) {
				fprintf_s(f, "-Logs-\n");
				for (const LogData& data : s_logData) {
					const std::time_t time = std::chrono::system_clock::to_time_t(data.time);
					char timeString[std::size("yyyy-mm-dd hh:mm:ss")];
					tm t;
					localtime_s(&t, &time);
					std::strftime(std::data(timeString), std::size(timeString), "%F %T", &t);
					fprintf_s(f, "%s | %s | %s : %d | %s\n", timeString, data.level, data.fileName, data.line, data.message.c_str());
				}
			}
			fclose(f);
		}
		class Timer {
		public:
			Timer(const char* fileName, uint32_t line, const char* name)
				: m_start(std::chrono::system_clock::now()), m_fileName(fileName), m_line(line), m_timerName(name) {};
			~Timer() {
				if (s_timerData.contains(m_timerName)) {
					s_timerData.at(m_timerName).count++;
					s_timerData.at(m_timerName).length += (std::chrono::duration<double, std::milli>(std::chrono::system_clock::now() - m_start)).count();
				}
				else {
					s_timerData.emplace(m_timerName, TimerData{ (std::chrono::duration<double, std::milli>(std::chrono::system_clock::now() - m_start)).count(), m_fileName, m_line, 1 });
				}
			}
		private:
			std::chrono::system_clock::time_point m_start;
			const char* m_fileName, * m_timerName;
			uint32_t m_line;
		};
	}
}
#define TIMER(NAME) Iris::Logger::Timer scopeTimer##line{__FILENAME__ , (uint32_t)__LINE__, NAME}
#ifdef IRIS_DEBUG
#define LOG(FORMAT, ...) printf("\x1b[48;5;33m\x1b[38;5;0m LOG \033[0m %s : %d | %s\n", __FILENAME__, __LINE__, std::vformat(FORMAT, std::make_format_args(__VA_ARGS__)).c_str()); Iris::Logger::s_logData.emplace_back(std::chrono::system_clock::now(), __FILENAME__, "LOG", std::vformat(FORMAT, std::make_format_args(__VA_ARGS__)), __LINE__)
#define INFO(FORMAT, ...) printf("\x1b[48;5;42m\x1b[38;5;0m INFO \033[0m %s : %d | %s\n", __FILENAME__, __LINE__, std::vformat(FORMAT, std::make_format_args(__VA_ARGS__)).c_str()); Iris::Logger::s_logData.emplace_back(std::chrono::system_clock::now(), __FILENAME__, "INFO", std::vformat(FORMAT, std::make_format_args(__VA_ARGS__)), __LINE__)
#define WARN(FORMAT, ...) printf("\x1b[48;5;226m\x1b[38;5;0m WARN \033[0m %s : %d | %s\n", __FILENAME__, __LINE__, std::vformat(FORMAT, std::make_format_args(__VA_ARGS__)).c_str()); Iris::Logger::s_logData.emplace_back(std::chrono::system_clock::now(), __FILENAME__, "WARN", std::vformat(FORMAT, std::make_format_args(__VA_ARGS__)), __LINE__)
#define ERROR(FORMAT, ...) printf("\x1b[48;5;160m\x1b[38;5;255m ERROR \033[0m %s : %d | %s\n", __FILENAME__, __LINE__, std::vformat(FORMAT, std::make_format_args(__VA_ARGS__)).c_str()); Iris::Logger::s_logData.emplace_back(std::chrono::system_clock::now(), __FILENAME__, "ERROR", std::vformat(FORMAT, std::make_format_args(__VA_ARGS__)), __LINE__)
#else
#define LOG(FORMAT, ...) Iris::Logger::s_logData.emplace_back(std::chrono::system_clock::now(), __FILENAME__, "LOG", std::vformat(FORMAT, std::make_format_args(__VA_ARGS__)), __LINE__)
#define INFO(FORMAT, ...) Iris::Logger::s_logData.emplace_back(std::chrono::system_clock::now(), __FILENAME__, "INFO", std::vformat(FORMAT, std::make_format_args(__VA_ARGS__)), __LINE__)
#define WARN(FORMAT, ...) Iris::Logger::s_logData.emplace_back(std::chrono::system_clock::now(), __FILENAME__, "WARN", std::vformat(FORMAT, std::make_format_args(__VA_ARGS__)), __LINE__)
#define ERROR(FORMAT, ...) Iris::Logger::s_logData.emplace_back(std::chrono::system_clock::now(), __FILENAME__, "ERROR", std::vformat(FORMAT, std::make_format_args(__VA_ARGS__)), __LINE__)
#endif
#ifdef IRIS_IGNORE_WARNINGS
#define WARN(FORMAT, ...)
#endif
#ifdef IRIS_NO_LOGS 
#define LOG(FORMAT, ...)
#define INFO(FORMAT, ...)
#define WARN(FORMAT, ...)
#define ERROR(FORMAT, ...)
#endif