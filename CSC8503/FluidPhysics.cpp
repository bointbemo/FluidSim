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

FluidPhysics::FluidPhysics(GameWorld& world) : OGLRenderer(*Window::GetWindow()), gameWorld(world) {
	NNScomputeShader = new OGLComputeShader("computeShader.comp");
	GLvoid* p;
	glGenBuffers(1, &ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
	
	

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
	
	std::vector<FluidGameObject*>::const_iterator first;
	std::vector<FluidGameObject*>::const_iterator last;
	ParticleProperties ParticlePropslocal[1024];
	ParticleProperties ParticlePropsOut[1024];
	int* size = new int;
	gameWorld.GetFluidObjectIterators(first, last);
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


	//glBufferStorage(GL_SHADER_STORAGE_BUFFER, sizeof(ParticlePropslocal), &ParticlePropslocal[0], GL_CLIENT_STORAGE_BIT | GL_MAP_READ_BIT );
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(ParticlePropslocal), &ParticlePropslocal[0], GL_DYNAMIC_COPY);
	
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssbo);
	
	
	NNScomputeShader->Bind();
	NNScomputeShader->Execute(1024, 1, 1);
	NNScomputeShader->Unbind();
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	
	/*glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);*/
	GLfloat* ptr;
	ptr = (GLfloat*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY); 
	//ptr = (GLfloat*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, sizeof(ParticlePropslocal), GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
	int itt = 0;
	for (int i = 0; i < 4 * 1024; i = i + 4){ //return particles from 
		
		ParticlePropsOut[itt].x = ptr[i];
		ParticlePropsOut[itt].y = ptr[i + 1];
		ParticlePropsOut[itt].z = ptr[i + 2];
		ParticlePropsOut[itt].empty = 0;
		itt++;
}
	//glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	
	j = 0;
	for (auto i = first; i != last; ++i) {
		(*i)->SetParticlePositions(*i, ParticlePropsOut[j]);
		j++;
	}


	
	
};
void FluidPhysics::FluidCollision() {

};
void FluidPhysics::UpdateParticlePositions() {

};
void FluidPhysics::ClearFluids() {
}


