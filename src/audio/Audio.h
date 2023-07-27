#pragma once
#include "SoundSource.h"
namespace Iris {
	namespace Audio {
		ALCdevice* s_audioDevice;
		ALCcontext* s_audioContext;
		bool s_EFX;
		i8 init() {
			s_audioDevice = alcOpenDevice(nullptr);
			if (s_audioDevice == nullptr) {
				ERROR("Failed to initialize audio device!");
				return 0;
			}
			INFO("Audio device initialized.");
			INFO("Using Audio Device: {}", (const char*)alcGetString(s_audioDevice, ALC_ALL_DEVICES_SPECIFIER));
			if (alcIsExtensionPresent(s_audioDevice, "ALC_EXT_EFX") == AL_FALSE) {
				WARN("OpenAL EFX extension isn't available! Some features disabled!");
				s_EFX = false;
			}
			else {
				INFO("OpenAL EFX extension enabled.");
				s_EFX = true;
			}
			s_audioContext = alcCreateContext(s_audioDevice, nullptr);
			alcMakeContextCurrent(s_audioContext);

			alGenSources(AUDIO_POOL_SIZE, s_audioPool);
			memset(s_audioPoolUsage, 0, AUDIO_POOL_SIZE);
			return 1;
		}
		void destroy() {
			alDeleteSources(AUDIO_POOL_SIZE, s_audioPool);
			if (s_audioDevice != nullptr) {
				alcDestroyContext(s_audioContext);
				alcCloseDevice(s_audioDevice);
			}
			INFO("Audio device destroyed.");
		}
	}
}