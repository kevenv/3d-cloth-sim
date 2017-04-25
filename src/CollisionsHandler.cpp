#include "CollisionsHandler.h"

#include "ClothSimulator.h"
#include "cubicSolver.h"
#include "Spring.h"
#include "Particle.h"
#include "Cloth.h"
#include <math.h>
#include <assert.h>
#include "lineintersect_utils.h"

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
	auto& springs = clothSim.getSprings();
	for (auto* sA : springs) {
		Particle* p0 = sA->getP1();
		Particle* p1 = sA->getP2();

		for (auto* sB : springs) {
			if (sA != sB) {
				Particle* p2 = sB->getP1();
				Particle* p3 = sB->getP2();

				assert(p0 != p1 || p2 != p3);

				if (p0 == p2 || p0 == p3 || p1 == p2 || p1 == p3) continue; // skip edges that share an endpoint

				core::vector3df N;
				float a, b;
				if (testEdgeEdge(p0->p, p1->p, p2->p, p3->p, a, b, N)) {
					//apply repulsion force
					p0->pinned = true;
					p1->pinned = true;
					p2->pinned = true;
					p3->pinned = true;
				}
			}
		}
	}

	auto& particles = clothSim.getParticles();
	auto& pTriangles = clothSim.getTriangleParticles();
	for (auto* p : particles) {
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

		auto& springs = clothSim.getSprings();
		for (auto* sA : springs) {
			Particle* p0 = sA->getP1();
			Particle* p1 = sA->getP2();

			for (auto* sB : springs) {
				if (sA != sB) {
					Particle* p2 = sB->getP1();
					Particle* p3 = sB->getP2();

					assert(p0 != p1 || p2 != p3);

					if (p0 == p2 || p0 == p3 || p1 == p2 || p1 == p3) continue; // skip edges that share an endpoint

					// find t*
					core::vector3df x[4];
					x[0] = p0->p; x[1] = p1->p; x[2] = p2->p; x[3] = p3->p;
					core::vector3df v[4];
					v[0] = p0->v; v[1] = p1->v; v[2] = p2->v; v[3] = p3->v;
					float t = solveCollisionTime(x, v, dt);
					if (t < 0) continue;

					if (t != 0) {
						volatile int t = 0;
						t++;
					}

					// set p to t*
					core::vector3df p0New(p0->p + p0->v*t);
					core::vector3df p1New(p1->p + p1->v*t);
					core::vector3df p2New(p2->p + p2->v*t);
					core::vector3df p3New(p3->p + p3->v*t);
					
					core::vector3df N;
					float a, b;
					if (testEdgeEdge(p0New, p1New, p2New, p3New, a, b, N)) {
						//apply repulsion force
						p0->pinned = true;
						p1->pinned = true;
						p2->pinned = true;
						p3->pinned = true;
					}
				}
			}
		}

		auto& particles = clothSim.getParticles();
		auto& pTriangles = clothSim.getTriangleParticles();
		for (auto* p : particles) {
			for (int i = 0; i < pTriangles.size(); i += 3) {
				Particle* pA = pTriangles[i + 0];
				Particle* pB = pTriangles[i + 1];
				Particle* pC = pTriangles[i + 2];

				if (!partOfTriangle(&p->p, &pA->p, &pB->p, &pC->p)) {
					// find t*
					core::vector3df x[4];
					x[0] = pA->p; x[1] = pB->p; x[2] = pC->p; x[3] = p->p;
					core::vector3df v[4];
					v[0] = pA->v; v[1] = pB->v; v[2] = pC->v; v[3] = p->v;
					float t = solveCollisionTime(x, v, dt);
					if (t < 0) continue;

					if (t != 0) {
						volatile int t = 0;
						t++;
					}

					// get p to t*
					core::vector3df pNew(p->p + p->v*t);
					core::vector3df pANew(pA->p + pA->v*t);
					core::vector3df pBNew(pB->p + pB->v*t);
					core::vector3df pCNew(pC->p + pC->v*t);

					if (testPointTriangle(pNew, pANew, pBNew, pCNew)) {
						//apply repulsion force
						p->pinned = true;
						pA->pinned = true;
						pB->pinned = true;
						pC->pinned = true;
					}
				}
			}
		}
		
		// collisions detection
		resolved = true; //todo: implement iterative solver

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

core::vector3df CollisionsHandler::computeNormalTriangle(core::vector3df& p, core::vector3df& a, core::vector3df& b, core::vector3df& c)
{
	core::vector3df U(b - a);
	core::vector3df V(c - a);
	core::vector3df n(U.crossProduct(V));
	n.normalize();
	return n;
}

core::vector3df CollisionsHandler::computeNormalEdges(core::vector3df& p1, core::vector3df& p2, core::vector3df& p3, core::vector3df& p4)
{
	core::vector3df E1(p1 - p2);
	core::vector3df E2(p3 - p4);
	return E1.crossProduct(E2).normalize();
}

void CollisionsHandler::computeBarycentricCoords(core::vector3df & p, core::vector3df& a, core::vector3df& b, core::vector3df& c, float& u, float& v, float& w)
{
	core::vector3df v0(b - a);
	core::vector3df v1(c - a);
	core::vector3df v2(p - a);
	float dot00 = v0.dotProduct(v0);
	float dot01 = v0.dotProduct(v1);
	float dot11 = v1.dotProduct(v1);
	float dot20 = v2.dotProduct(v0);
	float dot21 = v2.dotProduct(v1);
	float denom = dot00 * dot11 - dot01 * dot01;
	v = (dot11 * dot20 - dot01 * dot21) / denom;
	w = (dot00 * dot21 - dot01 * dot20) / denom;
	u = 1.0f - v - w;
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
bool CollisionsHandler::testPointTriangle(core::vector3df& p, core::vector3df& a, core::vector3df& b, core::vector3df& c)
{
	core::vector3df x43(p - c);
	core::vector3df n = computeNormalTriangle(p, a, b, c);
	if ( abs(x43.dotProduct(n)) >= H ) return false;

	float u, v, w;
	computeBarycentricCoords(p, a, b, c, u, v, w);

	// Check if point is in triangle
	return (v >= 0.0f - DELTA) && (w >= 0.0f - DELTA) && (v + w <= 1.0f + DELTA);
}

/*
	p1,p2 = Edge 1
	p3,p4 = Edge 2
	a = nearest pt on edge 1 (relative to edge 1)
	b = nearest pt on edge 2 (relative to edge 2)
	N = Collision Normal

	From: "Fast, Robust Intersection of 3D Line Segments", Graham Rhodes, Game Programming Gems II, 2001
*/
bool CollisionsHandler::testEdgeEdge(core::vector3df& p1, core::vector3df& p2, core::vector3df& p3, core::vector3df& p4, float& a, float &b, core::vector3df& N)
{
	bool intersects;
	core::vector3df intPt;
	core::vector3df pa, pb;
	IntersectLineSegments(p1.X, p1.Y, p1.Z, p2.X, p2.Y, p2.Z, 
                          p3.X, p3.Y, p3.Z, p4.X, p4.Y, p4.Z,
                          false /*infinite lines*/, DELTA,

                          pa.X, pa.Y, pa.Z, pb.X, pb.Y, pb.Z,
                          intPt.X, intPt.Y, intPt.Z,
						  N.X, N.Y, N.Z,
					      intersects);
	if (intersects) {
		N.normalize();
		core::vector3df p12(p1 - p2);
		core::vector3df p34(p3 - p4);
		core::vector3df p1a(p1 - pa);
		core::vector3df p3b(p3 - pb);
		a = 0.0f;
		b = 0.0f;
		if (!p1a.equals(core::vector3df(0.0f, 0.0f, 0.0f))) {
			float p12l = p12.getLength();
			p12.normalize();
			p1a /= p12l;
			a = p1a.getLength();
		}
		if (!p3b.equals(core::vector3df(0.0f, 0.0f, 0.0f))) {
			float p34l = p34.getLength();
			p34.normalize();
			p3b /= p34l;
			b = p3b.getLength();
		}
	}

	return intersects;
}