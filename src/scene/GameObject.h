#pragma once
namespace Iris {
	namespace Scene {
		class GameObject {
			virtual void onUpdate() = 0;
			virtual void onDraw() = 0;
		};
	}
}