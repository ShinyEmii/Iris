#pragma once
#include "../logger/Logger.h"
#include "../utility/ByteBuffer.h"
namespace Iris {
	namespace Audio {
		struct SoundData {
			bool valid;
			char* data;
			uint32_t size;
			uint16_t channels;
			int32_t freq;
			uint16_t bitsPerSample;
		};
		SoundData createSoundDataFromBuffer(Utility::ByteBuffer& buf) {
			uint16_t channels;
			int32_t freq;
			uint16_t bitsPerSample;
			uint32_t size;
			if (buf.getSize() == 0)
				return { 0 };
			buf.resetIndex();
			if (!buf.compare("RIFF", 4)) {
				ERROR("Invalid WAV file!");
				return { 0 };
			}
			buf.move(4);
			if (!buf.compare("WAVE", 4)) {
				ERROR("Invalid WAV file!");
				return { 0 };
			}
			buf.move(10);
			buf.getValue(&channels);
			buf.getValue(&freq);
			buf.move(6);
			buf.getValue(&bitsPerSample);
			if (!buf.compare("data", 4)) {
				ERROR("Invalid WAV file!");
				return { 0 };
			}
			buf.getValue(&size);
			if (buf.getSize() < size) {
				ERROR("Invalid WAV file!");
				return { 0 };
			}
			return { true, buf.getBuffer(), size, channels, freq, bitsPerSample };
		}
	}
}