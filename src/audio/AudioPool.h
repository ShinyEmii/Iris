#pragma once
#include <al.h>
#include <alc.h>
#define IRIS_AUDIO_POOL_SIZE 256
namespace Iris {
	namespace Audio {
		static ALuint s_audioPool[IRIS_AUDIO_POOL_SIZE];
		static bool s_audioPoolUsage[IRIS_AUDIO_POOL_SIZE];
		int16_t getFreeAudioPoolID() {
			for (int16_t i = 0; i < IRIS_AUDIO_POOL_SIZE; i++) {
				if (!s_audioPoolUsage[i]) return i;
			}
			return -1;
		}
		void cleanAudioPool() {
			ALint state;
			for (int16_t i = 0; i < IRIS_AUDIO_POOL_SIZE; i++) {
				alGetSourcei(s_audioPool[i], AL_SOURCE_STATE, &state);
				if (state != AL_PLAYING) s_audioPoolUsage[i] = false;
			}
		}
	}
}