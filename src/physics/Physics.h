#pragma once
#include "../logger/Logger.h"
#include <thread>
#define IRIS_PHYSICS_TICK_RATE 32
namespace Iris {
	namespace Physics {
		static bool s_paused = false;
		static bool s_shouldStop = true;
		static std::thread s_thread;
		static std::chrono::time_point<std::chrono::steady_clock, std::chrono::duration<double, std::ratio<1, IRIS_PHYSICS_TICK_RATE>>> s_lastFrame;
		void thread() {
			INFO("Physics thread has started. Running with {} tick rate", IRIS_PHYSICS_TICK_RATE);
			constexpr std::chrono::duration<double, std::ratio<1, IRIS_PHYSICS_TICK_RATE>> physicsFrameTime{1};
			while (!s_shouldStop) {
				if (s_paused) continue;
				TIMER("PHYSICS TICK");
				s_lastFrame += physicsFrameTime;
				std::this_thread::sleep_until(s_lastFrame);
			}
			INFO("Physics thread has stopped");
		}
		void start() {
			s_shouldStop = false;
			s_lastFrame = std::chrono::steady_clock::now();
			s_thread = std::thread(thread);
		}
		void pause() {
			s_paused = !s_paused;
		}
		void stop() {
			s_shouldStop = true;
			s_thread.join();
		}

	}
}