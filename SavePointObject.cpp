#include "SavePointObject.hpp"

SavePointObject::SavePointObject() {
}

SavePointObject::SavePointObject(float mass /* = 10.0f */, const glm::vec3& pos /* = glm::vec3(0.f) */, float w /* = 1.0f */, float h /* = 1.f */, const glm::vec3& vel /* = glm::vec3(0.f) */, bool isFixed /* = false */, const std::string& filename /* = "" */, float l /* = 100.f */) 
: GameObject(mass, pos, vel, isFixed, filename, l), width(w), height(h) {
	
	createVerts();
	prepareDraw();
}

SavePointObject::~SavePointObject() 
{
	glDeleteBuffers(1, &VBO_positions);
	glDeleteBuffers(1, &EBO);
	glDeleteVertexArrays(1, &VAO);
}

void SavePointObject::createVerts()
{
	vertex_positions = vector<glm::vec4>({
	glm::vec4(width,  height, 0.0f, 1.0f),  // top right
	glm::vec4(width, -height, 0.0f, 1.0f),  // bottom right
	glm::vec4(-width, -height, 0.0f, 1.0f),  // bottom left
	glm::vec4(-width,  height, 0.0f, 1.0f)  // top left 
		});

	vertex_texcoords = vector<glm::vec2>({
		glm::vec2(1.f,1.f),
		glm::vec2(1.f,0.f),
		glm::vec2(0.f,0.f),
		glm::vec2(0.f,1.f)
		});

	indices = std::vector<unsigned int>({ 0, 1, 2, 2, 3, 0 });
}

void SavePointObject::reset() {
	GameObject::reset();
	vertex_positions.clear();
}