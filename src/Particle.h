#ifndef PARTICLE_H
#define PARTICLE_H

#include "irrlicht.h"
using namespace irr;

class Particle
{
public:
	Particle();

	core::vector3df position;
	f32 mass;
};

#endif
