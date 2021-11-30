#pragma once
#include "Inivar.hpp"
#include "PlayerStats.hpp"
#include "Scene.hpp"
#include "Sound.hpp"

class AudioSystem {
public:
	AudioSystem(AudioSystem const&) = delete;
	void operator=(AudioSystem const&) = delete;

	static AudioSystem& Instance()
	{
		static AudioSystem audio_system;
		return audio_system;
	}

	void PlayShortAudio(const AudioSourceList sound, float volume = 1.0f, const glm::vec3& pos = PlayerStats::Instance().player1Pos);
	void PlayLongAudio(const AudioSourceList sound, float volume = 1.0f, const glm::vec3& pos = PlayerStats::Instance().player1Pos);
	void StopLongAudio(const AudioSourceList sound);
	void StopAllAudio();

	~AudioSystem();
	
private:
	AudioSystem();
	std::shared_ptr< Sound::PlayingSample > foot_steps_ps;
	std::shared_ptr< Sound::PlayingSample > time_stop_ps;
	std::shared_ptr< Sound::PlayingSample > bgm_ps;
};