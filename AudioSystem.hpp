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

	void InitializeSound();
	void UpdateSoundPos();

	~AudioSystem() {}
	
private:
	AudioSystem() {}
	std::shared_ptr< Sound::PlayingSample > dusty_floor_ps;
	std::shared_ptr< Sound::PlayingSample > car_honk_ps;
	std::shared_ptr< Sound::PlayingSample > train_horn_ps;
};