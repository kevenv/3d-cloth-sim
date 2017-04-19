#ifndef COLLISIONS_HANDLER_H
#define COLLISIONS_HANDLER_H

#include <irrlicht.h>
using namespace irr;

class ClothSimulator;
class Cloth;

class CollisionsHandler
{
public:
	CollisionsHandler();
	~CollisionsHandler();

	void handleCollisions(ClothSimulator& clothSim, float dt);

private:
	void applyRepulsionsForces(ClothSimulator& clothSim, float dt);
	void resolveCollisions(ClothSimulator& clothSim, float dt);

	bool tooClose(Cloth* clothA, Cloth* clothB);
	bool testPointTriangle(Cloth* clothA, Cloth* clothB);
	bool testEdgeEdge(Cloth* clothA, Cloth* clothB);
	bool testPointTriangle(core::vector3df & x0, core::vector3df& x1, core::vector3df& x2, core::vector3df& x3);
	bool testEdgeEdge(core::vector3df & x0, core::vector3df & x1, core::vector3df & x2, core::vector3df & x3);
};

#endif
