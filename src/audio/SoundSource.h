#pragma once
#include <glm/glm.hpp>
#include "Sound.h"
#include "AudioPool.h"
namespace Iris {
	class SoundSource {
	public:
		SoundSource(float gain = 1.0f, bool loop = false, float pitch = 1.0f, glm::vec3 pos = { 0.0f, 0.0f, 0.0f }, glm::vec3 velocity = { 0.0f, 0.0f, 0.0f })
			: m_currentID(-1), m_gain(gain), m_loop(loop), m_pitch(pitch), m_pos(pos), m_vel(velocity) {}
		void play(std::string_view name) {
			Sound& sound = Iris::getSound(name).get();
			ALint state;
			alGetSourcei(Audio::s_audioPool[m_currentID], AL_SOURCE_STATE, &state);
			if (m_currentID != -1 && state == AL_PLAYING) {
				ERROR("Can't play sound because it's currently playing!");
				return;
			}
			m_currentID = Audio::getFreeAudioPoolID();
			if (m_currentID == -1) Audio::cleanAudioPool();
			m_currentID = Audio::getFreeAudioPoolID();
			if (m_currentID == -1) return;
			Audio::s_audioPoolUsage[m_currentID] = true;
			alSourcef(Audio::s_audioPool[m_currentID], AL_PITCH, m_pitch);
			alSourcef(Audio::s_audioPool[m_currentID], AL_GAIN, m_gain);
			alSource3f(Audio::s_audioPool[m_currentID], AL_POSITION, m_pos.x, m_pos.y, m_pos.z);
			alSourcei(Audio::s_audioPool[m_currentID], AL_LOOPING, m_loop);
			alSourcei(Audio::s_audioPool[m_currentID], AL_BUFFER, sound.getBuffer());
			alSourcePlay(Audio::s_audioPool[m_currentID]);
		}
		void forcePlay(std::string_view name) {
			stop();
			play(name);
		}
		bool getLoop() {
			return m_loop;
		}
		void setLoop(bool loop) {
			m_loop = loop;
			if (m_currentID != -1) {
				alSourcei(Audio::s_audioPool[m_currentID], AL_LOOPING, m_loop);
			}
		}
		float getGain() {
			return m_gain;
		}
		void setGain(float gain) {
			m_gain = fmin(fmax(gain, 0.0f), 1.0f);
			if (m_currentID != -1) {
				alSourcef(Audio::s_audioPool[m_currentID], AL_GAIN, m_gain);
			}
		}
		float getPitch() {
			return m_pitch;
		}
		void setPitch(float pitch) {
			m_pitch = fmax(pitch, 0.0f), 1.0f;
			if (m_currentID != -1) {
				alSourcef(Audio::s_audioPool[m_currentID], AL_PITCH, m_pitch);
			}
		}
		glm::vec3 getPosition() {
			return m_pos;
		}
		void setPosition(glm::vec3 pos) {
			m_pos = pos;
			if (m_currentID != -1) {
				alSource3f(Audio::s_audioPool[m_currentID], AL_POSITION, m_pos.x, m_pos.y, m_pos.z);
			}
		}
		glm::vec3 getVelocity() {
			return m_vel;
		}
		void setVelocity(glm::vec3 vel) {
			m_vel = vel;
			if (m_currentID != -1) {
				alSource3f(Audio::s_audioPool[m_currentID], AL_VELOCITY, m_vel.x, m_vel.y, m_vel.z);
			}
		}
		void stop() {
			if (m_currentID == -1) {
				return;
			}
			alSourceStop(Audio::s_audioPool[m_currentID]);
			Audio::s_audioPoolUsage[m_currentID] = false;
			m_currentID = -1;
		}
	private:
		int16_t m_currentID;
		float m_pitch, m_gain;
		glm::vec3 m_pos, m_vel;
		bool m_loop;
	};
}