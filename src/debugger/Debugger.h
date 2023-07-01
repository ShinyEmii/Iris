#pragma once
#include <vector>
#include <chrono>
#include <unordered_map>
#include <format>
#include "../utils/units.h"
#ifdef IRIS_DEBUG
#ifdef _WIN32
#define __FUNC_NAME__   __FUNCTION__  
#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#elif
#define __FUNC_NAME__   __func__ 
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif
namespace Iris {
	namespace Debugger {
		struct TimerData {
			f64 length;
			const char* fileName;
			i32 line;
			u64 count;
		};
		struct LogData {
			std::chrono::system_clock::time_point time;
			const char* fileName, * level, * message;
			i32 line;
		};
		static std::unordered_map<const char*, TimerData> timerData;
		static std::vector<LogData> logData;
		void save(const char* fileName) {
			FILE* f;
			fopen_s(&f, fileName, "w");
			if (!f) {
				printf("Failed to write debug data to %s!", fileName);
				return;
			}
			fprintf_s(f, "-------| Timers |-------\n");
			for (auto data : timerData) {
				if (data.second.count > 1) {
					fprintf_s(f, "%s : %d | %s | %3.2fms in %llu samples\n", data.second.fileName, data.second.line, data.first, data.second.length / data.second.count, data.second.count);
					printf("%s : %d | %s | %3.2fms in %llu samples\n", data.second.fileName, data.second.line, data.first, data.second.length / data.second.count, data.second.count);
				}
				else {
					fprintf_s(f, "%s : %d | %s | %3.2fms\n", data.second.fileName, data.second.line, data.first, data.second.length);
					printf("%s : %d | %s | %3.2fms\n", data.second.fileName, data.second.line, data.first, data.second.length);
				}
			}
			fprintf_s(f, "-------| Logs |-------\n");
			for (const LogData& data : logData) {
				const std::time_t time = std::chrono::system_clock::to_time_t(data.time);
				char timeString[std::size("yyyy-mm-dd hh:mm:ss")];
				tm t{};
				::localtime_s(&t, &time);
				std::strftime(std::data(timeString), std::size(timeString), "%F %T", &t);
				fprintf_s(f, "%s | %s | %s : %d | %s\n", timeString, data.level, data.fileName, data.line, data.message);
				free((void*)data.message);
			}
			fclose(f);
		}
		void clean() {
			timerData.clear();
			logData.clear();
		}
		template <typename... Args>
		void LOG(const char* fileName, i32 line, const char* name, const char* level, const char* format, Args&&... args) {
			char* message = _strdup(std::vformat(format, std::make_format_args(args...)).c_str());
			printf("%s | %s : %d | %s\n", level, fileName, line, message);
			logData.emplace_back(std::chrono::system_clock::now(), fileName, level, message, line);
		}
		class Timer {
		public:
			Timer(const char* fileName, i32 line, const char* name)
				: m_start(std::chrono::system_clock::now()), m_fileName(fileName), m_line(line), m_timerName(name) {};
			~Timer() {
				if (timerData.contains(m_timerName)) {
					timerData.at(m_timerName).count++;
					timerData.at(m_timerName).length += (std::chrono::duration<f64, std::milli>(std::chrono::system_clock::now() - m_start)).count();
				}
				else {
					timerData.emplace(m_timerName, TimerData{ (std::chrono::duration<f64, std::milli>(std::chrono::system_clock::now() - m_start)).count(), m_fileName, m_line, 1 });
				}
			}
		private:
			std::chrono::system_clock::time_point m_start;
			const char* m_fileName, * m_timerName;
			i32 m_line;
		};
	}
}
#define IRIS_TIME_SCOPE(NAME) Iris::Debugger::Timer scopeTimer##line{__FILENAME__ , __LINE__, NAME}
#define IRIS_TIME_FUNCTION() Iris::Debugger::Timer funcTimer##line{__FILENAME__ , __LINE__, __FUNC_NAME__}
#define INFO(FORMAT, ...) Iris::Debugger::LOG(__FILENAME__ , __LINE__, __FUNC_NAME__, "INFO", FORMAT, __VA_ARGS__); 
#define WARN(FORMAT, ...) Iris::Debugger::LOG(__FILENAME__ , __LINE__, __FUNC_NAME__, "WARN", FORMAT, __VA_ARGS__); 
#define ERROR(FORMAT, ...) Iris::Debugger::LOG(__FILENAME__ , __LINE__, __FUNC_NAME__, "ERROR", FORMAT, __VA_ARGS__); 
#else
#define IRIS_TIME_SCOPE(NAME) ;
#define IRIS_TIME_FUNCTION() ;
#define INFO(FORMAT, ...) ;
#define WARN(FORMAT, ...) ;
#define ERROR(FORMAT, ...) ;
namespace Iris {
	namespace Debugger {
		void clean() {};
		void save(const char*) {};
	}
}
#endif