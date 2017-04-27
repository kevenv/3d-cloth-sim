#include "TestSystems.h"

#include "ClothSimulator.h"
#include "Particle.h"
#include "Spring.h"

TestSystems::TestSystems()
{

}

/*
         y
         ^
         |
    z <--x
*/

void TestSystems::load(ClothSimulator* sim, scene::ICameraSceneNode* camera)
{
	// test edge-edge
	{
		float z = 10.0f;
		float y = 100.0f;
		Particle* p1 = new Particle(0, y, z + 0, 0, 0, 0); p1->pinned = true;
		Particle* p2 = new Particle(0, y, z + 100, 0, 0, 0); p2->pinned = true;
		Spring* s = new Spring(p1, p2, 10.0f, 0.5f);

		sim->addTestParticle(p1);
		sim->addTestParticle(p2);
		sim->addTestSpring(s);
	}
	{
		float z = 10.0f;
		float y = 100.0f;
		float x = 0.0f;
		Particle* p1 = new Particle(x, y, z + 0, 0, 0, 0); p1->pinned = true;
		Particle* p2 = new Particle(x, y, z + 100, 0, 0, 0); p2->pinned = true;
		Spring* s = new Spring(p1, p2, 10.0f, 0.5f);

		sim->addTestParticle(p1);
		sim->addTestParticle(p2);
		sim->addTestSpring(s);
	}

	Particle* pA = new Particle(0, 0, 0, 0, 0, 0); pA->pinned = true;
	Particle* pB = new Particle(0, 100, 0, 0, 0, 0); pB->pinned = true;
	Particle* pC = new Particle(0, 0, 100, 0, 0, 0); pC->pinned = true;

	sim->addTestParticle(pA);
	sim->addTestParticle(pB);
	sim->addTestParticle(pC);
	sim->addTestTriangle(pA, pB, pC);

	camera->setPosition(core::vector3df(37, 200, 223));
	camera->setTarget(core::vector3df(130,46,-21));
}