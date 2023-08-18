#define NOMINMAX
#include "FluidPhysics.h"
#include "GameTechRenderer.h"
#include "GameObject.h"
#include "RenderObject.h"
#include "Camera.h"
#include "TextureLoader.h"
#include "TutorialGame.h"
#include "Debug.h"
#include "Window.h"
#include <functional>
#include <algorithm>
using namespace NCL;
using namespace Rendering;
using namespace CSC8503;

const int   idealHZ = 120;
const float idealDT = 1.0f / idealHZ;
const int PARTICLE_NUM = 1024;
int realhz = idealHZ;
float realdt = idealDT;
bool initial = false;
FluidPhysics::FluidPhysics(GameWorld& world) : OGLRenderer(*Window::GetWindow()), gameWorld(world) {
	PositioncomputeShader = new OGLComputeShader("computeShader.comp"); // position commpute shader
	ForcecomputeShader = new OGLComputeShader("forcecomputeShader.comp");
	DensitycomputeShader = new OGLComputeShader("densitycomputeShader.comp");
	GLvoid* p;
	float	dTOffset = 0;
	//GLfloat* ptr = new GLfloat;
	
	/*glGenBuffers(2, &Densityssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, Densityssbo);
	glGenBuffers(3, &Forcessbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, Forcessbo);
	glGenBuffers(4, &Pressuressbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, Pressuressbo);
	glGenBuffers(5, &Velocityssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, Velocityssbo);*/
	
	ParticleProperties ParticlePropsOut[1024];
	//initialPositionData();
	

};
FluidPhysics::~FluidPhysics() {
	delete PositioncomputeShader;
	delete ForcecomputeShader;
	delete DensitycomputeShader;
	

	/*glDeleteFramebuffers(1, &Positionssbo);
	glDeleteFramebuffers(2, &Forcessbo);
	glDeleteFramebuffers(3, &Densityssbo);
	glDeleteFramebuffers(4, &Pressuressbo);
	glDeleteFramebuffers(5, &Velocityssbo);*/

};

void FluidPhysics::Update(float dt) {

	if (initial == false) {
		initialPositionData();
		initial = true;
	}
		
	
	GameTimer t;
	t.GetTimeDeltaSeconds(); 
	
	int iteratorCount = 0;
	dTOffset += dt; //We accumulate time delta here - there might be remainders from previous frame!
	if (dTOffset/ realdt > 120) {
	//NearestNeighbour();
	}
	UpdateParticlePositions();
		ComputeForces();
		//ComputeDensity();
		FluidCollision();
		
		float updateTime = t.GetTimeDeltaSeconds();
		iteratorCount++;
	/*}*/
	ClearFluids();
	
} 

