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

	const float H = 1E-3f;
	const float DELTA = 1E-6f; // H / (characteristic length of the triangle = longuest side)

private:
	void applyRepulsionsForces(ClothSimulator& clothSim, float dt);
	void resolveCollisions(ClothSimulator& clothSim, float dt);

	float solveCollisionTime(const core::vector3df x[4], const core::vector3df v[4], float dt);
	bool partOfTriangle(core::vector3df* p, core::vector3df* a, core::vector3df* b, core::vector3df* c);
	core::vector3df computeNormalTriangle(core::vector3df& p, core::vector3df& a, core::vector3df& b, core::vector3df& c);
	core::vector3df computeNormalEdges(core::vector3df& p1, core::vector3df& p2, core::vector3df& p3, core::vector3df& p4);
	void computeBarycentricCoords(core::vector3df& p, core::vector3df& a, core::vector3df& b, core::vector3df& c, float& u, float& v, float& w);
	bool testPointTriangle(core::vector3df& p, core::vector3df& a, core::vector3df& b, core::vector3df& c);
	bool testEdgeEdge(core::vector3df& p1, core::vector3df& p2, core::vector3df& p3, core::vector3df& p4, float& a, float& b, core::vector3df& N);
};

#endif
