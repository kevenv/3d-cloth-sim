#ifndef SPRING_H
#define SPRING_H

#include "irrlicht.h"
using namespace irr;

class Particle;

class Spring
{
public:
	Spring();
	Spring(Particle* p1, Particle* p2, f32 k, f32 b);

	void computeRestLength();
	void apply();
	inline Particle* getP1() { return p1; }
	inline Particle* getP2() { return p2; }

private:
	Particle* p1;
	Particle* p2;

	f32 l0; // rest length
	f32 k; // stiffness
	f32 b; // damping
};

#endif
