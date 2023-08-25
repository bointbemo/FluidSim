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
			void ComputeConstraints();
			void ComputeSolids();
			std::set < CollisionDetection::FluidCollisionInfo > Neighbourhood;
			GameWorld& gameWorld;
			GLuint ssbo;
			GLuint solidsssbo;

			GLint bufMask = GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT;
			GLvoid* p;
			// data input
			GLuint fluidbuffer;
			size_t fluidCount;
			//compute shader
			OGLComputeShader* PositioncomputeShader;
			OGLComputeShader* ForcecomputeShader;
			OGLComputeShader* DensitycomputeShader;
			OGLComputeShader* ConstraintcomputeShader;
			OGLComputeShader* SolidCollisioncomputeShader;
			float	dTOffset;
			
		};
	}
}