void FluidPhysics::NearestNeighbour() {
	Neighbourhood.clear();
	QuadTree<FluidGameObject*> tree(Vector2(1024, 1024), 7, 6); // size must be 12, 

	std::vector<FluidGameObject*>::const_iterator first;
	std::vector<FluidGameObject*>::const_iterator last;
	gameWorld.GetFluidObjectIterators(first, last);
	for (auto i = first; i != last; ++i) {
		Vector3 halfSizes;
		if (!(*i)->GetNeighbourhoodSize(halfSizes))
			continue;
		Vector3 pos = (*i)->GetTransform().GetPosition();
		tree.Insert(*i, pos, halfSizes);
	}
	tree.OperateOnContents([&](std::list<QuadTreeEntry<FluidGameObject*>>& data) {
		CollisionDetection::FluidCollisionInfo info;
		for (auto i = data.begin(); i != data.end(); ++i) {
			for (auto j = std::next(i); j != data.end(); ++j) {
				// Is this pair of items in th ecollisions set
				// Is the same pair is in another quadtree node together
				info.a = std::min((*i).object, (*j).object);
				info.b = std::max((*i).object, (*j).object);
				Neighbourhood.insert(info);
			}
		}
		});
	
	
};
void FluidPhysics::SetNeighbourhoodLists() {
	
		 for (std::set < CollisionDetection::FluidCollisionInfo >::iterator
		 i = Neighbourhood.begin();
			 i != Neighbourhood.end(); ++i) {
			 CollisionDetection::FluidCollisionInfo info = *i;
			 if (CollisionDetection::CreateNeighbourhood(info.a, info.b, info)) {
				// ImpulseResolveCollision(*info.a, *info.b, info.point);
				// allCollisions.insert(info); // insert into our main set
				
			}
			
		
		
	}
}
void FluidPhysics::initialPositionData() {
	ParticleProperties ParticlePropslocal[1024];

	const int POSITIONSIZE = sizeof(float) * 4 * PARTICLE_NUM;
	const int VELOCITYSIZE = sizeof(float) * 4 * PARTICLE_NUM;
	const int FORCESIZE = sizeof(float) * 4 * PARTICLE_NUM;
	const int DENSITYSIZE = sizeof(float)  * PARTICLE_NUM;
	const int PRESSURESIZE = sizeof(float)  * PARTICLE_NUM;
	const int BUFFERSIZE = POSITIONSIZE + VELOCITYSIZE + FORCESIZE + DENSITYSIZE + PRESSURESIZE;

	const int POSITIONOFFSET = 0;
	const int VELOCITYOFFSET = POSITIONSIZE;
	const int FORCEOFFSET = VELOCITYOFFSET + VELOCITYSIZE;
	const int DENSITYOFFSET = FORCEOFFSET+ FORCESIZE;
	const int PRESSUREOFFSET = DENSITYOFFSET+DENSITYSIZE;
	

	glGenBuffers(1, &PARTICLESSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, PARTICLESSBO);
	std::vector<FluidGameObject*>::const_iterator first;
	std::vector<FluidGameObject*>::const_iterator last;


	//adding initial particle locations can be done once
	gameWorld.GetFluidObjectIterators(first, last);
	int j = 0;
	for (auto i = first; i != last; ++i) {

		ParticlePropslocal[j].x = (*i)->AddParticleToStruct(*i, j)->x;
		ParticlePropslocal[j].y = (*i)->AddParticleToStruct(*i, j)->y;
		ParticlePropslocal[j].z = (*i)->AddParticleToStruct(*i, j)->z;
		ParticlePropslocal[j].empty = 0;
		j++;
	}


	//glBufferStorage(GL_SHADER_STORAGE_BUFFER, sizeof(ParticlePropslocal), &ParticlePropslocal[0], GL_CLIENT_STORAGE_BIT | GL_MAP_READ_BIT );
	//glBufferStorage(GL_SHADER_STORAGE_BUFFER, BUFFERSIZE, &ParticlePropslocal[0], GL_DYNAMIC_STORAGE_BIT|GL_MAP_READ_BIT);
	glBufferData(GL_SHADER_STORAGE_BUFFER, BUFFERSIZE, &ParticlePropslocal[0], GL_DYNAMIC_COPY);
	//glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, PARTICLESSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);


	glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 0, PARTICLESSBO, POSITIONOFFSET, POSITIONSIZE);
	glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 1, PARTICLESSBO, VELOCITYOFFSET, VELOCITYSIZE);
	glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 2, PARTICLESSBO, FORCEOFFSET, FORCESIZE);
	glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 3, PARTICLESSBO, DENSITYOFFSET, DENSITYSIZE);
	glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 4, PARTICLESSBO, PRESSUREOFFSET, PRESSURESIZE);
	

};
void FluidPhysics::FluidCollision() {


};
void FluidPhysics::UpdateParticlePositions() {
	ParticleProperties ParticlePropsOut[1024];
	//glGenBuffers(1, &Positionssbo);
	//glBindBuffer(GL_SHADER_STORAGE_BUFFER, Positionssbo);
	std::vector<FluidGameObject*>::const_iterator first;
	std::vector<FluidGameObject*>::const_iterator last;

	int j = 0;
	//
	// //adding initial particle locations can be done once
	//gameWorld.GetFluidObjectIterators(first, last);
	////int j = 0;
	//for (auto i = first; i != last; ++i) {
	//	
	//	ParticlePropslocal[j].x = (*i)->AddParticleToStruct(*i, j)->x;
	//	ParticlePropslocal[j].y = (*i)->AddParticleToStruct(*i, j)->y;
	//	ParticlePropslocal[j].z = (*i)->AddParticleToStruct(*i, j)->z;
	//	ParticlePropslocal[j].empty = 0;
	//	j++;
	//}


	////glBufferStorage(GL_SHADER_STORAGE_BUFFER, sizeof(ParticlePropslocal), &ParticlePropslocal[0], GL_CLIENT_STORAGE_BIT | GL_MAP_READ_BIT );
	//glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(ParticlePropslocal), &ParticlePropslocal[0], GL_DYNAMIC_COPY);
	//glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, Positionssbo);

	//
	//glBindBuffer(GL_SHADER_STORAGE_BUFFER, PARTICLESSBO);
	PositioncomputeShader->Bind();
	PositioncomputeShader->Execute(1024, 1, 1);
	PositioncomputeShader->Unbind();
	glMemoryBarrier(GL_ALL_BARRIER_BITS);
	//glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	GLfloat* ptr;
	
	ptr = (GLfloat*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
	j = 0;
	for (int i = 0; i < 4 * 1024; i = i + 4) { //return particles from ssbo for rendering 
        ParticlePropsOut[j].x = ptr[i];
		ParticlePropsOut[j].y = ptr[i + 1];
		ParticlePropsOut[j].z = ptr[i + 2];
		ParticlePropsOut[j].empty = 0;
		j++;
	}
	//glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	j = 0;
	for (auto i = first; i != last; ++i) {
		(*i)->SetParticlePositions(*i, ParticlePropsOut[j]);
		j++;
	}
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

};
void FluidPhysics::ComputeDensity() {
	ParticleProperties ParticlePropslocal[1024];
	ParticleProperties ParticlePropsOut[1024];
	glGenBuffers(1, &Positionssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, Positionssbo);
	std::vector<FluidGameObject*>::const_iterator first;
	std::vector<FluidGameObject*>::const_iterator last;


	//adding initial particle locations can be done once
	gameWorld.GetFluidObjectIterators(first, last);
	int j = 0;
	for (auto i = first; i != last; ++i) {

		ParticlePropslocal[j].x = (*i)->AddParticleToStruct(*i, j)->x;
		ParticlePropslocal[j].y = (*i)->AddParticleToStruct(*i, j)->y;
		ParticlePropslocal[j].z = (*i)->AddParticleToStruct(*i, j)->z;
		ParticlePropslocal[j].empty = 0;
		j++;
	}

	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(ParticlePropslocal), &ParticlePropslocal[0], GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, Positionssbo);

	glGenBuffers(2, &Densityssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, Densityssbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(ParticlePropslocal), &ParticlePropslocal[0], GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, Positionssbo);

	DensitycomputeShader->Bind();
	DensitycomputeShader->Execute(1024, 1, 1);
	DensitycomputeShader->Unbind();
	
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	GLfloat* ptr;
	ptr = (GLfloat*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);

	j = 0;
	for (int i = 0; i < 4 * 1024; i = i + 4) { //return particles from ssbo for rendering 
		ParticlePropsOut[j].x = ptr[i];
		ParticlePropsOut[j].y = ptr[i + 1];
		ParticlePropsOut[j].z = ptr[i + 2];
		ParticlePropsOut[j].empty = 0;
		j++;
	}
	//glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	j = 0;
	for (auto i = first; i != last; ++i) {
		(*i)->SetParticlePositions(*i, ParticlePropsOut[j]);
		j++;
	}

};
void FluidPhysics::ComputeForces() {
};

void FluidPhysics::ClearFluids() {
}


