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
	/*delete NNScomputeShader;*/
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
	glDisable(GL_DEPTH_TEST);
	std::vector<FluidGameObject*>::const_iterator first;
	std::vector<FluidGameObject*>::const_iterator last;
	int* size = new int;

	
	gameWorld.GetFluidObjectIterators(first, last, size);

	ParticleProperties ParticlePropslocal[9];
	ParticleProperties ParticlePropsOut[9];
	int j = 0;
	for (auto i = first; i != last; ++i) {
		/*ParticlePropslocal[j].density = (*i)->AddParticleToStruct(*i, j)->density;
		/*ParticlePropslocal[j].mass = (*i)->AddParticleToStruct(*i, j)->mass;*/
		ParticlePropslocal[j].x = (*i)->AddParticleToStruct(*i, j)->x;
		ParticlePropslocal[j].y = (*i)->AddParticleToStruct(*i, j)->y;
		ParticlePropslocal[j].z = (*i)->AddParticleToStruct(*i, j)->z;
		ParticlePropslocal[j].empty = 0;
		j++;
	}

	int it;
	
	//  /*invalidate makes a big difference when re-writting*/
	
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, 3 * sizeof(ParticlePropslocal), &ParticlePropslocal[0], GL_DYNAMIC_COPY);
	
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssbo);
	
	
	NNScomputeShader->Bind();
	NNScomputeShader->Execute(9, 1, 1);
	NNScomputeShader->Unbind();
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	
	/*glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);*/
	GLfloat* ptr;
	ptr = (GLfloat*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
	int itt = 0;
	for (int i = 0; i < 36; i = i+ 4){
		
		ParticlePropsOut[itt].x = ptr[i];
		ParticlePropsOut[itt].y = ptr[i + 1];
		ParticlePropsOut[itt].z = ptr[i + 2];
		ParticlePropsOut[itt].empty = 0;
		itt++;
}
	int ittt = 0;
	
	j = 0;
	for (auto i = first; i != last; ++i) {
		(*i)->SetParticlePositions(*i, ParticlePropsOut[j]);
	}
	delete size;
	
	
};
void FluidPhysics::FluidCollision() {

};
void FluidPhysics::UpdateParticlePositions() {

};
void FluidPhysics::ClearFluids() {
}


