#ifndef CLOTH_SIMULATOR_H
#define CLOTH_SIMULATOR_H

#include <vector>

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
	const std::vector<Cloth*> getCloths() const { return m_Cloths; }

private:
	std::vector<Cloth*> m_Cloths;
	std::vector<Particle*> m_Particles;
	std::vector<Spring*> m_Springs;
};

#endif
