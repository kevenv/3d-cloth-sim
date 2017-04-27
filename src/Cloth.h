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
	Particle* getParticle(int x, int y);
	inline int getWidth() const { return m_Width; }
	inline int getHeight() const { return m_Height; }
	inline float getResolution() const { return m_Resolution; }
	inline void setPosition(const core::vector3df& position) { m_Position = position; }
	inline void setRotation(const core::vector3df& rotation) { m_Rotation = rotation; }
	void pinUpCorners(bool pinned = true);
	void pinDownCorners(bool pinned = true);
	void pinAllCorners(bool pinned = true);
	void pinAll(bool pinned = true);
	void pinUpLCorner(bool pinned = true);
	void pinUpRCorner(bool pinned = true);
	inline void idx1Dto2D(int idx, int& x, int& y)
	{
		x = idx % m_Width;
		y = idx / m_Width;
	}
	inline int idx2Dto1D(int x, int y)
	{
		return x + y * m_Width;
	}

private:
	std::vector<Spring> m_Springs;
	std::vector<Particle> m_Particles;

	int m_Width;
	int m_Height;
	float m_Resolution;

	core::vector3df m_Position;
	core::vector3df m_Rotation;

	void addNeighbor(int x, int y, Particle* p1, float k, float b, bool edge);
};

#endif
