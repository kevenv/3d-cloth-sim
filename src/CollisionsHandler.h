#ifndef COLLISIONS_HANDLER_H
#define COLLISIONS_HANDLER_H

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
};

#endif
