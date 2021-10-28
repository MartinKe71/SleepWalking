#pragma once

#include "GL.hpp"
#include "Load.hpp"
#include "Scene.hpp"

#include <ft2build.h>
#include FT_FREETYPE_H

struct ShapeTextureProgram
{
	ShapeTextureProgram();
	~ShapeTextureProgram();

	GLuint program = 0;

	GLuint Position_vec4 = -1U;
	GLuint Color_vec4 = -1U;
	GLuint TexCoord_vec2 = -1U;

	struct Vertex
	{
		glm::vec2 Position;
		glm::u8vec4 Color;
		glm::vec2 TexCoord;
	};

	// Code from https://github.com/GenBrg/MarryPrincess/blob/master/Texture2DProgram.hpp
	struct BoxDrawable
	{
		GLuint vertex_array{ 0 };
		GLuint vertex_buffer{ 0 };
		glm::u8vec4 color{ 0xff, 0xff, 0xff, 0xff };
		
		void Clear();
	};

	static_assert(sizeof(Vertex) == 2 * 4 + 4 * 1 + 2 * 4);

	GLuint GetVao(GLuint vertex_buffer) const;
	GLuint GetTextureId(const FT_Bitmap& bitmap) const;
	void SetBoxHighlight(BoxDrawable& drawable, const glm::vec4& box) const;
	void ResetBoxHighlight(BoxDrawable& drawable, const glm::vec4& box) const;
	void DeleteTextureId(const GLuint texture_id) const;
	void SetBox(BoxDrawable& drawable, const glm::vec4& box, const glm::u8vec4 color) const;
	void DrawBox(const BoxDrawable& drawable) const;
	void DrawFont(const Vertex* vertices, const GLuint texture_id) const;
};

extern Load < ShapeTextureProgram > shape_texture_program;