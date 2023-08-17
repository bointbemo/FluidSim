#pragma once
#include "OGLRenderer.h"
//#include "OGLShader.h"
//#include "OGLTexture.h"
//#include "OGLMesh.h"
#include "OGLComputeShader.h"

#include "GameWorld.h"
#include "TutorialGame.h"
using namespace NCL;
using namespace Rendering;
namespace NCL {
	class Maths::Vector3;
	class Maths::Vector4;
	extern struct ParticleProperties;

	namespace CSC8503 {
		class RenderObject;

		class FluidPhysics : public OGLRenderer {
		public:
			FluidPhysics(GameWorld& world);
			~FluidPhysics();
			void Update(float dt);
			
			struct ParticleProperty {
				NCL::Maths::Vector3 position;
				float density;
				float mass;
				NCL::Maths::Vector3 dimensions;
				NCL::Maths::Vector3 volume;

			};
			ParticleProperties ParticlePropsGlobal[9];
		protected:
			/*void AddParticleToStruct(FluidGameObject particle, int PARTICLEINDEX);*/
			void NearestNeighbour();
			void SetNeighbourhoodLists();
			void FluidCollision();
			void UpdateParticlePositions();
			void ClearFluids();
			void ComputeForces();
			void ComputeDensity();
			std::set < CollisionDetection::FluidCollisionInfo > Neighbourhood;
			GameWorld& gameWorld;
			GLuint ssbo;
			GLint bufMask = GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT;
			GLvoid* p;
			// data input
			GLuint fluidbuffer;
			size_t fluidCount;
			//compute shader
			OGLComputeShader* NNScomputeShader;
			float	dTOffset;
			
		};
	}
}

