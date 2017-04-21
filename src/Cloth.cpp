#include "Cloth.h"

#include <cassert>

Cloth::Cloth():
	m_Width(0),
	m_Height(0),
	m_Resolution(1.0f),
	m_Position(0.0f,0.0f,0.0f),
	m_Rotation(0.0f,0.0f,0.0f)
{

}

void Cloth::clear()
{
	m_Particles.clear();
	m_Springs.clear();
	m_TriangleIndices.clear();
	m_Width = 0;
	m_Height = 0;
	m_Resolution = 1.0f;
}

void Cloth::generate(int width, int height, float resolution)
{
	clear();

	m_Width = width; //nParticles width
	m_Height = height; //nParticles height
	m_Resolution = resolution;

	assert( m_Resolution >= 0.0 ); // this would be stupid
	assert( m_Width >= 2 && m_Height >= 2); // not a cloth mesh

	// create particles
	for (int j = 0; j < height; ++j) {
		f32 y = j*resolution;
		for (int i = 0; i < width; ++i) {
			f32 x = i*resolution;
			m_Particles.push_back( Particle(x, y, 0, 0, 0, 0) );
		}
	}
	
	// transform to world
	for (int i = 0; i < m_Particles.size(); ++i) {
		Particle* p = &m_Particles[i];
		core::vector3df newP0(p->getP0());
		core::matrix4 m;
		m.setRotationDegrees(m_Rotation);
		m.rotateVect(newP0);
		p->setP0(newP0 * 10.0f + m_Position);
	}

	for (int i = 0; i < m_Particles.size(); ++i) {
		Particle* p = &m_Particles[i];
		int x, y;
		idx1Dto2D(i, x, y);

		// create structural springs
		const float k_structural = 150.0f;
		const float b_structural = 2.5f;
		addNeighbor(x+1, y, p, k_structural, b_structural);
		addNeighbor(x, y+1, p, k_structural, b_structural);

		// create shear springs
		const float k_shear = 100.0f;
		const float b_shear = 1.0f;
		addNeighbor(x-1, y+1, p, k_shear, b_shear);
		addNeighbor(x+1, y+1, p, k_shear, b_shear);
		
		// create flexion springs
		const float k_flexion = 100.0f;
		const float b_flexion = 1.0f;
		addNeighbor(x+2, y, p, k_flexion, b_flexion);
		addNeighbor(x, y+2, p, k_flexion, b_flexion);
	}
}

void Cloth::addNeighbor(int x, int y, Particle* p1, float k, float b)
{
	Particle* p2 = getParticle(x,y);
	if (p2) {
		m_Springs.push_back(Spring(p1, p2, k, b));
	}
}

Particle* Cloth::getParticle(int x, int y)
{
	if (x >= 0 && x < m_Width && y >= 0 && y < m_Height) {
		int i = idx2Dto1D(x,y);
		return &m_Particles[i];
	}
	else {
		return NULL;
	}
}

void Cloth::pinUpCorners(bool pinned)
{
	Particle* p0 = getParticle(m_Width-1, m_Height-1); p0->pinned = pinned;
	Particle* p1 = getParticle(0, m_Height-1); p1->pinned = pinned;
}

void Cloth::pinDownCorners(bool pinned)
{
	Particle* p2 = getParticle(0, 0); p2->pinned = pinned;
	Particle* p3 = getParticle(m_Width-1, 0); p3->pinned = pinned;
}

void Cloth::pinAllCorners(bool pinned)
{
	pinUpCorners(pinned);
	pinDownCorners(pinned);
}

void Cloth::pinAll(bool pinned)
{
	for (auto& p : m_Particles) {
		p.pinned = pinned;
	}
}

void Cloth::addTriangleIndex(int idx)
{
	m_TriangleIndices.push_back(idx);
}