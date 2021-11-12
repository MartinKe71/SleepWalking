/**
 * @ Author: Wenlin Mao
 * @ Create Time: 2021-11-11 21:05:07
 * @ Modified by: Wenlin Mao
 * @ Modified time: 2021-11-12 17:05:50
 * @ Description: Animation System
 */

#include "Animation2D.hpp"
#include "GLCall.hpp"

#include <string>
#include <sstream>

Animation2D::Animation2D(const std::string& filename)
	: anim_cursor(0),
	current_frame_indx(0),
	speed(0.05f)
{
	FILE* fp = nullptr;
	const int bufferlen = 255;
	char line[bufferlen];
    std::cout << filename << std::endl;
	fp = fopen(filename.c_str(), "r");

	if(fp == nullptr)
	{
		printf("erorr in reading animation file \n");
	}
	else
	{
		while (fgets(line, bufferlen, fp))
		{
			vector<int> result;

			stringstream ss(line);
			string token;
			while (getline(ss,token,','))
			{
				result.push_back(stoi(token));
			}

			glm::vec4 frame = glm::vec4(result[0], result[1], result[2], result[3]);
			frames.push_back(frame);
		}
	}

	frames_count = (int)frames.size();
    
	// for (glm::vec4& frame : frames)
	// 	std::cout << frame.x << " " << frame.y << " " << frame.z << " " << frame.w << std::endl;
	// exit(0);
	fclose(fp);
}

Animation2D::~Animation2D()
{
}

void Animation2D::play(GLuint VAO, GLuint VBO_texcoords, const glm::uvec2& texture_size, double deltatime)
{
	anim_cursor += deltatime;

	if(anim_cursor > speed)
	{
		current_frame_indx = (current_frame_indx + 1) % frames_count;
		anim_cursor = 0;
	}
    
	glm::vec4 frame = frames[current_frame_indx];

	// normalization
	frame.x /= texture_size.x;
	frame.y /= texture_size.y;
	frame.z /= texture_size.x;
	frame.w /= texture_size.y;
    
	vector<glm::vec2> uv = {
        glm::vec2(frame.x + frame.z, frame.y + frame.w),
        glm::vec2(frame.x + frame.z, frame.y),
		glm::vec2(frame.x, frame.y),
		glm::vec2(frame.x, frame.y + frame.w)
	};

    // std::cout << frame.x << " " << frame.y << " " << frame.z << " " << frame.w << std::endl;
    
	GLCall(glBindVertexArray(VAO));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, VBO_texcoords));

	// { //realocation for memory
	// 	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(glm::vec2), &uv[0], GL_DYNAMIC_DRAW);
	// }

	// best practice to send data to gpu memory..
	void* gpubuffer = nullptr;
	gpubuffer = glMapBufferRange(GL_ARRAY_BUFFER, 0, 4 * sizeof(glm::vec2), GL_MAP_WRITE_BIT| GL_MAP_INVALIDATE_BUFFER_BIT);
	memcpy(gpubuffer, uv.data(), 4 * sizeof(glm::vec2));
	GLCall(glUnmapBuffer(GL_ARRAY_BUFFER));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

void Animation2D::set_animation_speed(float newspeed)
{
	speed = newspeed;
}
