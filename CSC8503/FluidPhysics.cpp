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
#include "PhysicsObject.h"
using namespace NCL;
using namespace Rendering;
using namespace CSC8503;

const int   idealHZ = 120;
const float idealDT = 1.0f / idealHZ;
 bool initial = false;
int realhz = idealHZ;
float realdt = idealDT;
const int PARTICLENUMBERS = 7200;
const bool solidtest = false;
FluidPhysics::FluidPhysics(GameWorld& world) : OGLRenderer(*Window::GetWindow()), gameWorld(world) {
	PositioncomputeShader = new OGLComputeShader("computeShader.comp"); // position commpute shader
	ForcecomputeShader = new OGLComputeShader("forcecomputeShader.comp");
	DensitycomputeShader = new OGLComputeShader("densitycomputeShader.comp");
	ConstraintcomputeShader = new OGLComputeShader("correctioncomputeShader.comp");
	SolidCollisioncomputeShader = new OGLComputeShader("solidcomputeShader.comp");
	GLvoid* p;
	float	dTOffset = 0;
	glGenBuffers(1, &ssbo);
	glGenBuffers(1, &solidsssbo);
	

	
	

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
UpdateParticlePositions();
ComputeConstraints();
ComputeDensity();
ComputeForces();
ComputeSolids();
} 


