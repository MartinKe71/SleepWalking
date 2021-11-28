#include "AudioSystem.hpp"
#include "Load.hpp"
#include "data_path.hpp"


Load < Sound::Sample > jump_sample(LoadTagDefault, []() -> Sound::Sample const* {
	return new Sound::Sample(data_path("jump.opus"));
});

Load < Sound::Sample > rotate_sample(LoadTagDefault, []() -> Sound::Sample const* {
	return new Sound::Sample(data_path("rotate.opus"));
});

Load < Sound::Sample > die_sample(LoadTagDefault, []() -> Sound::Sample const* {
	return new Sound::Sample(data_path("die.opus"));
});

Load < Sound::Sample > checkPoint_sample(LoadTagDefault, []() -> Sound::Sample const* {
	return new Sound::Sample(data_path("checkPoint.opus"));
});

Load < Sound::Sample > drag_sample(LoadTagDefault, []() -> Sound::Sample const* {
	return new Sound::Sample(data_path("drag.opus"));
});

Load < Sound::Sample > foot_steps_sample(LoadTagDefault, []() -> Sound::Sample const* {
	return new Sound::Sample(data_path("running_og-nr.opus"));
});

Load < Sound::Sample > time_stop_sample(LoadTagDefault, []() -> Sound::Sample const* {
	return new Sound::Sample(data_path("timestop.opus"));
});

AudioSystem::AudioSystem()
{
	foot_steps_ps = Sound::loop_3D(*foot_steps_sample, 0.0f, glm::vec3(0));
	time_stop_ps = Sound::loop_3D(*time_stop_sample, 0.0f, glm::vec3(0));
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
		Sound::play_3D(*jump_sample, volume, pos);
		break;

	case AudioSourceList::Rotate:
		Sound::play_3D(*rotate_sample, volume, pos);
		break;

	case AudioSourceList::Drag:
		Sound::play_3D(*drag_sample, volume, pos);
		break;

	case AudioSourceList::Die:
		Sound::play_3D(*die_sample, volume, pos);
		break;

	case AudioSourceList::CheckPoint:
		Sound::play_3D(*checkPoint_sample, volume, pos);
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

	case AudioSourceList::Timestop:
		time_stop_ps->set_position(pos);
		time_stop_ps->set_volume(volume);
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

	case AudioSourceList::Timestop:
		time_stop_ps->set_volume(0.0f);
		break;
	}
}

void AudioSystem::StopAllAudio()
{
	foot_steps_ps->set_volume(0.0f);
	time_stop_ps->set_volume(0.0f);
}
