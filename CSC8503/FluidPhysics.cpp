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

int realhz = idealHZ;
float realdt = idealDT;

FluidPhysics::FluidPhysics(GameWorld& world) : OGLRenderer(*Window::GetWindow()), gameWorld(world) {
	NNScomputeShader = new OGLComputeShader("computeShader.comp");
	GLvoid* p;
	float	dTOffset = 0;
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
	
	int iteratorCount = 0;
	dTOffset += dt; //We accumulate time delta here - there might be remainders from previous frame!
	if (dTOffset/ realdt > 120) {
	//NearestNeighbour();
	}
		ComputeForces();
		ComputeDensity();
		FluidCollision();
		UpdateParticlePositions();
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
//void FluidPhysics::ImpulseResolveCollision(GameObject& a, GameObject& b, CollisionDetection::ContactPoint& p) const {
//}
void FluidPhysics::FluidCollision() {

};
void FluidPhysics::UpdateParticlePositions() {

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
	for (int i = 0; i < 4 * 1024; i = i + 4) { //return particles from 

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

void FluidPhysics::ComputeForces() {
};
void FluidPhysics::ComputeDensity() {
};
void FluidPhysics::ClearFluids() {
}


