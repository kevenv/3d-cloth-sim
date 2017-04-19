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
}