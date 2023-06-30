#pragma once
#include <al.h>
#include <alc.h>
#define AUDIO_POOL_SIZE 256
namespace Iris {
	namespace Audio {
		ALuint s_audioPool[AUDIO_POOL_SIZE];
		bool s_audioPoolUsage[AUDIO_POOL_SIZE];
	}
}