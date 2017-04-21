#include "Particle.h"

Particle::Particle():
	mass(0.0f),
	pinned(false),
	p(0.0f,0.0f,0.0f),
	v(0.0f, 0.0f, 0.0f),
	p0(0.0f, 0.0f, 0.0f),
	v0(0.0f, 0.0f, 0.0f),
	f(0.0f, 0.0f, 0.0f)
{

}

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

void Particle::setP0(const core::vector3df& p0)
{
	this->p0 = p0;
	reset();
}