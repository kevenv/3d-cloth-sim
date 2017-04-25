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
	/*
	core::vector3df x1(0.0f, 0.0f, 0.0f);
	core::vector3df x2(0.0f, 1.0f, 0.0f);
	core::vector3df x3(-0.5f, -0.5f, 0.0f);
	core::vector3df x4(0.5f, 0.5f, 0.0f);
	*/
	/*core::vector3df x1(0.0f, 0.0f, 0.0f);
	core::vector3df x2(0.0f, 1.0f, 0.0f);
	core::vector3df x3(-0.5f, -0.5f, 0.0f);
	core::vector3df x4(0.5f, 0.5f, 0.0f);
	x3 += 0.2; x4 += 0.2;
	core::matrix4 m;
	m.setRotationDegrees(core::vector3df(0.0f,-45.0f,0.0f));
	m.rotateVect(x3);
	m.rotateVect(x4);
	if (testEdgeEdge(x1, x2, x3, x4)) {
		volatile int t = 3;
	}*/

	applyRepulsionsForces(sim, dt);
	//resolveCollisions(sim, dt);
}

void CollisionsHandler::applyRepulsionsForces(ClothSimulator& clothSim, float dt)
{
	//todo: create a static array of all particles in all springs of all cloths
	/*auto& springs = clothSim.getSprings();
	for (auto* sA : springs) {
		Particle* p0 = sA->getP1();
		Particle* p1 = sA->getP2();
		core::vector3df& x0 = p0->p;
		core::vector3df& x1 = p1->p;

		for (auto* sB : springs) {
			if (sA != sB) {
				Particle* p2 = sB->getP1();
				Particle* p3 = sB->getP2();
				core::vector3df& x2 = p2->p;
				core::vector3df& x3 = p3->p;

				if (p0 == p1 || p2 == p3) {
					static bool once = false;
					if (!once) {
						once = true;
						p0->pinned = true;
						p1->pinned = true;
						p2->pinned = true;
						p3->pinned = true;
					}
				}

				if (p0 == p2 || p0 == p3 || p1 == p2 || p1 == p3) continue; // skip edges that share an endpoint

				if (testEdgeEdge(x0, x1, x2, x3)) {
					//apply repulsion force
					static bool once = false;
					if (!once) {
						once = true;
						p0->pinned = true;
						p1->pinned = true;
						p2->pinned = true;
						p3->pinned = true;
					}
				}
			}
		}
	}*/

	auto& particles = clothSim.getParticles();
	for (auto* p : particles) {
		auto& pTriangles = clothSim.getTriangleParticles();
		for (int i = 0; i < pTriangles.size(); i+=3) {
			Particle* pA = pTriangles[i + 0];
			Particle* pB = pTriangles[i + 1];
			Particle* pC = pTriangles[i + 2];
			
			if (!partOfTriangle(&p->p, &pA->p, &pB->p, &pC->p)) {
				if (testPointTriangle(p->p, pA->p, pB->p, pC->p)) {
					//apply repulsion force
					p->pinned = true;
					pA->pinned = true;
					pB->pinned = true;
					pC->pinned = true;
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

		//todo: create a static array of all particles in all springs of all cloths
		auto& springs = clothSim.getSprings();
		for (auto* sA : springs) {
			core::vector3df& x0 = sA->getP1()->p;
			core::vector3df& x1 = sA->getP2()->p;
			for (auto* sB : springs) {
				if (sA != sB) {
					core::vector3df& x2 = sB->getP1()->p;
					core::vector3df& x3 = sB->getP2()->p;

					core::vector3df x[4];
					core::vector3df v[4];
					float t = solveCollisionTime(x, v, dt);

					if (testEdgeEdge(x0, x1, x2, x3)) {
						//apply repulsion force
						sA->getP1()->p += 3.0f;
					}
				}
			}
		}

		auto& particles = clothSim.getParticles();
		for (auto* p : particles) {
			auto& pTriangles = clothSim.getTriangleParticles();
			for (int i = 0; i < pTriangles.size(); i += 3) {
				Particle* pA = pTriangles[i + 0];
				Particle* pB = pTriangles[i + 1];
				Particle* pC = pTriangles[i + 2];

				core::vector3df x[4];
				core::vector3df v[4];
				float t = solveCollisionTime(x, v, dt);

				if (!partOfTriangle(&p->p, &pA->p, &pB->p, &pC->p)) {
					if (testPointTriangle(p->p, pA->p, pB->p, pC->p)) {
						//apply repulsion force
						p->p += 3.0f;
					}
				}
			}
		}

		// collisions detection
		resolved = true;

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
	for (int i = 0; i < nRoots; ++i) { //todo: unroll
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

	From: Real Time Collision Detection, Christer Ericson, p.47
*/
bool CollisionsHandler::testPointTriangle(core::vector3df & p, core::vector3df& a, core::vector3df& b, core::vector3df& c)
{
	core::vector3df x43(p - c);
	core::vector3df U(b - a);
	core::vector3df V(c - a);
	core::vector3df n(U.crossProduct(V));
	n.normalize();
	if ( abs(x43.dotProduct(n)) >= 1E-3f ) return false;

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
	float w = 1.0f - u - v;

	// Check if point is in triangle
	float eps = 1E-6f;
	return (u >= 0.0f - eps) && (v >= 0.0f - eps) && (u + v <= 1.0f + eps);
}

/*
	p1,p2 = Edge 1
	p3,p4 = Edge 2

	From: Paul Bourke (http://paulbourke.net/geometry/pointlineplane/)
*/
bool CollisionsHandler::testEdgeEdge(core::vector3df & p1, core::vector3df & p2, core::vector3df & p3, core::vector3df & p4)
{
	const float EPS = 1E-6f;
	core::vector3df p13(p1 - p3);
	core::vector3df p43(p4 - p3);
	core::vector3df p21(p2 - p1);
	if (abs(p43.X) < EPS && abs(p43.Y) < EPS && abs(p43.Z) < EPS)
		return false;
	if (abs(p21.X) < EPS && abs(p21.Y) < EPS && abs(p21.Z) < EPS)
		return false;

	float d1343 = p13.dotProduct(p43);
	float d4321 = p43.dotProduct(p21);
	float d1321 = p13.dotProduct(p21);
	float d4343 = p43.dotProduct(p43);
	float d2121 = p21.dotProduct(p21);

	float denom = d2121 * d4343 - d4321 * d4321;
	if (abs(denom) < EPS)
		return false;
	float numer = d1343 * d4321 - d1321 * d4343;

	float mua = numer / denom;
	float mub = (d1343 + d4321 * (mua)) / d4343;

	core::vector3df pa = p1 + mua * p21;
	core::vector3df pb = p3 + mub * p43;

	float d = pa.getDistanceFrom(pb);
	return d < EPS;
}
