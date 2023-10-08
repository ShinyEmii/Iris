#pragma once
#include "al.h"
#include "alc.h"
#include "SoundData.h"
namespace Iris {
	class Sound {
	public:
		Sound() : m_buffer(0) {};
		Sound(Audio::SoundData& sound) : m_buffer(0) {
			if (sound.size == 0) {
				ERROR("Invalid data was provided!");
				return;
			}
			char* c = new char[256];
			c = nullptr;
			ALenum format = AL_FORMAT_MONO8;
			if (sound.channels == 1 && sound.bitsPerSample == 8)
				format = AL_FORMAT_MONO8;
			else if (sound.channels == 1 && sound.bitsPerSample == 16)
				format = AL_FORMAT_MONO16;
			else if (sound.channels == 2 && sound.bitsPerSample == 8)
				format = AL_FORMAT_STEREO8;
			else if (sound.channels == 2 && sound.bitsPerSample == 16)
				format = AL_FORMAT_STEREO16;
			alGenBuffers(1, &m_buffer);
			alBufferData(m_buffer, format, sound.data, sound.size, sound.freq);
		}
		ALuint getBuffer() {
			return m_buffer;
		}
		~Sound() {
			alDeleteBuffers(1, &m_buffer);
		}
	private:
		ALuint m_buffer;
	};
}