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

void AudioSystem::InitializeSound()
{
	dusty_floor_ps->set_position();
}

void AudioSystem::UpdateSoundPos(const glm::vec3& pos)
{
	dusty_floor_ps->set_position(pos, 1.0f/60.0f);
	car_honk_ps->set_position(pos, 1.0f/60.0f);
	train_horn_ps->set_position(pos, 1.0f/60.0f);
}