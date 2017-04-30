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
	/*
	{
		float z = 10.0f;
		float y = 100.0f;
		Particle* p1 = new Particle(0, y, z + 0, 0, 0, 0); //p1->pinned = true;
		Particle* p2 = new Particle(0, y, z + 100, 0, 0, 0); //p2->pinned = true;
		Spring* s = new Spring(p1, p2, 10.0f, 0.5f);

		sim->addTestParticle(p1);
		sim->addTestParticle(p2);
		sim->addTestSpring(s);
	}
	{
		float z = 10.0f;
		float y = 100.0f;
		float x = 100.0f;
		Particle* p1 = new Particle(x, y, z + 0, 0, 0, 0); p1->pinned = true;
		Particle* p2 = new Particle(x, y, z + 100, 0, 0, 0); p2->pinned = true;
		Spring* s = new Spring(p1, p2, 10.0f, 0.5f);

		sim->addTestParticle(p1);
		sim->addTestParticle(p2);
		sim->addTestSpring(s);
	}
	camera->setPosition(core::vector3df(37, 200, 223));
	camera->setTarget(core::vector3df(130,46,-21));
	*/

	// point-triangle
	
	Particle* p = new Particle(-100, 50, 20, 0, 0, 0); //p->pinned = true;
	Particle* pA = new Particle(0, 0, 0, 0, 0, 0); pA->pinned = true;
	Particle* pB = new Particle(0, 100, 0, 0, 0, 0); pB->pinned = true;
	Particle* pC = new Particle(0, 0, 100, 0, 0, 0); pC->pinned = true;

	core::matrix4 m;
	m.setRotationDegrees(core::vector3df(0,0,25));
	auto newA(pA->getP0());
	auto newB(pB->getP0());
	auto newC(pC->getP0());
	m.rotateVect(newA);
	m.rotateVect(newB);
	m.rotateVect(newC);
	pA->setP0(newA);
	pB->setP0(newB);
	pC->setP0(newC);

	sim->addTestParticle(p);
	sim->addTestParticle(pA);
	sim->addTestParticle(pB);
	sim->addTestParticle(pC);
	sim->addTestTriangle(pA, pB, pC);

	camera->setPosition(core::vector3df(86, 212, 360));
	camera->setTarget(core::vector3df(133, 9.3, -12));
	
}