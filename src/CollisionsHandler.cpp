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
	//applyRepulsionsForces(sim, dt);
	resolveCollisions(sim, dt);
}

void CollisionsHandler::applyRepulsionsForces(ClothSimulator& clothSim, float dt)
{
	auto& particles = clothSim.getPoints();
	auto& triangles = clothSim.getTriangles();
	for (auto* p : particles) {
		for (int i = 0; i < triangles.size(); i+=3) {
			Particle* pA = triangles[i + 0];
			Particle* pB = triangles[i + 1];
			Particle* pC = triangles[i + 2];

			if (!pointPartOfTriangle(p, pA, pB, pC)) {
				applyRepulsion_PointTriangle(p, pA, pB, pC, dt);
			}
		}
	}

	auto& springs = clothSim.getEdges();
	for (auto* sA : springs) {
		Particle* p1 = sA->getP1();
		Particle* p2 = sA->getP2();

		for (auto* sB : springs) {
			if (sA != sB) {
				Particle* p3 = sB->getP1();
				Particle* p4 = sB->getP2();
				
				if (!edgesSharePoint(p1, p2, p3, p4)) {
					applyRepulsion_EdgeEdge(p1, p2, p3, p4, dt);
				}
			}
		}
	}
}

void CollisionsHandler::applyRepulsion_PointTriangle(Particle* p, Particle* pA, Particle* pB, Particle* pC, float dt)
{
	if (testPointTriangle(p->p, pA->p, pB->p, pC->p)) {

		float w1, w2, w3;
		computeBarycentricCoords(p->p, pA->p, pB->p, pC->p, w1, w2, w3); // todo: computed 2 times
		core::vector3df N = computeNormalTriangle(p->p, pA->p, pB->p, pC->p); // todo: computed 2 times
		core::vector3df T = computeTangent(N);
		float d = H - (p->p - w1*pA->p - w2*pB->p - w3*pC->p).dotProduct(N);
		if (d < 0.0f) return; //outside

		float Vrn, Vrt;
		computeRelVel_PointTriangle(p, pA, pB, pC, w1, w2, w3, N, T, Vrn, Vrt);
		if (Vrn >= 0.1*d/dt) {
			return; //things are going to sort themselves out
		}

		float I = computeRepulsionImpulse(p, Vrn, d, dt);
		applyImpulsion_PointTriangle(p, pA, pB, pC, N, w1, w2, w3, I);
	}
}

void CollisionsHandler::applyRepulsion_EdgeEdge(Particle* p1, Particle* p2, Particle* p3, Particle* p4, float dt)
{
	float a, b;
	core::vector3df N;
	if (testEdgeEdge(p1->p, p2->p, p3->p, p4->p, a, b, N)) {
		
		N = computeNormalEdges(p1->p, p2->p, p3->p, p4->p);
		core::vector3df T = computeTangent(N);
		float d = H - ((1 - a)*p1->p + a*p2->p - (1 - b)*p3->p - b*p4->p).dotProduct(N);
		if (d < 0.0f) return; //outside

		float Vrn, Vrt;
		computeRelVel_EdgeEdge(p1, p2, p3, p4, a, b, N, T, Vrn, Vrt);
		if (Vrn >= 0.1*d / dt) {
			return; //things are going to sort themselves out
		}

		float I = computeRepulsionImpulse(p1, Vrn, d, dt);
		applyImpulsion_EdgeEdge(p1, p2, p3, p4, N, a, b, I);
	}
}

