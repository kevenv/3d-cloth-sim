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
	const float RESTITUTION = 0.0f;

private:
	void applyRepulsionsForces(ClothSimulator& clothSim, float dt);
	void applyRepulsion_PointTriangle(Particle* p, Particle* pA, Particle* pB, Particle* pC, float dt);
	void applyRepulsion_EdgeEdge(Particle* p1, Particle* p2, Particle* p3, Particle* p4, float dt);
	void resolveCollisions(ClothSimulator& clothSim, float dt);
	bool resolveCollision_PointTriangle(Particle* p, Particle* pA, Particle* pB, Particle* pC, float dt);
	bool resolveCollision_EdgeEdge(Particle* p1, Particle* p2, Particle* p3, Particle* p4, float dt);

	void applyImpulsion_PointTriangle(Particle* p, Particle* pA, Particle* pB, Particle* pC, core::vector3df& N, float w1, float w2, float w3, float I);
	void applyImpulsion_EdgeEdge(Particle* p1, Particle* p2, Particle* p3, Particle* p4, core::vector3df& N, float a, float b, float I);
	float computeRepulsionImpulse(Particle* p, float Vrn, float d, float dt);
	float computeImpulse_EdgeEdge(Particle* p1, Particle* p2, Particle* p3, Particle* p4, float a, float b, float Vrn);
	float computeImpulse_PointTriangle(Particle* p, Particle* pA, Particle* pB, Particle* pC, float w1, float w2, float w3, float Vrn);
	void computeRelVel_PointTriangle(Particle* p, Particle* pA, Particle* pB, Particle* pC, float w1, float w2, float w3, core::vector3df& N, core::vector3df& T, 
                                     float& Vrn, float& Vrt);
	void computeRelVel_EdgeEdge(Particle* p1, Particle* p2, Particle* p3, Particle* p4, float a, float b, core::vector3df& N, core::vector3df& T,
                                float& Vrn, float& Vrt);
	core::vector3df computeTangent(core::vector3df& N);
	void computeNewPos(Particle* p1, Particle* p2, Particle* p3, Particle* p4, float t,
                       core::vector3df& newP1, core::vector3df& newP2, core::vector3df& newP3, core::vector3df& newP4);
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
