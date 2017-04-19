#include "Spring.h"

#include "Particle.h"

Spring::Spring():
	p1(NULL),
	p2(NULL),
	k(1.0f),
	b(1.0f),
	l0(1.0f)
{

}

Spring::Spring(Particle* p1, Particle* p2):
	p1(p1),
	p2(p2),
	k(100.0f),
	b(1.0f)
{
	computeRestLength();
}

void Spring::computeRestLength()
{
	l0 = p1->p0.getDistanceFrom(p2->p0);
}

void Spring::setRestLength()
{
	l0 = p1->p.getDistanceFrom(p2->p);
}

void Spring::apply()
{
	core::vector3df l = p1->p - p2->p;
	core::vector3df ln(l); ln.normalize();
	core::vector3df l_ = p1->v - p2->v;
	core::vector3df fa = -(k*(l.getLength() - l0) + b*l_.dotProduct(ln))*ln;
	p1->addForce(fa);
	p2->addForce(-fa);
}
