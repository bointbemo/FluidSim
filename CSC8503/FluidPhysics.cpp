#include "FluidPhysics.h"
#include "GameTechRenderer.h"
#include "GameObject.h"
#include "RenderObject.h"
#include "Camera.h"
#include "TextureLoader.h"
using namespace NCL;
using namespace Rendering;
using namespace CSC8503;
struct shader_data_t
{
	float camera_position[4];
	float light_position[4];
	float light_diffuse[4];
} shader_data;

FluidPhysics::FluidPhysics(GameWorld& world) : OGLRenderer(*Window::GetWindow()), gameWorld(world) {
	computeShader = new OGLComputeShader("computeShader.comp");
	GLvoid* p;
	glGenBuffers(1, &ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(shader_data), &shader_data, GL_DYNAMIC_COPY);
	

};
void FluidPhysics::Update(float dt) {
	
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
	p = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
	memcpy(p, &shader_data, sizeof(shader_data));
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	computeShader->Bind();
	computeShader->Execute(1, 1, 1);
	computeShader->Unbind();
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
} 
FluidPhysics::~FluidPhysics() {
	delete computeShader;
	delete p;
};

