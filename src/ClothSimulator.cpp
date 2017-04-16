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

}

void ClothSimulator::addCloth()
{
	Cloth* cloth = new Cloth();
	cloth->generate(20, 25, 2.5f);
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