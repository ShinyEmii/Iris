#pragma once
#include "SoundSource.h"
namespace Iris {
	namespace Audio {
		static bool s_deviceCreated = false;
		static bool s_efxAvailable;
		static ALCdevice* s_audioDevice;
		static ALCcontext* s_audioContext;
		void createDevice() {
			if (s_deviceCreated) {
				ERROR("Audio device was already initialized!");
				return;
			}
			s_audioDevice = alcOpenDevice(nullptr);
			if (s_audioDevice == nullptr) {
				ERROR("Failed to initialize audio device!");
				return;
			}
			INFO("Audio device initialized");
			INFO("Using audio device: {}", (const char*)alcGetString(s_audioDevice, ALC_ALL_DEVICES_SPECIFIER));
			if (alcIsExtensionPresent(s_audioDevice, "ALC_EXT_EFX") == AL_FALSE) {
				WARN("OpenAL EFX extension isn't available! Some features disabled!");
				s_efxAvailable = false;
			}
			else {
				INFO("OpenAL EFX extension enabled");
				s_efxAvailable = true;
			}
			s_audioContext = alcCreateContext(s_audioDevice, nullptr);
			alcMakeContextCurrent(s_audioContext);

			alGenSources(IRIS_AUDIO_POOL_SIZE, Audio::s_audioPool);
			s_deviceCreated = true;
			Resource::s_defaultSound.setAsset(new Sound());
		}
		void destroyDevice() {
			if (!s_deviceCreated) {
				ERROR("Tried to destroy audio device without initializing it!");
				return;
			}
			alDeleteSources(IRIS_AUDIO_POOL_SIZE, Audio::s_audioPool);
			if (s_audioDevice != nullptr) {
				alcDestroyContext(s_audioContext);
				alcCloseDevice(s_audioDevice);
			}
			s_deviceCreated = false;
			INFO("Audio device destroyed");
		}
	}
}