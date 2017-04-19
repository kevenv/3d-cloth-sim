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

	inline std::vector<Spring>& getSprings() { return m_Springs; }
	inline std::vector<Particle>& getParticles() { return m_Particles; }
	inline int getWidth() { return m_Width; }
	inline int getHeight() { return m_Height; }
	inline float getResolution() { return m_Resolution; }

private:
	std::vector<Spring> m_Springs;
	std::vector<Particle> m_Particles;

	int m_Width;
	int m_Height;
	float m_Resolution;

	void addNeighbor(int x, int y, Particle* p1, float k, float b);
};

#endif
