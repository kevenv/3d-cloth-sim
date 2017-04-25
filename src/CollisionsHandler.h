#ifndef COLLISIONS_HANDLER_H
#define COLLISIONS_HANDLER_H

#include <irrlicht.h>
using namespace irr;

class ClothSimulator;
class Particle;

class CollisionsHandler
{
public:
	CollisionsHandler();
	~CollisionsHandler();

	void handleCollisions(ClothSimulator& clothSim, float dt);

	const float H = 1E-3f;
	const float DELTA = 1E-6f; // H / (characteristic length of the triangle = longuest side)
	const int N_ITERATIONS = 1;

private:
	void applyRepulsionsForces(ClothSimulator& clothSim, float dt);
	void applyRepulsion_PointTriangle(Particle* p, Particle* pA, Particle* pB, Particle* pC, float dt);
	void applyRepulsion_EdgeEdge(Particle* p1, Particle* p2, Particle* p3, Particle* p4, float dt);
	void resolveCollisions(ClothSimulator& clothSim, float dt);
	bool resolveCollision_PointTriangle(Particle* p, Particle* pA, Particle* pB, Particle* pC, float dt);
	bool resolveCollision_EdgeEdge(Particle* p1, Particle* p2, Particle* p3, Particle* p4, float dt);

	float solveCollisionTime(Particle* p1, Particle* p2, Particle* p3, Particle* p4, float dt);
	bool pointPartOfTriangle(Particle* p, Particle* pA, Particle* pB, Particle* pC);
	bool edgesSharePoint(Particle* p1, Particle* p2, Particle* p3, Particle* p4);
	core::vector3df computeNormalTriangle(core::vector3df& p, core::vector3df& pA, core::vector3df& pB, core::vector3df& pC);
	core::vector3df computeNormalEdges(core::vector3df& p1, core::vector3df& p2, core::vector3df& p3, core::vector3df& p4);
	void computeBarycentricCoords(core::vector3df& p, core::vector3df& pA, core::vector3df& pB, core::vector3df& pC, float& u, float& v, float& w);
	bool testPointTriangle(core::vector3df& p, core::vector3df& pA, core::vector3df& pB, core::vector3df& pC);
	bool testEdgeEdge(core::vector3df& p1, core::vector3df& p2, core::vector3df& p3, core::vector3df& p4, float& a, float& b, core::vector3df& N);
};

#endif
