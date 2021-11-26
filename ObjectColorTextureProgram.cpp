#include "ObjectColorTextureProgram.hpp"

#include "gl_compile_program.hpp"
#include "gl_errors.hpp"

Load< ObjectColorTextureProgram > object_color_texture_program(LoadTagEarly);

ObjectColorTextureProgram::ObjectColorTextureProgram() {
	//Compile vertex and fragment shaders using the convenient 'gl_compile_program' helper function:
	program = gl_compile_program(
		//vertex shader:
		"#version 330\n"
		"uniform mat4 OBJECT_TO_CLIP;\n"
		"uniform mat4x3 OBJECT_TO_LIGHT;\n"
		"uniform mat3 NORMAL_TO_LIGHT;\n"
		"uniform mat4 Model;\n"
		"uniform vec3 Normal;\n"
		"in vec4 Position;\n"			
		"in vec2 TexCoord;\n"
		"out vec3 position;\n"
		"out vec3 normal;\n"
		"out vec2 texCoord;\n"
		"void main() {\n"
		"	gl_Position = OBJECT_TO_CLIP * Model * Position;\n"
		"	position = OBJECT_TO_LIGHT * Position;\n"
		"	normal = NORMAL_TO_LIGHT * Normal;\n"
		"	texCoord = TexCoord;\n"
		"}\n"
		,
		//fragment shader:
		"#version 330\n"
		"uniform sampler2D TEX;\n"
		"uniform int LIGHT_TYPE;\n"
		"uniform vec3 LIGHT_LOCATION;\n"
		"uniform vec3 LIGHT_DIRECTION;\n"
		"uniform vec3 LIGHT_ENERGY;\n"
		"uniform float LIGHT_CUTOFF;\n"
		"uniform vec4 Color;\n"
		"in vec3 position;\n"
		"in vec3 normal;\n"
		"in vec2 texCoord;\n"
		"out vec4 fragColor;\n"
		"void main() {\n"
		"	vec3 n = normalize(normal);\n"
		"	vec3 e;\n"
		"	if (LIGHT_TYPE == 0) { //point light \n"
		"		vec3 l = (LIGHT_LOCATION - position);\n"
		"		float dis2 = dot(l,l);\n"
		"		l = normalize(l);\n"
		"		float nl = max(0.0, dot(n, l)) / max(1.0, dis2);\n"
		"		e = nl * LIGHT_ENERGY;\n"
		"	} else if (LIGHT_TYPE == 1) { //hemi light \n"
		"		e = (dot(n,-LIGHT_DIRECTION) * 0.5 + 0.5) * LIGHT_ENERGY;\n"
		"	} else if (LIGHT_TYPE == 2) { //spot light \n"
		"		vec3 l = (LIGHT_LOCATION - position);\n"
		"		float dis2 = dot(l,l);\n"
		"		l = normalize(l);\n"
		"		float nl = max(0.0, dot(n, l)) / max(1.0, dis2);\n"
		"		float c = dot(l,-LIGHT_DIRECTION);\n"
		"		nl *= smoothstep(LIGHT_CUTOFF,mix(LIGHT_CUTOFF,1.0,0.1), c);\n"
		"		e = nl * LIGHT_ENERGY;\n"
		"	} else { //(LIGHT_TYPE == 3) //directional light \n"
		//"		e = vec3(1.0f);\n"
		"		e = max(0.0, dot(n,-LIGHT_DIRECTION)) * LIGHT_ENERGY;\n"
		"	}\n"
		"	vec4 albedo = texture(TEX, texCoord) * Color;\n"
		"	fragColor = vec4(e*albedo.rgb, albedo.a);\n"
		"}\n"
	);
	//As you can see above, adjacent strings in C/C++ are concatenated.
	// this is very useful for writing long shader programs inline.

	//look up the locations of vertex attributes:
	Position_vec4 = glGetAttribLocation(program, "Position");
	TexCoord_vec2 = glGetAttribLocation(program, "TexCoord");

	//look up the locations of uniforms:
	Normal_vec3 = glGetUniformLocation(program, "Normal");
	Color_vec4 = glGetUniformLocation(program, "Color");
	Model_mat4 = glGetUniformLocation(program, "Model");
	OBJECT_TO_CLIP_mat4 = glGetUniformLocation(program, "OBJECT_TO_CLIP");
	OBJECT_TO_LIGHT_mat4x3 = glGetUniformLocation(program, "OBJECT_TO_LIGHT");
	NORMAL_TO_LIGHT_mat3 = glGetUniformLocation(program, "NORMAL_TO_LIGHT");

	LIGHT_TYPE_int = glGetUniformLocation(program, "LIGHT_TYPE");
	LIGHT_LOCATION_vec3 = glGetUniformLocation(program, "LIGHT_LOCATION");
	LIGHT_DIRECTION_vec3 = glGetUniformLocation(program, "LIGHT_DIRECTION");
	LIGHT_ENERGY_vec3 = glGetUniformLocation(program, "LIGHT_ENERGY");
	LIGHT_CUTOFF_float = glGetUniformLocation(program, "LIGHT_CUTOFF");

	GLuint TEX_sampler2D = glGetUniformLocation(program, "TEX");

	//set TEX to always refer to texture binding zero:
	glUseProgram(program); //bind program -- glUniform* calls refer to this program now

	glUniform1i(TEX_sampler2D, 0); //set TEX to sample from GL_TEXTURE0

	glUseProgram(0); //unbind program -- glUniform* calls refer to ??? now
}

ObjectColorTextureProgram::~ObjectColorTextureProgram() {
	glDeleteProgram(program);
	program = 0;
}