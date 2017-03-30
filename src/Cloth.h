#ifndef CLOTH_H
#define CLOTH_H

#include <vector>
#include "Spring.h"
#include "Particle.h"

class Cloth
{
public:
	Cloth();

	void clear();
	void generate(int width, int height, float resolution);

	std::vector<Spring>& getSprings() { return m_Springs; }
	std::vector<Particle>& getParticles() { return m_Particles; }
	int getWidth() { return m_Width; }
	int getHeight() { return m_Height; }
	float getResolution() { return m_Resolution; }

private:
	std::vector<Spring> m_Springs;
	std::vector<Particle> m_Particles;

	int m_Width;
	int m_Height;
	float m_Resolution;

	void addNeighbor(int x, int y, Particle* p1);
};

#endif
