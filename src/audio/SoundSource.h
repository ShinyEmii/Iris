#pragma once
#include "Sound.h"
namespace Iris {
	namespace Audio {
		i16 getFreeAudioPoolID() {
			for (i16 i = 0; i < AUDIO_POOL_SIZE; i++) {
				if (!s_audioPoolUsage[i]) return i;
			}
			return -1;
		}
		void cleanAudioPool() {
			ALint state;
			for (i16 i = 0; i < AUDIO_POOL_SIZE; i++) {
				alGetSourcei(s_audioPool[i], AL_SOURCE_STATE, &state);
				if (state != AL_PLAYING) s_audioPoolUsage[i] = false;
			}
		}
		class SoundSource {
		public:
			SoundSource(f32 gain = 1.0f, bool loop = false, f32 pitch = 1.0f, glm::vec3 pos = { 0.0f, 0.0f, 0.0f }, glm::vec3 velocity = { 0.0f, 0.0f, 0.0f })
				: m_currentID(-1), m_gain(gain), m_loop(loop), m_pitch(pitch), m_pos(pos), m_vel(velocity) {}
			void play(Sound& sound) {
				ALint state;
				alGetSourcei(s_audioPool[m_currentID], AL_SOURCE_STATE, &state);
				if (m_currentID != -1 && state == AL_PLAYING) {
					ERROR("Can't play sound because it's currently playing! You can use forcePlay method to force it to play.");
					return;
				}
				m_currentID = getFreeAudioPoolID();
				if (m_currentID == -1) cleanAudioPool();
				m_currentID = getFreeAudioPoolID();
				if (m_currentID == -1) return;
				s_audioPoolUsage[m_currentID] = true;
				alSourcef(s_audioPool[m_currentID], AL_PITCH, m_pitch);
				alSourcef(s_audioPool[m_currentID], AL_GAIN, m_gain);
				alSource3f(s_audioPool[m_currentID], AL_POSITION, m_pos.x, m_pos.y, m_pos.z);
				alSourcei(s_audioPool[m_currentID], AL_LOOPING, m_loop);
				alSourcei(s_audioPool[m_currentID], AL_BUFFER, sound.getBuffer());
				alSourcePlay(s_audioPool[m_currentID]);
			}
			void forcePlay(Sound& sound) {
				stop();
				play(sound);
			}
			bool getLoop() {
				return m_loop;
			}
			void setLoop(bool loop) {
				m_loop = loop;
				if (m_currentID != -1) {
					alSourcei(s_audioPool[m_currentID], AL_LOOPING, m_loop);
				}
			}
			f32 getGain() {
				return m_gain;
			}
			void setGain(f32 gain) {
				m_gain = fmin(fmax(gain, 0.0f), 1.0f);
				if (m_currentID != -1) {
					alSourcef(s_audioPool[m_currentID], AL_GAIN, m_gain);
				}
			}
			f32 getPitch() {
				return m_pitch;
			}
			void setPitch(f32 pitch) {
				m_pitch = fmax(pitch, 0.0f), 1.0f;
				if (m_currentID != -1) {
					alSourcef(s_audioPool[m_currentID], AL_PITCH, m_pitch);
				}
			}
			glm::vec3 getPosition() {
				return m_pos;
			}
			void setPosition(glm::vec3 pos) {
				m_pos = pos;
				if (m_currentID != -1) {
					alSource3f(s_audioPool[m_currentID], AL_POSITION, m_pos.x, m_pos.y, m_pos.z);
				}
			}
			glm::vec3 getVelocity() {
				return m_vel;
			}
			void setVelocity(glm::vec3 vel) {
				m_vel = vel;
				if (m_currentID != -1) {
					alSource3f(s_audioPool[m_currentID], AL_VELOCITY, m_vel.x, m_vel.y, m_vel.z);
				}
			}
			void stop() {
				if (m_currentID == -1) {
					return;
				}
				alSourceStop(s_audioPool[m_currentID]);
				s_audioPoolUsage[m_currentID] = false;
				m_currentID = -1;
			}
		private:
			i16 m_currentID;
			f32 m_pitch, m_gain;
			glm::vec3 m_pos, m_vel;
			bool m_loop;
		};
	}
}