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
			GLuint PARTICLESSBO;

		protected:
			
			void NearestNeighbour();
			void SetNeighbourhoodLists();
			void FluidCollision();
			void UpdateParticlePositions();
			void ClearFluids();
			void ComputeForces();
			void ComputeDensity();
			void initialPositionData();
			std::set < CollisionDetection::FluidCollisionInfo > Neighbourhood;
			GameWorld& gameWorld;
			GLuint Positionssbo;
			GLuint Densityssbo;
			GLuint Forcessbo;
			GLuint Pressuressbo;
			GLuint Velocityssbo;
			// data input
			GLuint fluidbuffer;
			size_t fluidCount;
			//compute shader
			//GLfloat* ptr;
			OGLComputeShader* PositioncomputeShader;
			OGLComputeShader* ForcecomputeShader;
			OGLComputeShader* DensitycomputeShader;
			
			float	dTOffset;
			
		};
	}
}

