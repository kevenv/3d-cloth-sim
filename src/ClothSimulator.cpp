#include "ClothSimulator.h"

#include "Cloth.h"

ClothSimulator::ClothSimulator()
{

}

ClothSimulator::~ClothSimulator()
{

}

void ClothSimulator::init()
{

}

void ClothSimulator::close()
{
	for (int i = 0; i < m_Cloths.size(); ++i) {
		delete m_Cloths[i];
	}
}

void ClothSimulator::update()
{
	float dt = 0.01f;
	float kd = 0.01f;
	float g = 9.81f;
	
	for (int i = 0; i < m_Particles.size(); ++i) {
		Particle& p = *m_Particles[i];
		// gravity force
		p.addForce( p.mass * core::vector3df(0.0f, -g, 0.0f) );
		// viscous drag
		p.addForce( -kd * p.v );

		p.addForce(core::vector3df(-2.0f,0.0f,-2.0f));
	}

	// spring forces
	for (int i = 0; i < m_Springs.size(); ++i) {
		m_Springs[i]->apply();
	}

	// update velocities (symplectic euler)
	for (int i = 0; i < m_Particles.size(); ++i) {
		Particle& p = *m_Particles[i];
		if (p.pinned) {
			p.f.set(0.0f, 0.0f, 0.0f);
			p.v.set(0.0f, 0.0f, 0.0f);
		}
		else {
			p.v += (p.f / p.mass) * dt;
		}
	}

	// collision responses
	m_CollisionsHandler.handleCollisions(*this, dt);

	// update positions (symplectic euler)
	for (int i = 0; i < m_Particles.size(); ++i) {
		Particle& p = *m_Particles[i];
		if (p.pinned) continue;
		p.p += p.v * dt;
		p.f.set(0.0f, 0.0f, 0.0f);
	}
}

void ClothSimulator::addCloth(Cloth* cloth)
{
	m_Cloths.push_back(cloth);

	std::vector<Particle>& p = cloth->getParticles();
	for (int i = 0; i < p.size(); ++i) {
		m_Particles.push_back(&p[i]);
	}
	std::vector<Spring>& s = cloth->getSprings();
	for (int i = 0; i < s.size(); ++i) {
		m_Springs.push_back(&s[i]);
	}

	//2 triangles per quad, 3 pts per triangle
	int width = cloth->getWidth();
	int height = cloth->getHeight();
	float nVertices = p.size();
	int nTriangles = 2 * ((width - 1)*(height - 1));
	float nIndices = 3 * nTriangles;
	for (int y = 0; y < height - 1; ++y) {
		for (int x = 0; x < width - 1; ++x) {
			m_TriangleParticles.push_back(&p[cloth->idx2Dto1D(x, y)]);
			m_TriangleParticles.push_back(&p[cloth->idx2Dto1D(x + 1, y)]);
			m_TriangleParticles.push_back(&p[cloth->idx2Dto1D(x, y + 1)]);

			m_TriangleParticles.push_back(&p[cloth->idx2Dto1D(x, y + 1)]);
			m_TriangleParticles.push_back(&p[cloth->idx2Dto1D(x + 1, y)]);
			m_TriangleParticles.push_back(&p[cloth->idx2Dto1D(x + 1, y + 1)]);
		}
	}
}