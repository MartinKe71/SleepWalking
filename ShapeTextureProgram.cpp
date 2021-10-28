#include "ShapeTextureProgram.hpp"
#include <signal.h>
#include "gl_compile_program.hpp"
#include "gl_errors.hpp"

#ifdef _MSC_VER
#define DEBUG_BREAK __debugbreak()
#elif __APPLE__
#define DEBUG_BREAK __builtin_trap()
#else
#define DEBUG_BREAK raise(SIGTRAP)
#endif

#define ASSERT(x) if (!(x)) DEBUG_BREAK;
#define GLCall(x) GLClearError();\
	x;\
	ASSERT(GLLogCall(#x, __FILE__, __LINE__))


#define TEXT_DEFAULT_COLOR glm::u8vec4(0x00, 0x00, 0x00, 0xff)
#define TEXT_HIGHLIGHT_COLOR glm::u8vec4(0xff)
#define BOX_HIGHLIGHT_COLOR glm::u8vec4(0xde, 0xf2, 0x37, 0xff)
#define BOX_DEFAULT_COLOR glm::u8vec4(0x77, 0x93, 0x7c, 0xff)


static void GLClearError()
{
	while (glGetError() != GL_NO_ERROR);
}

static bool GLLogCall(const char* function, const char* file, int line)
{
	while (GLenum error = glGetError())
	{
		std::cout << "[OpenGL Error] ( " << error << " ) : " << function << " " << file << ": " << line << std::endl;
		return false;
	}
	return true;
}

Load< ShapeTextureProgram > shape_texture_program(LoadTagEarly);

static unsigned int default_index_buffer_content[]{0, 1, 2, 1, 2, 3};
static GLuint default_index_buffer{ 0 };
static GLuint default_texture{ 0 };

static Load<void> load_default_index_buffer(LoadTagEarly, []() {
	GLCall(glGenBuffers(1, &default_index_buffer));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, default_index_buffer));
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), default_index_buffer_content, GL_STATIC_DRAW));
});

static Load<void> load_default_texture(LoadTagEarly, []() {
	GLCall(glGenTextures(1, &default_texture));
	GLCall(glBindTexture(GL_TEXTURE_2D, default_texture));
	std::vector< glm::u8vec4 > tex_data(1, glm::u8vec4(0xff));
	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex_data.data()));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
});

ShapeTextureProgram::ShapeTextureProgram() 
{
	GLCall(program = gl_compile_program(
		//vertex shader:
		"#version 330\n"
		"in vec4 Position;\n"
		"in vec4 Color;\n"
		"in vec2 TexCoord;\n"
		"out vec4 color;\n"
		"out vec2 texCoord;\n"
		"void main() {\n"
		"	gl_Position = Position;\n"
		"	color = Color;\n"
		"	texCoord = TexCoord;\n"
		"}\n"
		,
		//fragment shader:
		"#version 330\n"
		"uniform sampler2D TEX;\n"
		"in vec4 color;\n"
		"in vec2 texCoord;\n"
		"out vec4 fragColor;\n"
		"void main() {\n"
		"	fragColor = texture(TEX, texCoord) * color;\n"
		"}\n"
	));

	GLCall(Position_vec4 = glGetAttribLocation(program, "Position"));
	GLCall(Color_vec4 = glGetAttribLocation(program, "Color"));
	GLCall(TexCoord_vec2 = glGetAttribLocation(program, "TexCoord"));

	GLCall(GLuint TEX_sampler2D = glGetUniformLocation(program, "TEX"));

	GLCall(glUseProgram(program));

	GLCall(glUniform1i(TEX_sampler2D, 0));

	GLCall(glUseProgram(0));
}

ShapeTextureProgram::~ShapeTextureProgram()
{
	GLCall(glDeleteProgram(program));
	program = 0;
}

GLuint ShapeTextureProgram::GetVao(GLuint vertex_buffer) const 
{
	GLuint vao;
	GLCall(glGenVertexArrays(1, &vao));
	GLCall(glBindVertexArray(vao));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer));

	GLCall(glEnableVertexAttribArray(Position_vec4));
	GLCall(glVertexAttribPointer(Position_vec4, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, Position)));
	
	GLCall(glEnableVertexAttribArray(Color_vec4));
	GLCall(glVertexAttribPointer(Color_vec4, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (const void*)offsetof(Vertex, Color)));

	GLCall(glEnableVertexAttribArray(TexCoord_vec2));
	GLCall(glVertexAttribPointer(TexCoord_vec2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, TexCoord)));

	return vao;
}

