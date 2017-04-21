#ifndef PARTICLE_H
#define PARTICLE_H

#include "irrlicht.h"
using namespace irr;

class Particle
{
public:
	f32 mass;
	bool pinned;
	core::vector3df p;
	core::vector3df v;
	core::vector3df f;

	Particle();
	Particle(float x, float y, float z, float vx, float vy, float vz);

	void reset();
	void addForce(const core::vector3df& force) { f += force; }
	inline core::vector3df& getP0() { return p0; }
	void setP0(const core::vector3df& p0);

private:
	core::vector3df p0;
	core::vector3df v0;
};

#endif