void CollisionsHandler::resolveCollisions(ClothSimulator& clothSim, float dt)
{
	auto& particles = clothSim.getPoints();
	auto& triangles = clothSim.getTriangles();
	auto& edges = clothSim.getEdges();

	bool resolved = false;
	int iterations = 0;
	while (!resolved && iterations < N_ITERATIONS) {
		resolved = true;
		
		for (auto* p : particles) {
			for (int i = 0; i < triangles.size(); i += 3) {
				Particle* pA = triangles[i + 0];
				Particle* pB = triangles[i + 1];
				Particle* pC = triangles[i + 2];

				if (!pointPartOfTriangle(p, pA, pB, pC)) {
					resolved = resolveCollision_PointTriangle(p, pA, pB, pC, dt);
				}
			}
		}

		for (auto* sA : edges) {
			Particle* p1 = sA->getP1();
			Particle* p2 = sA->getP2();

			for (auto* sB : edges) {
				if (sA != sB) {
					Particle* p3 = sB->getP1();
					Particle* p4 = sB->getP2();

					if (!edgesSharePoint(p1, p2, p3, p4)) {
						resolved = resolveCollision_EdgeEdge(p1, p2, p3, p4, dt);
					}
				}
			}
		}

		iterations++;
	}
}

bool CollisionsHandler::resolveCollision_PointTriangle(Particle* p, Particle* pA, Particle* pB, Particle* pC, float dt)
{
	// find t*
	float t = solveCollisionTime(pA, pB, pC, p, dt);
	if (t < 0) return true;

	// set p to t*
	core::vector3df pNew, pANew, pBNew, pCNew;
	computeNewPos(p, pA, pB, pC, t, pNew, pANew, pBNew, pCNew);

	if (testPointTriangle(pNew, pANew, pBNew, pCNew)) {

		float w1, w2, w3;
		computeBarycentricCoords(pNew, pANew, pBNew, pCNew, w1, w2, w3); // todo: computed 2 times
		core::vector3df N = computeNormalTriangle(pNew, pANew, pBNew, pCNew); // todo: computed 2 times
		core::vector3df T = computeTangent(N);
		float d = H - (pNew - w1*pANew - w2*pBNew - w3*pCNew).dotProduct(N);
		if (d < 0.0f) return true; //outside

		float Vrn, Vrt;
		//computeRelVel_PointTriangle(p, pA, pB, pC, w1, w2, w3, N, T, Vrn, Vrt);
		core::vector3df Vr((pNew - p->v) - w1*(pANew - pA->v) - w2*(pBNew - pB->v) - w3*(pCNew - pC->v));
		Vrn = Vr.dotProduct(N);
		Vrt = Vr.dotProduct(T);
		/*
		if (Vrn >= 0.1*d / dt) {
			return true; //things are going to sort themselves out
		}
		*/

		float I = computeImpulse_PointTriangle(p, pA, pB, pC, w1, w2, w3, Vrn);
		applyImpulsion_PointTriangle(p, pA, pB, pC, N, w1, w2, w3, I);
	}

	return false;
}

bool CollisionsHandler::resolveCollision_EdgeEdge(Particle* p1, Particle* p2, Particle* p3, Particle* p4, float dt)
{
	// find t*
	float t = solveCollisionTime(p1, p2, p3, p4, dt);
	if (t < 0) return true;

	// set p to t*
	core::vector3df p1New, p2New, p3New, p4New;
	computeNewPos(p1, p2, p3, p4, t, p1New, p2New, p3New, p4New);

	float a, b;
	core::vector3df N;
	if (testEdgeEdge(p1New, p2New, p3New, p4New, a, b, N)) {
		
		N = computeNormalEdges(p1New, p2New, p3New, p4New);
		core::vector3df T = computeTangent(N);
		float d = H - ((1 - a)*p1New + a*p2New - (1 - b)*p3New - b*p4New).dotProduct(N);
		if (d < 0.0f) return true; //outside

		float Vrn, Vrt;
		computeRelVel_EdgeEdge(p1, p2, p3, p4, a, b, N, T, Vrn, Vrt);
		/*
		core::vector3df Vr((1 - a)*(p1New-p1->v) + a*(p2New-p2->v) - (1 - b)*(p3New-p3->v) - b*(p4New-p4->v));
		Vrn = Vr.dotProduct(N);
		Vrt = Vr.dotProduct(T);
		*/
		/*
		if (Vrn >= 0.1*d / dt) {
			return true; //things are going to sort themselves out
		}
		*/

		float I = computeImpulse_EdgeEdge(p1, p2, p3, p4, a, b, Vrn);
		applyImpulsion_EdgeEdge(p1, p2, p3, p4, N, a, b, I);
	}

	return false;
}

