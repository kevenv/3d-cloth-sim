#ifndef SPRING_H
#define SPRING_H

#include "irrlicht.h"
using namespace irr;

class Particle;

class Spring
{
public:
	Spring();
	Spring(Particle* p1, Particle* p2);

private:
	Particle* p1;
	Particle* p2;

	f32 restLength;
	f32 stiffness;
};

#endif
