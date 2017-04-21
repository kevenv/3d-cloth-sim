#ifndef COLLISIONS_HANDLER_H
#define COLLISIONS_HANDLER_H

#include <irrlicht.h>
using namespace irr;

class ClothSimulator;

class CollisionsHandler
{
public:
	CollisionsHandler();
	~CollisionsHandler();

	void handleCollisions(ClothSimulator& clothSim, float dt);

private:
	void applyRepulsionsForces(ClothSimulator& clothSim, float dt);
	void resolveCollisions(ClothSimulator& clothSim, float dt);

	float solveCollisionTime(const core::vector3df x[4], const core::vector3df v[4], float dt);
	bool partOfTriangle(core::vector3df* p, core::vector3df* a, core::vector3df* b, core::vector3df* c);
	bool testPointTriangle(core::vector3df & p, core::vector3df& a, core::vector3df& b, core::vector3df& c);
	bool testEdgeEdge(core::vector3df & p1, core::vector3df & p2, core::vector3df & p3, core::vector3df & p4);
};

#endif
