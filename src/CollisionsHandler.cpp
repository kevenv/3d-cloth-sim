#include "CollisionsHandler.h"

#include "ClothSimulator.h"
#include "cubicSolver.h"
#include "Spring.h"
#include "Particle.h"
#include "Cloth.h"
#include <math.h>

CollisionsHandler::CollisionsHandler()
{

}

CollisionsHandler::~CollisionsHandler()
{

}

void CollisionsHandler::handleCollisions(ClothSimulator& sim, float dt)
{
	applyRepulsionsForces(sim, dt);
	resolveCollisions(sim, dt);
}

void CollisionsHandler::applyRepulsionsForces(ClothSimulator& clothSim, float dt)
{
	//todo: create a static array of all particles in all springs of all cloths
	auto& springs = clothSim.getSprings();
	for (auto* sA : springs) {
		core::vector3df& x0 = sA->getP1()->p;
		core::vector3df& x1 = sA->getP2()->p;
		for (auto* sB : springs) {
			if (sA != sB) {
				core::vector3df& x2 = sB->getP1()->p;
				core::vector3df& x3 = sB->getP2()->p;
				if (testEdgeEdge(x0, x1, x2, x3)) {
					//apply repulsion force
					sA->getP1()->p += 3.0f;
				}
			}
		}
	}

	auto& particles = clothSim.getParticles();
	for (auto* p : particles) {
		auto& cloths = clothSim.getCloths();
		for (auto* c : cloths) { //todo: create a list of all triangles for all cloths
			auto& indices = c->getTriangleIndices();
			for (int i = 0; i < indices.size(); i+=3) {
				auto& pCloth = c->getParticles();
				core::vector3df* x1 = &pCloth[indices[i + 0]].p;
				core::vector3df* x2 = &pCloth[indices[i + 1]].p;
				core::vector3df* x3 = &pCloth[indices[i + 2]].p;
				if (!partOfTriangle(&p->p, x1, x2, x3)) {
					if (testPointTriangle(p->p, *x1, *x2, *x3)) {
						//apply repulsion force
						p->p += 3.0f;
					}
				}
			}
		}
	}
}

void CollisionsHandler::resolveCollisions(ClothSimulator& clothSim, float dt)
{
	bool resolved = false;
	int iterations = 0;
	while (!resolved && iterations < 1000) {
		resolved = true;

		// collisions detection
		resolved = true;

		core::vector3df x[4];
		core::vector3df v[4];
		float t = solveCollisionTime(x,v,dt);

		iterations++;
	}
}

float CollisionsHandler::solveCollisionTime(const core::vector3df x[4], const core::vector3df v[4], float dt)
{
	const core::vector3df& x1 = x[0];
	const core::vector3df& x2 = x[1];
	const core::vector3df& x3 = x[2];
	const core::vector3df& x4 = x[3];
	const core::vector3df& v1 = v[0];
	const core::vector3df& v2 = v[1];
	const core::vector3df& v3 = v[2];
	const core::vector3df& v4 = v[3];

	core::vector3df x21(x2 - x1);
	core::vector3df x31(x3 - x1);
	core::vector3df x41(x4 - x1);
	core::vector3df v21(v2 - v1);
	core::vector3df v31(v3 - v1);
	core::vector3df v41(v4 - v1);

	float A = (-(v21.Z*v31.Y*v41.X) + v21.Y*v31.Z*v41.X + v21.Z*v31.X*v41.Y - v21.X*v31.Z*v41.Y - v21.Y*v31.X*v41.Z + v21.X*v31.Y*v41.Z);

	float B = (-(v31.X*v41.Z*x21.Y) + v31.X*v41.Y*x21.Z + v21.Z*v41.Y*x31.X - v21.Y*v41.Z*x31.X - v21.Z*v41.X*x31.Y + 
		v21.X*v41.Z*x31.Y + v21.Y*v41.X*x31.Z - v21.X*v41.Y*x31.Z + v21.Z*v31.X*x41.Y + v31.Z*(-(v41.Y*x21.X) + v41.X*x21.Y + 
			v21.Y*x41.X - v21.X*x41.Y) - v21.Y*v31.X*x41.Z + v31.Y*(v41.Z*x21.X - v41.X*x21.Z - v21.Z*x41.X + v21.X*x41.Z));

	float C = (-(v41.Z*x21.Y*x31.X) + v41.Y*x21.Z*x31.X + v41.Z*x21.X*x31.Y - v41.X*x21.Z*x31.Y - v41.Y*x21.X*x31.Z + 
		v41.X*x21.Y*x31.Z + v31.Z*x21.Y*x41.X - v31.Y*x21.Z*x41.X - v21.Z*x31.Y*x41.X + v21.Y*x31.Z*x41.X - v31.Z*x21.X*x41.Y + 
		v31.X*x21.Z*x41.Y + v21.Z*x31.X*x41.Y - v21.X*x31.Z*x41.Y + v31.Y*x21.X*x41.Z - v31.X*x21.Y*x41.Z - v21.Y*x31.X*x41.Z + v21.X*x31.Y*x41.Z);

	float D = -x21.Z*x31.Y*x41.X + x21.Y*x31.Z*x41.X + x21.Z*x31.X*x41.Y - x21.X*x31.Z*x41.Y - x21.Y*x31.X*x41.Z + x21.X*x31.Y*x41.Z;

	float roots[3] = { 0 };
	int nRoots = cubic_solver(A,B,C,D, roots); // 0 <= t <= 1
	if (nRoots == 0) return -1;
	
	// get smallest t
	bool validRoot = false;
	float rootsGood[3] = { 1E16f };
	for (int i = 0; i < nRoots; ++i) {
		if (roots[i] >= 0 && roots[i] <= dt) {
			rootsGood[i] = roots[i];
			validRoot = true;
		}
	}
	if (!validRoot) return -1;
	float root = std::fmin(std::fmin(rootsGood[0], rootsGood[1]), rootsGood[2]);
	return root;
}

bool CollisionsHandler::partOfTriangle(core::vector3df* p, core::vector3df* a, core::vector3df* b, core::vector3df* c)
{
	return p == a || p == b || p == c;
}

/*
	p = Point
	a,b,c = Triangle
*/
bool CollisionsHandler::testPointTriangle(core::vector3df & p, core::vector3df& a, core::vector3df& b, core::vector3df& c)
{
	// Compute barycentric coordinates
	core::vector3df v0(b - a);
	core::vector3df v1(c - a);
	core::vector3df v2(p - a);
	float dot00 = v0.dotProduct(v0);
	float dot01 = v0.dotProduct(v1);
	float dot11 = v1.dotProduct(v1);
	float dot20 = v2.dotProduct(v0);
	float dot21 = v2.dotProduct(v1);
	float denom = dot00 * dot11 - dot01 * dot01;
	float u = (dot11 * dot20 - dot01 * dot21) * denom;
	float v = (dot00 * dot21 - dot01 * dot20) * denom;
	float w = 1.0f - v - w;

	// Check if point is in triangle
	return (u >= 0.0f) && (v >= 0.0f) && (u + v <= 1.0f);
}

/*
	x0,x1 = Edge 1
	x2,x3 = Edge 2
*/
bool CollisionsHandler::testEdgeEdge(core::vector3df & x0, core::vector3df & x1, core::vector3df & x2, core::vector3df & x3)
{
	return false;
}