void CollisionsHandler::applyImpulsion_PointTriangle(Particle* p, Particle* pA, Particle* pB, Particle* pC, core::vector3df& N, float w1, float w2, float w3, float I)
{
	float J = (2 * I) / (1 + w1*w1 + w2*w2 + w3*w3);
	if (!p->pinned) {
		p->v -= (J / p->mass)*N;
	}
	if (!pA->pinned) {
		pA->v += w1*(J / pA->mass)*N;
	}
	if (!pB->pinned) {
		pB->v += w2*(J / pB->mass)*N;
	}
	if (!pC->pinned) {
		pC->v += w3*(J / pC->mass)*N;
	}
	/*
	p->pinned = true;
	pA->pinned = true;
	pB->pinned = true;
	pC->pinned = true;
	*/
}

void CollisionsHandler::applyImpulsion_EdgeEdge(Particle* p1, Particle* p2, Particle* p3, Particle* p4, core::vector3df& N, float a, float b, float I)
{
	float J = (2 * I) / (a*a + (1 - a)*(1 - a) + b*b + (1 - b)*(1 - b));
	if (!p1->pinned) {
		p1->v += (1 - a)*(J / p1->mass)*N;
	}
	if (!p2->pinned) {
		p2->v += a*(J / p2->mass)*N;
	}
	if (!p3->pinned) {
		p3->v -= (1 - b)*(J / p3->mass)*N;
	}
	if (!p4->pinned) {
		p4->v -= b*(J / p4->mass)*N;
	}
	/*
	p1->pinned = true;
	p2->pinned = true;
	p3->pinned = true;
	p4->pinned = true;
	*/
}

float CollisionsHandler::computeRepulsionImpulse(Particle* p, float Vrn, float d, float dt)
{
	float k = 150.0f; //todo: LOL
	float I = p->pinned ? -dt*k*d : -fmin(dt*k*d, p->mass*(0.1f*d / dt - Vrn));
	return I;
}

float CollisionsHandler::computeImpulse_EdgeEdge(Particle* p1, Particle* p2, Particle* p3, Particle* p4, float a, float b, float Vrn)
{
	float iP1 = p1->pinned ? 0.0f : ((1 - a)*(1 - a)) / p1->mass;
	float iP2 = p2->pinned ? 0.0f : (a*a) / p2->mass;
	float iP3 = p3->pinned ? 0.0f : ((1 - b)*(1 - b)) / p3->mass;
	float iP4 = p4->pinned ? 0.0f : (b*b) / p4->mass;
	float I = -(1 + RESTITUTION) / (iP1 + iP2 + iP3 + iP4) * Vrn;
	return I;
}

float CollisionsHandler::computeImpulse_PointTriangle(Particle* p, Particle* pA, Particle* pB, Particle* pC, float w1, float w2, float w3, float Vrn)
{
	float iP = p->pinned ? 0.0f : 1.0f / p->mass;
	float iPA = pA->pinned ? 0.0f : (w1*w1) / pA->mass;
	float iPB = pB->pinned ? 0.0f : (w2*w2) / pB->mass;
	float iPC = pC->pinned ? 0.0f : (w3*w3) / pC->mass;
	float I = -(1 + RESTITUTION) / (iP + iPA + iPB + iPC) * Vrn;
	return I;
}

void CollisionsHandler::computeRelVel_PointTriangle(Particle* p, Particle* pA, Particle* pB, Particle* pC, float w1, float w2, float w3, core::vector3df& N, core::vector3df& T,
                                                    float& Vrn, float& Vrt)
{
	core::vector3df Vr(p->v - w1*pA->v - w2*pB->v - w3*pC->v);
	Vrn = Vr.dotProduct(N);
	Vrt = Vr.dotProduct(T);
}

