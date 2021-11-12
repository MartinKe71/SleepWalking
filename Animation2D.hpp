/**
 * @ Author: Wenlin Mao
 * @ Create Time: 2021-11-11 21:05:14
 * @ Modified by: Wenlin Mao
 * @ Modified time: 2021-11-12 00:15:58
 * @ Description: Animation Interface
 */

#pragma once
#include "Inivar.hpp"
#include <vector>

using namespace std;

enum AnimationType
{
	Idle,
	Run,
	Jump
};

class Animation2D
{
public:
	Animation2D(const std::string& filename);

	~Animation2D();

	
	void play(GLuint VAO, GLuint VBO_texcoords, const glm::uvec2& texture_size, double deltatime);

	void set_animation_speed(float newspeed);


private:
	double anim_cursor;
	int current_frame_indx;
	int frames_count;
	float speed;

	vector<glm::vec4> frames;
};