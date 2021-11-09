#include "AudioSystem.hpp"
#include "Load.hpp"
#include "data_path.hpp"


Load < Sound::Sample > dusty_floor_sample(LoadTagDefault, []() -> Sound::Sample const * {
	return new Sound::Sample(data_path("dusty-floor.opus"));
});

Load < Sound::Sample > car_honk_sample(LoadTagDefault, []() -> Sound::Sample const * {
	return new Sound::Sample(data_path("car_honk.opus"));
});

Load < Sound::Sample > train_horn_sample(LoadTagDefault, []() -> Sound::Sample const* {
	return new Sound::Sample(data_path("train_horn.opus"));
});

Load < Sound::Sample > foot_steps_sample(LoadTagDefault, []() -> Sound::Sample const* {
	return new Sound::Sample(data_path("running_og-nr.opus"));
});

AudioSystem::AudioSystem()
{
	foot_steps_ps = Sound::loop_3D(*foot_steps_sample, 0.0f, glm::vec3(0), HALF_VOLUME_RADIUS);
}

//void AudioSystem::InitializeSound(const glm::vec3& pos)
//{
//	dusty_floor_ps->Sound::loop_3D (*dusty_floor_sample, 1.0f, pos, HALF_VOLUME_RADIUS);
//}

void AudioSystem::UpdateSoundPos(const glm::vec3& pos)
{
	//dusty_floor_ps->set_position(pos, 1.0f/60.0f);
	//car_honk_ps->set_position(pos, 1.0f/60.0f);
	//train_horn_ps->set_position(pos, 1.0f/60.0f);
}

void AudioSystem::PlayJumpSound( glm::vec3& pos, float volume) const
{
	Sound::play_3D(*car_honk_sample, volume, pos, HALF_VOLUME_RADIUS);
	//Sound::play_3D(*foot_steps_sample, volume, pos, HALF_VOLUME_RADIUS);
}

void AudioSystem::PlayFootStepsSound( glm::vec3& pos, float volume)
{
	foot_steps_ps->set_position(pos);
	foot_steps_ps->set_volume(volume);
	std::cout << "PLAYED_HERE" << volume << std::endl;
}

void AudioSystem::ResetPlayerSound()
{
	foot_steps_ps->set_volume(0.0f);
}
