#include "FluidGameObject.h"
#include "CollisionDetection.h"
#include "PhysicsObject.h"
#include "RenderObject.h"
#include "NetworkObject.h"

using namespace NCL::CSC8503;

FluidGameObject::FluidGameObject(string objectName) {
	name = objectName;
	worldID = -1;
	isActive = true;
	boundingVolume = nullptr;
	
	renderObject = nullptr;
	
}

FluidGameObject::~FluidGameObject() {
	delete boundingVolume;
	
	delete renderObject;
	
}
ParticleProperties* FluidGameObject::AddParticleToStruct( FluidGameObject* ParticleProps,int PARTICLEINDEX) {
	
	Transform& transformA = ParticleProps->GetTransform();
	Vector3 position = transformA.GetPosition(); // test values need changed
	Vector3 volume = transformA.GetScale();
	float mass = 1.0 ;
	float density = 0.5;
	/*ParticleProps->particleProperties.density = density;
	ParticleProps->particleProperties.volume = volume;
	ParticleProps->particleProperties.mass = mass;*/
	ParticleProps->particleProperties.x = position.x;
	ParticleProps->particleProperties.y = position.y;
	ParticleProps->particleProperties.z = position.z;
	ParticleProps->particleProperties.empty = 0;
	
	return &ParticleProps->particleProperties;

	
};

