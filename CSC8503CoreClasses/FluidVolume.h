
#pragma once
#include "CollisionVolume.h"

namespace NCL {
    class FluidVolume : public CollisionVolume
    {
    public:
        FluidVolume(float halfHeight, float radius) {
            this->halfHeight = halfHeight;
            this->radius = radius;
            this->type = VolumeType::Fluid;
        };
        ~FluidVolume() {

        }
        float GetRadius() const {
            return radius;
        }

        float GetHalfHeight() const {
            return halfHeight;
        }

    protected:
        float radius;
        float halfHeight;
    };
}

