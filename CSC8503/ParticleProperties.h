
#pragma once
#include "Vector3.h"
#include "Maths.h"


namespace NCL {
	namespace CSC8503 {
		struct ParticleProperties {
			
			float x; //position
			float y;
			float z;
			float empty;
			
			float velocityx;//velocity
			float velocityy;
			float velocityz;
			float vempty;
			
			float forcex;//force
			float forcey;
			float forcez;
			float fempty;
			
			float density;//density
			float dempty;
			
			float pressure;//presure
			float pempty;

		};
	};
};