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