#pragma once
#include <vector>
#include <chrono>
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
	struct TimerData {
		std::chrono::system_clock::time_point start, end;
		const char* fileName, * timerName;
		i32 line;
	};
	struct LogData {
		std::chrono::system_clock::time_point time;
		const char* fileName, * level, * message;
		i32 line;
	};
	namespace Debugger {
		static std::vector<TimerData> timerData;
		static std::vector<LogData> logData;
		void save(const char* fileName) {
			FILE* f;
			fopen_s(&f, fileName, "w");
			if (!f) {
				printf("Failed to write debug data to %s!", fileName);
				return;
			}
			fprintf_s(f, "-------| Timers |-------\n");
			for (const TimerData& data : timerData) {
				fprintf_s(f, "%s : %d | %s | %3.2fms\n", data.fileName, data.line, data.timerName, (std::chrono::duration<float, std::milli>(data.end - data.start)).count());
				printf("%s : %d | %s | %3.2fms\n", data.fileName, data.line, data.timerName, (std::chrono::duration<float, std::milli>(data.end - data.start)).count());
			}
			fprintf_s(f, "-------| Logs |-------\n");
			for (const LogData& data : logData) {
				const std::time_t time = std::chrono::system_clock::to_time_t(data.time);
				char timeString[std::size("yyyy-mm-dd hh:mm:ss")];
				tm t{};
				::gmtime_s(&t, &time);
				std::strftime(std::data(timeString), std::size(timeString), "%F %T", &t);
				fprintf_s(f, "%s | %s | %s : %d | %s\n", timeString, data.level, data.fileName, data.line, data.message);
				printf("%s | %s | %s : %d | %s\n", timeString, data.level, data.fileName, data.line, data.message);
			}
			fclose(f);
		}
		void clean() {
			timerData.clear();
			logData.clear();
		}
	}
	template <typename... Args>
	void LOG(const char* fileName, i32 line, const char* name, const char* level, const char* format, Args&&... args) {
		Debugger::logData.emplace_back(std::chrono::system_clock::now(), fileName, level, _strdup(std::vformat(format, std::make_format_args(args...)).c_str()), line);
	}
	class Timer {
	public:
		Timer(const char* fileName, i32 line, const char* name)
			: m_start(std::chrono::system_clock::now()), m_fileName(fileName), m_line(line), m_timerName(name)
		{}
		~Timer() {
			Debugger::timerData.emplace_back(m_start, std::chrono::system_clock::now(), m_fileName, m_timerName, m_line);
		}
	private:
		std::chrono::system_clock::time_point m_start;
		const char* m_fileName, * m_timerName;
		i32 m_line;
	};
}
#define IRIS_TIME_SCOPE(NAME) Iris::Timer scopeTimer##line{__FILENAME__ , __LINE__, NAME}
#define IRIS_TIME_FUNCTION() Iris::Timer funcTimer##line{__FILENAME__ , __LINE__, __FUNC_NAME__}
#define INFO(FORMAT, ...) Iris::LOG(__FILENAME__ , __LINE__, __FUNC_NAME__, "INFO", FORMAT, __VA_ARGS__); 
#define WARN(FORMAT, ...) Iris::LOG(__FILENAME__ , __LINE__, __FUNC_NAME__, "WARN", FORMAT, __VA_ARGS__); 
#define ERROR(FORMAT, ...) Iris::LOG(__FILENAME__ , __LINE__, __FUNC_NAME__, "ERROR", FORMAT, __VA_ARGS__); 
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