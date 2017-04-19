#include "Cloth.h"

Cloth::Cloth():
	m_Width(0),
	m_Height(0),
	m_Resolution(1.0f)
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

	for (int i = 0; i < m_Particles.size(); ++i) {
		Particle* p = &m_Particles[i];
		int x = i % width;
		int y = i / width;

		// create structural springs
		addNeighbor(x-1, y, p);
		addNeighbor(x+1, y, p);
		addNeighbor(x, y-1, p);
		addNeighbor(x, y+1, p);

		// create shear springs
		addNeighbor(x-1, y-1, p);
		addNeighbor(x+1, y-1, p);
		addNeighbor(x-1, y+1, p);
		addNeighbor(x+1, y+1, p);

		//TODO: flexion springs
	}
}

void Cloth::addNeighbor(int x, int y, Particle* p1)
{
	if (x >= 0 && x < m_Width && y >= 0 && y < m_Height) {
		int i = x + y*m_Width;
		Particle* p2 = &m_Particles[i];
		m_Springs.push_back(Spring(p1, p2));
	}
}