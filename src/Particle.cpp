#include "Particle.h"

Particle::Particle() :
	mass(1.0f),
	pinned(false),
	p(0.0f, 0.0f, 0.0f),
	v(0.0f, 0.0f, 0.0f),
	p0(0.0f, 0.0f, 0.0f),
	v0(0.0f, 0.0f, 0.0f),
	f(0.0f, 0.0f, 0.0f)
{

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