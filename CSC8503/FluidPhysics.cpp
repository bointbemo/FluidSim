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
struct SSBO
{
	
	float mass[4];
	Vector3 position[4];



} ShaderBuff;
FluidPhysics::FluidPhysics(GameWorld& world) : OGLRenderer(*Window::GetWindow()), gameWorld(world) {
	computeShader = new OGLComputeShader("computeShader.comp");
	GLvoid* p;
	glGenBuffers(1, &ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(shader_data), &shader_data, GL_DYNAMIC_COPY);
	

};
FluidPhysics::~FluidPhysics() {
	delete computeShader;
	delete p;
};
void FluidPhysics::Update(float dt) {
	GameTimer t;
	t.GetTimeDeltaSeconds(); 

	NearestNeighbour();
	FluidCollision();
	UpdateParticlePositions();
	ClearFluids();
	
} 
void FluidPhysics::NearestNeighbour() {
	
	p = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
	memcpy(p, &shader_data, sizeof(shader_data));
	glGenBuffers(1, &ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(ssbo), NULL, GL_STATIC_DRAW);

	GLint bufMask = GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT; // invalidate makes a ig difference when re-writting
	SSBO* silly;
	silly = (SSBO*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0,sizeof(ssbo), bufMask);

	computeShader->Bind();
	computeShader->Execute(1, 1, 1);
	computeShader->Unbind();
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
};
void FluidPhysics::FluidCollision() {

};
void FluidPhysics::UpdateParticlePositions() {

};
void FluidPhysics::ClearFluids() {
}


