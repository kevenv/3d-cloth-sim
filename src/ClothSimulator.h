#ifndef CLOTH_SIMULATOR_H
#define CLOTH_SIMULATOR_H

#include <vector>
#include <irrlicht.h>
using namespace irr;
#include "CollisionsHandler.h"

class Particle;
class Spring;
class Cloth;

class ClothSimulator
{
public:
	ClothSimulator();
	~ClothSimulator();

	void init();
	void close();
	void update();

	void addCloth(Cloth* cloth);
	void addObject(scene::IMesh* mesh);
	void addTestParticle(Particle* particle);
	void addTestSpring(Spring* spring);
	void addTestTriangle(Particle* pA, Particle* pB, Particle* pC);

	inline const std::vector<Cloth*>& getCloths() const { return m_Cloths; }
	inline const std::vector<Particle*>& getTestParticles() const { return m_TestParticles; }
	inline const std::vector<Spring*>& getTestSprings() const { return m_TestSprings; }
	
	inline const std::vector<Particle*>& getPoints() const { return m_Points; }
	inline const std::vector<Particle*>& getTriangles() const { return m_Triangles; }
	inline const std::vector<Spring*>& getEdges() const { return m_Edges; }

	bool m_CollisionsEnabled;
	bool m_Reset;

private:
	// for particles system simulation
	std::vector<Particle*> m_Particles;
	std::vector<Spring*> m_Springs;

	// for collision detection
	CollisionsHandler m_CollisionsHandler;

	std::vector<Particle*> m_Points;
	std::vector<Particle*> m_Triangles;
	std::vector<Spring*> m_Edges;

	// objects data (Owner)
	/*
                    Cloth    Object    Test
        Simulation  x                  x
        Collisions  x        x         x
	*/
	std::vector<Cloth*> m_Cloths;

	std::vector<Particle*> m_ObjectParticles;
	std::vector<Spring*> m_ObjectSprings;

	std::vector<Particle*> m_TestParticles;
	std::vector<Spring*> m_TestSprings;
};

#endif
