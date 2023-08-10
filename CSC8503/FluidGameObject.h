#pragma once
#include "Transform.h"
#include "CollisionVolume.h"
#include "ParticleProperties.h"

namespace NCL::CSC8503 {
	class NetworkObject;
	class RenderObject;
	class FluidPhysics;

	class FluidGameObject {
	public:
		FluidGameObject(std::string name = "");
		
		~FluidGameObject();

		void SetBoundingVolume(CollisionVolume* vol) {
			boundingVolume = vol;
		}
	
		const CollisionVolume* GetBoundingVolume() const {
			return boundingVolume;
		}

		bool IsActive() const {
			return isActive;
		}

		Transform& GetTransform() {
			return transform;
		}

		RenderObject* GetRenderObject() const {
			return renderObject;
		}
		void SetWorldID(int newID) {
			worldID = newID;
		}


		void SetRenderObject(RenderObject* newObject) {
			renderObject = newObject;
		}

		ParticleProperties* AddParticleToStruct(FluidGameObject* ParticleProps, int PARTICLEINDEX);
		
	protected:
		Transform			transform;
		ParticleProperties particleProperties;
		CollisionVolume* boundingVolume;
		
		RenderObject* renderObject;
		

		bool		isActive;
		int			worldID;
		std::string	name;

		Vector3 broadphaseAABB;
	};
}
