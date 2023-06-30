#pragma once
#include "SoundSource.h"
namespace Iris {
	namespace Audio {
		ALCdevice* s_audioDevice;
		ALCcontext* s_audioContext;
		i8 init() {
			s_audioDevice = alcOpenDevice(nullptr);
			if (s_audioDevice == nullptr) {
				ERROR("Failed to initialize audio device");
				return 0;
			}
			s_audioContext = alcCreateContext(s_audioDevice, nullptr);
			alcMakeContextCurrent(s_audioContext);

			alGenSources(AUDIO_POOL_SIZE, s_audioPool);
			memset(s_audioPoolUsage, 0, AUDIO_POOL_SIZE);
			INFO("Audio device initialized!");
			INFO("Using Audio Device: {}", (const char*)alcGetString(s_audioDevice, ALC_ALL_DEVICES_SPECIFIER));
			return 1;
		}
		void destroy() {
			alDeleteSources(AUDIO_POOL_SIZE, s_audioPool);
		}
	}
}