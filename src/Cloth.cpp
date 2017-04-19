#include "Cloth.h"

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
		core::matrix4 m;
		m.setRotationDegrees(m_Rotation);
		m.rotateVect(p->p);
		p->p *= 10.0f;
		p->p += m_Position;
	}

	for (int i = 0; i < m_Particles.size(); ++i) {
		Particle* p = &m_Particles[i];
		int x = i % width;
		int y = i / width;

		// create structural springs
		const float k_structural = 100.0f;
		const float b_structural = 10.0f;
		addNeighbor(x-1, y, p, k_structural, b_structural);
		addNeighbor(x+1, y, p, k_structural, b_structural);
		addNeighbor(x, y-1, p, k_structural, b_structural);
		addNeighbor(x, y+1, p, k_structural, b_structural);

		// create shear springs
		const float k_shear = 25.0f;
		const float b_shear = 1.0f;
		addNeighbor(x-1, y-1, p, k_shear, b_shear);
		addNeighbor(x+1, y-1, p, k_shear, b_shear);
		addNeighbor(x-1, y+1, p, k_shear, b_shear);
		addNeighbor(x+1, y+1, p, k_shear, b_shear);
		
		// create flexion springs
		const float k_flexion = 80.0f;
		const float b_flexion = 2.5f;
		addNeighbor(x-2, y, p, k_flexion, b_flexion);
		addNeighbor(x+2, y, p, k_flexion, b_flexion);
		addNeighbor(x, y-2, p, k_flexion, b_flexion);
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
		int i = x + y*m_Width;
		return &m_Particles[i];
	}
	else {
		return NULL;
	}
}