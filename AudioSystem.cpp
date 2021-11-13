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

AudioSystem::~AudioSystem()
{
	Sound::stop_all_samples();
}

void AudioSystem::PlayShortAudio(const AudioSourceList sound, float volume /* = 1.0f */, const glm::vec3& pos /* = PlayerStats::Instance().player1Pos */)
{
	switch (sound)
	{
	default:
		break;

	case AudioSourceList::Jump:
		Sound::play_3D(*car_honk_sample, volume, pos, HALF_VOLUME_RADIUS);
		break;
	}
}

void AudioSystem::PlayLongAudio(const AudioSourceList sound, float volume /* = 1.0f */, const glm::vec3& pos /* = PlayerStats::Instance().player1Pos */)
{
	switch (sound)
	{
	default:
		break;

	case AudioSourceList::Footsteps:
		foot_steps_ps->set_position(pos);
		foot_steps_ps->set_volume(volume);
		break;
	}
}

void AudioSystem::StopLongAudio(const AudioSourceList sound)
{
	switch (sound)
	{
	default:
		break;

	case AudioSourceList::Footsteps:
		foot_steps_ps->set_volume(0.0f);
		break;
	}
}

void AudioSystem::StopAllAudio()
{
	foot_steps_ps->set_volume(0.0f);
}
