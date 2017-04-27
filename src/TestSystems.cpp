#include "TestSystems.h"

#include "ClothSimulator.h"
#include "Particle.h"
#include "Spring.h"

#include "irrlicht.h"
using namespace irr;

TestSystems::TestSystems()
{

}

void TestSystems::load(ClothSimulator* sim)
{
	Particle* p1 = new Particle(20+0, 100, 0, 0, 0, 0); p1->pinned = true;
	Particle* p2 = new Particle(20+100, 100, 0, 0, 0, 0);// p2->pinned = true;
	Spring* s = new Spring(p1, p2, 10.0f, 0.5f);

	sim->addTestParticle(p1);
	sim->addTestParticle(p2);
	sim->addTestSpring(s);
}