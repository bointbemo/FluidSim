#include "FluidPhysics.h"
#include "GameTechRenderer.h"
#include "GameObject.h"
#include "RenderObject.h"
#include "Camera.h"
#include "TextureLoader.h"
#include "TutorialGame.h"
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
	NNScomputeShader = new OGLComputeShader("computeShader.comp");
	GLvoid* p;
	glGenBuffers(1, &ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(shader_data), &shader_data, GL_DYNAMIC_COPY);
	

};
FluidPhysics::~FluidPhysics() {
	delete NNScomputeShader;
	delete p;
	glDeleteFramebuffers(1, &ssbo);
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
	NNScomputeShader->Bind();
	p = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
	memcpy(p, &shader_data, sizeof(shader_data));
	
	
	std::vector<FluidGameObject*>::const_iterator first;
	std::vector<FluidGameObject*>::const_iterator last;
	int* size = new int;
	
	gameWorld.GetFluidObjectIterators(first, last, size);
	
	ParticleProperties PartyP[9];
	ParticleProperties* ParticlePropslocal;
	
	ParticlePropslocal = (ParticleProperties*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, 3 * sizeof(ParticleProperties), bufMask);
	
	int j = 0;
	
	for (auto i = first; i != last; ++i) {
		ParticlePropslocal[j].density = (*i)->AddParticleToStruct(*i, j)->density;
		ParticlePropslocal[j].position = (*i)->AddParticleToStruct(*i, j)->position;
		
		j++;
	}
	  //invalidate makes a big difference when re-writting

	
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, ssbo);
	NNScomputeShader->Execute(1, 1, 1);
	NNScomputeShader->Unbind();
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	
	delete size;
	
};
void FluidPhysics::FluidCollision() {

};
void FluidPhysics::UpdateParticlePositions() {

};
void FluidPhysics::ClearFluids() {
}


