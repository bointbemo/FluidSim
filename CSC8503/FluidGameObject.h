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

		void SetParticlePositions(FluidGameObject* Particle, ParticleProperties ParticleProps) {
			Vector3 position(ParticleProps.x, ParticleProps.y,ParticleProps.z);
			Particle->transform.SetPosition(position);
		};
		void UpdateNeighbourhoodSize();
		
		bool GetNeighbourhoodSize(Vector3& outSize) const;
	protected:
		Transform			transform;
		CollisionVolume* boundingVolume;
		RenderObject* renderObject;
		ParticleProperties particleProperties;

		bool		isActive;
		int			worldID;
		std::string	name;

		Vector3 NeighbourhoodSize;

	};
}
