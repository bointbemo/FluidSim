
#pragma once
#include "Vector3.h"
#include "Maths.h"


namespace NCL {
	namespace CSC8503 {
		struct ParticleProperties {
			NCL::Maths::Vector3 position;
			float density;
			float mass;
			NCL::Maths::Vector3 dimensions;
			NCL::Maths::Vector3 volume;

		};
	};
};