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
 bool initial = false;
int realhz = idealHZ;
float realdt = idealDT;
const int PARTICLENUMBERS = 7200;
FluidPhysics::FluidPhysics(GameWorld& world) : OGLRenderer(*Window::GetWindow()), gameWorld(world) {
	PositioncomputeShader = new OGLComputeShader("computeShader.comp"); // position commpute shader
	ForcecomputeShader = new OGLComputeShader("forcecomputeShader.comp");
	DensitycomputeShader = new OGLComputeShader("densitycomputeShader.comp");
	ConstraintcomputeShader = new OGLComputeShader("correctioncomputeShader.comp");
	GLvoid* p;
	float	dTOffset = 0;
	glGenBuffers(1, &ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
	
	

};
FluidPhysics::~FluidPhysics() {
	delete PositioncomputeShader;
	delete ForcecomputeShader;
	delete DensitycomputeShader;
	delete ConstraintcomputeShader;
	delete p;
	glDeleteFramebuffers(1, &ssbo);
};

void FluidPhysics::Update(float dt) {
	GameTimer t;
	t.GetTimeDeltaSeconds(); 
	
	int iteratorCount = 0;
	dTOffset += dt; //We accumulate time delta here - there might be remainders from previous frame!
	//ClearFluids();
	UpdateParticlePositions();
	//if (dTOffset / realdt > 120) {
	  ComputeConstraints();

		ComputeDensity();
		
		ComputeForces();

	//}
		
		float updateTime = t.GetTimeDeltaSeconds();
		iteratorCount++;
	/*}*/
	
	
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
	ParticleProperties ParticlePropslocal[7200];
	ParticleProperties ParticlePropsOut[7200];
	int* size = new int;
	gameWorld.GetFluidObjectIterators(first, last);
	int j = 0;
	if (initial == false) {
		for (auto i = first; i != last; ++i) {
			/*ParticlePropslocal[j].density = (*i)->AddParticleToStruct(*i, j)->density;
			/*ParticlePropslocal[j].mass = (*i)->AddParticleToStruct(*i, j)->mass;*/
			ParticlePropslocal[j].x = (*i)->AddParticleToStruct(*i, j)->x;
			ParticlePropslocal[j].y = (*i)->AddParticleToStruct(*i, j)->y;
			ParticlePropslocal[j].z = (*i)->AddParticleToStruct(*i, j)->z;
			ParticlePropslocal[j].correction_forcex = 0;

			ParticlePropslocal[j].velocityx = 1;
			ParticlePropslocal[j].velocityy = 1;
			ParticlePropslocal[j].velocityz = 1;
			ParticlePropslocal[j].correction_forcey = 0;
				
			ParticlePropslocal[j].forcex = 0;
			ParticlePropslocal[j].forcey = 0;
			ParticlePropslocal[j].forcez = 0;
			ParticlePropslocal[j].correction_forcez = 0;
			
			ParticlePropslocal[j].density = 1000;
			ParticlePropslocal[j].dempty = 0;
			ParticlePropslocal[j].pressure = 1;
			ParticlePropslocal[j].pempty = 0;

			j++;
		}


		//glBufferStorage(GL_SHADER_STORAGE_BUFFER, sizeof(ParticlePropslocal), &ParticlePropslocal[0], GL_CLIENT_STORAGE_BIT | GL_MAP_READ_BIT );
		glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(ParticlePropslocal), &ParticlePropslocal[0], GL_DYNAMIC_COPY);

		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssbo);
		initial = true;
	}

	PositioncomputeShader->Bind();
	PositioncomputeShader->Execute(7200 /32, 1, 1);
	PositioncomputeShader->Unbind();
	int work_grp_cnt[3];
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &work_grp_cnt[0]);
	glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &work_grp_cnt[1]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &work_grp_cnt[2]);
	std::cout << "max work group size" << "x" << work_grp_cnt[0] << "y" << work_grp_cnt[1] << "z" << work_grp_cnt[2] << "\n";
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	
	GLfloat* ptr;
	ptr = (GLfloat*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
	
	int itt = 0;
	for (int i = 0; i < 16 * PARTICLENUMBERS; i = i + 16) { //return particles from 

		ParticlePropsOut[itt].x = ptr[i];
		ParticlePropsOut[itt].y = ptr[i + 1];
		ParticlePropsOut[itt].z = ptr[i + 2];
		
		ParticlePropsOut[itt].velocityx = ptr[i + 3];
		ParticlePropsOut[itt].velocityy = ptr[i + 4];
		ParticlePropsOut[itt].velocityz = ptr[i + 5];

		ParticlePropsOut[itt].forcex = ptr[i + 6];
		ParticlePropsOut[itt].forcey = ptr[i + 7];
		ParticlePropsOut[itt].forcez = ptr[i + 8];

		ParticlePropsOut[itt].density = ptr[i + 9];
		ParticlePropsOut[itt].dempty = ptr[i + 10];

		ParticlePropsOut[itt].pressure = ptr[i + 11];
	
		ParticlePropsOut[itt].pempty = ptr[i + 12];
		
		ParticlePropsOut[itt].correction_forcex = ptr[i + 13];
		ParticlePropsOut[itt].correction_forcey = ptr[i + 14];
		ParticlePropsOut[itt].correction_forcez = ptr[i + 15];
		itt++;
	}

	j = 0;
		for (auto i = first; i != last; ++i) {
		(*i)->SetParticlePositions(*i, ParticlePropsOut[j]);
		j++;
	}
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

};

void FluidPhysics::ComputeForces() {
	ForcecomputeShader->Bind();
	ForcecomputeShader->Execute(PARTICLENUMBERS /32, 1, 1);
	ForcecomputeShader->Unbind();
	glMemoryBarrier(GL_ALL_BARRIER_BITS);
};
void FluidPhysics::ComputeDensity() {
	DensitycomputeShader->Bind();
	DensitycomputeShader->Execute(PARTICLENUMBERS /32, 1, 1);
	DensitycomputeShader->Unbind();
	glMemoryBarrier(GL_ALL_BARRIER_BITS);

};
void FluidPhysics::ComputeConstraints() {
 ConstraintcomputeShader->Bind();
 ConstraintcomputeShader->Execute(PARTICLENUMBERS /32, 1, 1);
 ConstraintcomputeShader->Unbind();
	glMemoryBarrier(GL_ALL_BARRIER_BITS);

};
void FluidPhysics::ClearFluids() {
	
}
//GL_SHADER_STORAGE_BARRIER_BIT



