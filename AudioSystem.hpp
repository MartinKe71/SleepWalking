#pragma once
#include "Inivar.hpp"
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

	void UpdateSoundPos(const glm::vec3& pos);

	void PlayJumpSound( glm::vec3& pos,  float volume = 1.0f) const;
	void PlayFootStepsSound( glm::vec3& pos,  float volume = 1.0f);

	void ResetPlayerSound();

	~AudioSystem() {}
	
private:
	AudioSystem();

	std::shared_ptr< Sound::PlayingSample > foot_steps_ps;
};