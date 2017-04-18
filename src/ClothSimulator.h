#ifndef CLOTH_SIMULATOR_H
#define CLOTH_SIMULATOR_H

#include <vector>
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

	void addCloth();
	inline const std::vector<Cloth*> getCloths() const { return m_Cloths; }
	inline const std::vector<Particle*> getParticles() const { return m_Particles; }
	inline const std::vector<Spring*> getSprings() const { return m_Springs; }

private:
	std::vector<Cloth*> m_Cloths;
	std::vector<Particle*> m_Particles;
	std::vector<Spring*> m_Springs;

	CollisionsHandler m_CollisionsHandler;
};

#endif
