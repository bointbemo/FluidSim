
#pragma once
#include "Vector3.h"
#include "Maths.h"


namespace NCL {
	namespace CSC8503 {
		struct ParticleProperties {
			
			float x; //position
			float y;
			float z;
			
			
			float velocityx;//velocity
			float velocityy;
			float velocityz;
		
			float forcex;//force
			float forcey;
			float forcez;
			
			
			float density;//density
			float dempty;
			
			float pressure;//presure
			float pempty; //llambda

			float correction_forcex;
			float correction_forcey;
			float correction_forcez;

			//float correction_forcex; // correction force for density/ viscosity contraints
			//float correction_forcey;
			//float correction_forcez;
			//float correction_forceempty;
		};
	};
};