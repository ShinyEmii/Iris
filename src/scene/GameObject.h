#pragma once
#include "../logger/Logger.h"
namespace Iris {
	class GameObject {
	public:
		virtual void onCreate() = 0;
		virtual void onUpdate() = 0;
		virtual void onDestroy() = 0;
	};
}