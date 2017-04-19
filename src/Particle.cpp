#include "Particle.h"

Particle::Particle(float x, float y, float z, float vx, float vy, float vz):
	mass(1.0f),
	pinned(false),
	f(0.0f,0.0f,0.0f)
{
	p0 = core::vector3df(x,y,z);
	v0 = core::vector3df(vx,vy,vz);
	reset();
}

void Particle::reset()
{
	p = p0;
	v = v0;
	f = core::vector3df(0.0f, 0.0f, 0.0f);
}

void Particle::addForce(const core::vector3df& force)
{
	f += force;
}

double Particle::distance(float x, float y, float z)
{
	core::vector3df v(x, y, z);
	v -= p;
	return v.getLength();
}