void CollisionsHandler::computeRelVel_EdgeEdge(Particle* p1, Particle* p2, Particle* p3, Particle* p4, float a, float b, core::vector3df& N, core::vector3df& T,
                                               float& Vrn, float& Vrt)
{
	core::vector3df Vr((1 - a)*p1->v + a*p2->v - (1 - b)*p3->v - b*p4->v);
	Vrn = Vr.dotProduct(N);
	Vrt = Vr.dotProduct(T);
}

core::vector3df CollisionsHandler::computeTangent(core::vector3df& N)
{
	core::vector3df U1(N.crossProduct(core::vector3df(0.0f, 0.0f, 1.0f)));
	core::vector3df U2(N.crossProduct(core::vector3df(0.0f, 1.0f, 0.0f)));
	if (U1.getLengthSQ() > U2.getLengthSQ()) {
		return U1.normalize();
	}
	else {
		return U2.normalize();
	}
}

void CollisionsHandler::computeNewPos(Particle* p1, Particle* p2, Particle* p3, Particle* p4, float t,
                                      core::vector3df& newP1, core::vector3df& newP2, core::vector3df& newP3, core::vector3df& newP4)
{
	newP1 = p1->p + p1->v*t;
	newP2 = p2->p + p2->v*t;
	newP3 = p3->p + p3->v*t;
	newP4 = p4->p + p4->v*t;
}

float CollisionsHandler::solveCollisionTime(Particle* p1, Particle* p2, Particle* p3, Particle* p4, float dt)
{
	core::vector3df x21(p2->p - p1->p);
	core::vector3df x31(p3->p - p1->p);
	core::vector3df x41(p4->p - p1->p);
	core::vector3df v21(p2->v - p1->v);
	core::vector3df v31(p3->v - p1->v);
	core::vector3df v41(p4->v - p1->v);

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
	float rootsGood[3];
	rootsGood[0] = 1E16f;
	rootsGood[1] = 1E16f;
	rootsGood[2] = 1E16f;
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

core::vector3df CollisionsHandler::computeNormalTriangle(core::vector3df& p, core::vector3df& pA, core::vector3df& pB, core::vector3df& pC)
{
	core::vector3df U(pB - pA);
	core::vector3df V(pC - pA);
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

void CollisionsHandler::computeBarycentricCoords(core::vector3df& p, core::vector3df& pA, core::vector3df& pB, core::vector3df& pC, float& u, float& v, float& w)
{
	core::vector3df v0(pB - pA);
	core::vector3df v1(pC - pA);
	core::vector3df v2(p - pA);
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

bool CollisionsHandler::pointPartOfTriangle(Particle* p, Particle* pA, Particle* pB, Particle* pC)
{
	return p == pA || p == pB || p == pC;
}

bool CollisionsHandler::edgesSharePoint(Particle* p1, Particle* p2, Particle* p3, Particle* p4)
{
	assert( p1 != p2 && p3 != p4 );
	return p1 == p3 || p1 == p4 || p2 == p3 || p2 == p4;
}

/*
	From: Real Time Collision Detection, Christer Ericson, p.47
*/
bool CollisionsHandler::testPointTriangle(core::vector3df& p, core::vector3df& pA, core::vector3df& pB, core::vector3df& pC)
{
	core::vector3df x43(p - pC);
	core::vector3df n = computeNormalTriangle(p, pA, pB, pC);
	if ( abs(x43.dotProduct(n)) >= H ) return false;

	float u, v, w;
	computeBarycentricCoords(p, pA, pB, pC, u, v, w);

	// Check if point is in triangle
	return (v >= 0.0f - DELTA) && (w >= 0.0f - DELTA) && (v + w <= 1.0f + DELTA);
}

/*
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