void ShapeTextureProgram::BoxDrawable::Clear() 
{
	if (vertex_buffer)
	{
		GLCall(glDeleteBuffers(1, &vertex_buffer));
		vertex_buffer = 0;
	}

	if (vertex_array)
	{
		GLCall(glDeleteVertexArrays(1, &vertex_array));
		vertex_array = 0;
	}
}

void ShapeTextureProgram::SetBox(BoxDrawable& drawable, const glm::vec4& box, const glm::u8vec4 color) const 
{
	drawable.Clear();

	GLCall(glGenBuffers(1, &drawable.vertex_buffer));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, drawable.vertex_buffer));
	Vertex vertices[]{
		{{box[0], box[1]}, color, {0, 1}},
		{{box[2], box[1]}, color, {1, 1}},
		{{box[0], box[3]}, color, {0, 0}},
		{{box[2], box[3]}, color, {1, 0}}
	};
	drawable.color = color;
	GLCall(glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(Vertex), static_cast<const void*>(vertices), GL_STATIC_DRAW));
	drawable.vertex_array = shape_texture_program->GetVao(drawable.vertex_buffer);
}

// Code from https://github.com/GenBrg/MarryPrincess/blob/master/Texture2DProgram.cpp
void ShapeTextureProgram::DrawBox(const BoxDrawable& drawable) const
{
	GLCall(glUseProgram(program));
	GLCall(glBindVertexArray(drawable.vertex_array));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, default_index_buffer));
	GLCall(glActiveTexture(GL_TEXTURE0));
	GLCall(glBindTexture(GL_TEXTURE_2D, default_texture));
	GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, static_cast<const void *>(0)));
}

GLuint ShapeTextureProgram::GetTextureId(const FT_Bitmap& bitmap) const
{
	GLuint texture_id;

	GLCall(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
	GLCall(glGenTextures(1, &texture_id));
	GLCall(glBindTexture(GL_TEXTURE_2D, texture_id));

	GLCall(glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RGBA,
		bitmap.width,
		bitmap.rows,
		0,
		GL_RED,
		GL_UNSIGNED_BYTE,
		bitmap.buffer
	));

	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, GL_ONE));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_ONE));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_ONE));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_RED));

	return texture_id;
}

void ShapeTextureProgram::DeleteTextureId(const GLuint texture_id) const 
{
	GLCall(glDeleteTextures(1, &texture_id));
}

void ShapeTextureProgram::DrawFont(const Vertex* vertices, const GLuint texture_id) const
{
	GLCall(glUseProgram(program));

	GLuint vertex_buffer, vertex_array;

	GLCall(glGenBuffers(1, &vertex_buffer));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer));
	GLCall(glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(Vertex), static_cast<const void*>(vertices), GL_STATIC_DRAW));

	vertex_array = GetVao(vertex_buffer);
	GLCall(glBindVertexArray(vertex_array));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, default_index_buffer));
	GLCall(glActiveTexture(GL_TEXTURE0));
	GLCall(glBindTexture(GL_TEXTURE_2D, texture_id));

	GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, static_cast<const void*>(0)));

	GLCall(glDeleteBuffers(1, &vertex_buffer));
	GLCall(glDeleteVertexArrays(1, &vertex_array));
}

void ShapeTextureProgram::SetBoxHighlight(BoxDrawable& drawable, const glm::vec4& box) const
{
	if (drawable.color != BOX_HIGHLIGHT_COLOR)
		SetBox(drawable, box, BOX_HIGHLIGHT_COLOR);

	DrawBox(drawable);
}

void ShapeTextureProgram::ResetBoxHighlight(BoxDrawable& drawable, const glm::vec4& box) const
{
	if (drawable.color != BOX_DEFAULT_COLOR)
		SetBox(drawable, box, BOX_DEFAULT_COLOR);
	
	DrawBox(drawable);
}