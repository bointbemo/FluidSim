#pragma once
#include "OGLRenderer.h"
//#include "OGLShader.h"
//#include "OGLTexture.h"
//#include "OGLMesh.h"
#include "OGLComputeShader.h"

#include "GameWorld.h"
using namespace NCL;
using namespace Rendering;
namespace NCL {
	class Maths::Vector3;
	class Maths::Vector4;
	namespace CSC8503 {
		class RenderObject;

		class FluidPhysics : public OGLRenderer {
		public:
			FluidPhysics(GameWorld& world);
			~FluidPhysics();
			void Update(float dt);
		protected:
			void NearestNeighbour();
			void FluidCollision();
			void UpdateParticlePositions();
			void ClearFluids();
			GameWorld& gameWorld;
			GLuint ssbo = 0;
			GLvoid* p;
			// data input
			GLuint fluidbuffer;
			size_t fluidCount;
			//compute shader
			OGLComputeShader* computeShader;
		};
	}
}