void FluidPhysics::UpdateParticlePositions() {
	std::vector<GameObject*>::const_iterator firstsolid;
	std::vector<GameObject*>::const_iterator lastsolid;
	
	std::vector<FluidGameObject*>::const_iterator first;
	std::vector<FluidGameObject*>::const_iterator last;
	ParticleProperties ParticlePropslocal[7200];
	ParticleProperties ParticlePropsOut[7200];
	ParticleProperties ParticlePropsSolids[3];
	ParticleProperties ParticlePropsSolidsOut[3];
	int* size = new int;
	gameWorld.GetFluidObjectIterators(first, last);
	gameWorld.GetObjectIterators(firstsolid, lastsolid);
	int j = 0;
	if (initial == false) {
		for (auto i = first; i != last; ++i) {
			
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
		j = 0;
		for (auto i = firstsolid; i != lastsolid; ++i) {
			int volumeType;
			if ((*i)->GetPhysicsObject()->GetInverseMass() > 0) {
				ParticlePropsSolids[j].x = (*i)->GetTransform().GetPosition().x;
				ParticlePropsSolids[j].y = (*i)->GetTransform().GetPosition().y;
				ParticlePropsSolids[j].z = (*i)->GetTransform().GetPosition().z;
				ParticlePropsSolids[j].correction_forcex = 0;

				if ((*i)->GetBoundingVolume()->type == VolumeType::AABB) { volumeType = 1; };
				if ((*i)->GetBoundingVolume()->type == VolumeType::OBB) { volumeType = 2; };
				if ((*i)->GetBoundingVolume()->type == VolumeType::Sphere) { volumeType = 4; };
				if ((*i)->GetBoundingVolume()->type == VolumeType::Mesh) { volumeType = 8; };
				if ((*i)->GetBoundingVolume()->type == VolumeType::Capsule) { volumeType = 16; };



				ParticlePropsSolids[j].velocityx = (*i)->GetPhysicsObject()->GetLinearVelocity().x;
				ParticlePropsSolids[j].velocityy = (*i)->GetPhysicsObject()->GetLinearVelocity().y;
				ParticlePropsSolids[j].velocityz = (*i)->GetPhysicsObject()->GetLinearVelocity().z;
				ParticlePropsSolids[j].correction_forcey = volumeType;

				ParticlePropsSolids[j].forcex = (*i)->GetTransform().GetScale().x;
				ParticlePropsSolids[j].forcey = (*i)->GetTransform().GetScale().y;
				ParticlePropsSolids[j].forcez = (*i)->GetTransform().GetScale().z;
				ParticlePropsSolids[j].correction_forcez = 0;


				ParticlePropsSolids[j].density = 0;
				ParticlePropsSolids[j].dempty = 1 / (*i)->GetPhysicsObject()->GetInverseMass();;
				ParticlePropsSolids[j].pressure = 0;
				ParticlePropsSolids[j].pempty = 0;
				j++;
			}
			

		}

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
		glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(ParticlePropslocal), &ParticlePropslocal[0], GL_DYNAMIC_COPY);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssbo);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, solidsssbo);
		glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(ParticlePropsSolids), &ParticlePropsSolids[0], GL_DYNAMIC_COPY);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, solidsssbo);
		
		
		initial = true;
	}
	if (solidtest == true) {
		j = 0;
		for (auto i = firstsolid; i != lastsolid; ++i) {
			if ((*i)->GetPhysicsObject()->GetInverseMass() > 0) {
				int volumeType;
				ParticlePropsSolids[j].x = (*i)->GetTransform().GetPosition().x;
				ParticlePropsSolids[j].y = (*i)->GetTransform().GetPosition().y;
				ParticlePropsSolids[j].z = (*i)->GetTransform().GetPosition().z;
				ParticlePropsSolids[j].correction_forcex = 0;

				if ((*i)->GetBoundingVolume()->type == VolumeType::AABB) { volumeType = 1; };
				if ((*i)->GetBoundingVolume()->type == VolumeType::OBB) { volumeType = 2; };
				if ((*i)->GetBoundingVolume()->type == VolumeType::Sphere) { volumeType = 4; };
				if ((*i)->GetBoundingVolume()->type == VolumeType::Mesh) { volumeType = 8; };
				if ((*i)->GetBoundingVolume()->type == VolumeType::Capsule) { volumeType = 16; };



				ParticlePropsSolids[j].velocityx = (*i)->GetPhysicsObject()->GetLinearVelocity().x;
				ParticlePropsSolids[j].velocityy = (*i)->GetPhysicsObject()->GetLinearVelocity().y;
				ParticlePropsSolids[j].velocityz = (*i)->GetPhysicsObject()->GetLinearVelocity().z;
				ParticlePropsSolids[j].correction_forcey = volumeType;

				ParticlePropsSolids[j].forcex = (*i)->GetTransform().GetScale().x;
				ParticlePropsSolids[j].forcey = (*i)->GetTransform().GetScale().y;
				ParticlePropsSolids[j].forcez = (*i)->GetTransform().GetScale().z;
				ParticlePropsSolids[j].correction_forcez = 0;


				ParticlePropsSolids[j].density = 0;
				ParticlePropsSolids[j].dempty = 1 / (*i)->GetPhysicsObject()->GetInverseMass();
				ParticlePropsSolids[j].pressure = 0;
				ParticlePropsSolids[j].pempty = 0;
				j++;
			}
		}
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, solidsssbo);
		glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(ParticlePropsSolids), &ParticlePropsSolids[0], GL_DYNAMIC_COPY);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, solidsssbo);
	}
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
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
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	
 	j = 0;
		for (auto i = first; i != last; ++i) {
		(*i)->SetParticlePositions(*i, ParticlePropsOut[j]);
		j++;
	}
		
	if (solidtest == true) {
			itt = 0;
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, solidsssbo);
			GLfloat* solidptr;
			solidptr = (GLfloat*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
			
			for (int i = 0; i < 16 * 3; i = i + 16) { //return solid from 
				
				ParticlePropsSolidsOut[itt].x = solidptr[i];
				ParticlePropsSolidsOut[itt].y = solidptr[i + 1];
				ParticlePropsSolidsOut[itt].z = solidptr[i + 2];
				itt++;
			}
			j = 0;
			for (auto i = firstsolid; i != lastsolid; ++i) {
				if ((*i)->GetPhysicsObject()->GetInverseMass() > 0) {
					Transform& transform = (*i)->GetTransform();
					transform.SetPosition(Vector3(ParticlePropsSolidsOut[j].x, ParticlePropsSolidsOut[j].y, ParticlePropsSolidsOut[j].z));
					j++;
				}
			}
			glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	}
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
void FluidPhysics::ComputeSolids() {
	SolidCollisioncomputeShader->Bind();
	SolidCollisioncomputeShader->Execute(PARTICLENUMBERS / 32, 1, 1);
	SolidCollisioncomputeShader->Unbind();
	glMemoryBarrier(GL_ALL_BARRIER_BITS);

};
void FluidPhysics::ClearFluids() {
	
}




