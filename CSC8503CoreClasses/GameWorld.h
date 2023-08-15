#pragma once
#include <random>

#include "Ray.h"
#include "CollisionDetection.h"
#include "QuadTree.h"
#include "../../FluidModel/CSC8503/FluidGameObject.h"

namespace NCL {
		class Camera;
		using Maths::Ray;
	namespace CSC8503 {
		class GameObject;
		class Constraint;

		typedef std::function<void(GameObject*)> GameObjectFunc;
		typedef std::vector<GameObject*>::const_iterator GameObjectIterator;

		typedef std::function<void(FluidGameObject*)> FluidGameObjectFunc;
		typedef std::vector<FluidGameObject*>::const_iterator FluidGameObjectIterator;

		class GameWorld	{
		public:
			GameWorld();
			~GameWorld();

			void Clear();
			void ClearAndErase();

			void AddGameObject(GameObject* o);
			void RemoveGameObject(GameObject* o, bool andDelete = false);
			void AddFluidGameObject(FluidGameObject* o);
			void RemoveFluidGameObject(FluidGameObject* o, bool andDelete = false);
			void GetFluidObjectIterators(FluidGameObjectIterator& first, FluidGameObjectIterator& last, int* size)const;
			void AddConstraint(Constraint* c);
			void RemoveConstraint(Constraint* c, bool andDelete = false);

			Camera* GetMainCamera() const {
				return mainCamera;
			}

			void ShuffleConstraints(bool state) {
				shuffleConstraints = state;
			}

			void ShuffleObjects(bool state) {
				shuffleObjects = state;
			}

			bool Raycast(Ray& r, RayCollision& closestCollision, bool closestObject = false, GameObject* ignore = nullptr) const;

			virtual void UpdateWorld(float dt);

			void OperateOnContents(GameObjectFunc f);
			void OperateOnFluidContents(FluidGameObjectFunc f);
			
			void GetObjectIterators(
				GameObjectIterator& first,
				GameObjectIterator& last) const;

			void GetConstraintIterators(
				std::vector<Constraint*>::const_iterator& first,
				std::vector<Constraint*>::const_iterator& last) const;

			int GetWorldStateID() const {
				return worldStateCounter;
			}

		protected:
			std::vector<GameObject*> gameObjects;
			std::vector<FluidGameObject*>fluidObjects;
			std::vector<Constraint*> constraints;

			Camera* mainCamera;

			bool shuffleConstraints;
			bool shuffleObjects;
			int		worldIDCounter;
			int		worldStateCounter;
			int  worldfluidIDCounter;
			int worldFluidStateCounter;
		};
	}
}

