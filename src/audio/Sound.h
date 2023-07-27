#pragma once
#include "../debugger/Debugger.h"
#include "AudioPool.h"
#include "../assets/Package.h"
namespace Iris {
	namespace Audio {
		class Sound {
		public:
			Sound() : m_buffer(0) {};
			Sound(char* data, size_t dataSize, bool free = true) : m_buffer(0) {
				if (dataSize < 40) {
					ERROR("Invalid WAV file! (not enough data)");
					return;
				}
				u8 channels;
				i32 freq;
				u8 bitsPerSample;
				ALsizei size;
				if (strncmp(data, "RIFF", 4) != 0) {
					ERROR("Invalid WAV file! (no RIFF)");
					return;
				}
				if (strncmp(data + 8, "WAVE", 4) != 0) {
					ERROR("Invalid WAV file! (no WAVE)");
					return;
				}
				if (std::endian::native == std::endian::little)
					channels = *(u8*)(data + 22);
				else
					for (size_t i = 0; i < 2; ++i)
						reinterpret_cast<char*>(&channels)[3 - i] = (data + 22)[i];
				if (std::endian::native == std::endian::little)
					freq = *(i32*)(data + 24);
				else
					for (size_t i = 0; i < 4; ++i)
						reinterpret_cast<char*>(&freq)[3 - i] = (data + 24)[i];
				if (std::endian::native == std::endian::little)
					bitsPerSample = *(u8*)(data + 28);
				else
					for (size_t i = 0; i < 2; ++i)
						reinterpret_cast<char*>(&bitsPerSample)[3 - i] = (data + 28)[i];
				if (strncmp(data + 36, "data", 4) != 0) {
					ERROR("Invalid WAV file!");
					return;
				}
				if (std::endian::native == std::endian::little)
					size = *(ALsizei*)(data + 40);
				else
					for (size_t i = 0; i < 4; ++i)
						reinterpret_cast<char*>(&size)[3 - i] = (data + 40)[i];
				if (dataSize < size) {
					ERROR("Invalid WAV file! (not enough data)");
					return;
				}
				ALenum format = AL_FORMAT_MONO8;
				if (channels == 1 && bitsPerSample == 8)
					format = AL_FORMAT_MONO8;
				else if (channels == 1 && bitsPerSample == 16)
					format = AL_FORMAT_MONO16;
				else if (channels == 2 && bitsPerSample == 8)
					format = AL_FORMAT_STEREO8;
				else if (channels == 2 && bitsPerSample == 16)
					format = AL_FORMAT_STEREO16;
				char* byteData = data + 40;
				alGenBuffers(1, &m_buffer);
				alBufferData(m_buffer, format, byteData, size, freq);
				if (free) delete[] data;
				INFO("Sound loaded.");
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
		Sound loadSound(const char* src) {
			INFO("Loading Sound from file \"{}\".", src);
			FILE* f;
			fopen_s(&f, src, "rb");
			if (f == nullptr) {
				ERROR("Failed to load sound from file {}!", src);
				return Sound();
			}
			size_t size;
			fseek(f, 0, SEEK_END);
			size = ftell(f);
			fseek(f, 0, SEEK_SET);
			char* data = new char[size] {0};
			fread(data, size, 1, f);
			fclose(f);
			return Sound(data, size);
		}
		Sound loadSound(Assets::Package& package, const char* name) {
			INFO("Loading Sound from package \"{}\" with name \"{}\".", package.getName(), name);
			Assets::Asset sound = package.getAsset(name);
			if (sound.size == 0) return Sound();
			return Sound(sound.data, sound.size, false);
		}
	}
}