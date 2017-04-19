#include "CollisionsHandler.h"

#include "ClothSimulator.h"

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
	auto& cloths = clothSim.getCloths();
	for (int i = 0; i < cloths.size(); ++i) {
		Cloth* clothA = cloths[i];
		for (int i = 0; i < cloths.size(); ++i) {
			Cloth* clothB = cloths[i];
			// cloth too close to ITSELF or SOMETHING ELSE
			if (tooClose(clothA, clothB)) { 
				//apply repulsion force
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

		iterations++;
	}
}

bool CollisionsHandler::tooClose(Cloth* clothA, Cloth* clothB)
{
	return testPointTriangle(clothA, clothB) || testEdgeEdge(clothA, clothB);
}

bool CollisionsHandler::testPointTriangle(Cloth* clothA, Cloth* clothB)
{
	return false;
}

bool CollisionsHandler::testEdgeEdge(Cloth* clothA, Cloth* clothB)
{
	return false;
}

/*
	x0 = Point
	x1,x2,x3 = Triangle
*/
bool CollisionsHandler::testPointTriangle(core::vector3df & x0, core::vector3df& x1, core::vector3df& x2, core::vector3df& x3)
{
	return false;
}

/*
	x0,x1 = Edge 1
	x2,x3 = Edge 2
*/
bool CollisionsHandler::testEdgeEdge(core::vector3df & x0, core::vector3df & x1, core::vector3df & x2, core::vector3df & x3)
{
	return false;
}