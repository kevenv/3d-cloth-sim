#include "Spring.h"

#include "Particle.h"

Spring::Spring():
	p1(NULL),
	p2(NULL),
	k(0.0f),
	b(0.0f),
	l0(0.0f),
	edge(true)
{

}

Spring::Spring(Particle* p1, Particle* p2, f32 k, f32 b, bool edge):
	p1(p1),
	p2(p2),
	k(k),
	b(b),
	edge(edge)
{
	computeRestLength();
}

void Spring::computeRestLength()
{
	l0 = p1->getP0().getDistanceFrom(p2->getP0());
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
