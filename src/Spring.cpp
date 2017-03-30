#include "Spring.h"

Spring::Spring():
	p1(NULL),
	p2(NULL),
	stiffness(1.0f),
	restLength(1.0f)
{

}

Spring::Spring(Particle* p1, Particle* p2):
	p1(p1),
	p2(p2),
	stiffness(1.0f),
	restLength(1.0f)
{

}