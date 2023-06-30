#pragma once
#include "../debugger/Debugger.h"
#include "../utils/units.h"
#include "AudioPool.h"
namespace Iris {
	namespace Audio {
		class Sound {
		public:
			Sound(const char* src) {
				FILE* f;
				fopen_s(&f, src, "rb");
				if (f == nullptr) {
					ERROR("Failed to load sound from file {}!", src);
					return;
				}
				u8 channels;
				i32 freq;
				u8 bitsPerSample;
				ALsizei size;
				char buf[4];

				fread(buf, 4, 1, f);
				if (strncmp(buf, "RIFF", 4) != 0) {
					ERROR("Invalid WAV file! (no RIFF)");
					return;
				}

				fseek(f, 4, SEEK_CUR);

				fread(buf, 4, 1, f);
				if (strncmp(buf, "WAVE", 4) != 0) {
					ERROR("Invalid WAV file! (no WAVE)");
					return;
				}

				fseek(f, 10, SEEK_CUR);

				fread(buf, 2, 1, f);
				if (std::endian::native == std::endian::little)
					channels = *(u8*)&buf;
				else
					for (std::size_t i = 0; i < 2; ++i)
						reinterpret_cast<char*>(&channels)[3 - i] = buf[i];

				fread(buf, 4, 1, f);
				if (std::endian::native == std::endian::little)
					freq = *(i32*)&buf;
				else
					for (std::size_t i = 0; i < 4; ++i)
						reinterpret_cast<char*>(&freq)[3 - i] = buf[i];

				fseek(f, 6, SEEK_CUR);

				fread(buf, 2, 1, f);
				if (std::endian::native == std::endian::little)
					bitsPerSample = *(u8*)&buf;
				else
					for (std::size_t i = 0; i < 2; ++i)
						reinterpret_cast<char*>(&bitsPerSample)[3 - i] = buf[i];

				fread(buf, 4, 1, f);
				if (strncmp(buf, "data", 4) != 0) {
					ERROR("Invalid WAV file!");
					return;
				}

				fread(buf, 4, 1, f);
				if (std::endian::native == std::endian::little)
					size = *(ALsizei*)&buf;
				else
					for (std::size_t i = 0; i < 4; ++i)
						reinterpret_cast<char*>(&size)[3 - i] = buf[i];

				ALenum format = AL_FORMAT_MONO8;
				if (channels == 1 && bitsPerSample == 8)
					format = AL_FORMAT_MONO8;
				else if (channels == 1 && bitsPerSample == 16)
					format = AL_FORMAT_MONO16;
				else if (channels == 2 && bitsPerSample == 8)
					format = AL_FORMAT_STEREO8;
				else if (channels == 2 && bitsPerSample == 16)
					format = AL_FORMAT_STEREO16;

				char* byteData = new char[size];
				fread(byteData, size, 1, f);

				alGenBuffers(1, &m_buffer);
				alBufferData(m_buffer, format, byteData, size, freq);
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
}