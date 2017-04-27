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

	for (int i = 0; i < m_TestParticles.size(); ++i) {
		delete m_TestParticles[i];
	}
	for (int i = 0; i < m_TestSprings.size(); ++i) {
		delete m_TestSprings[i];
	}

	for (int i = 0; i < m_ObjectParticles.size(); ++i) {
		delete m_ObjectParticles[i];
	}
	for (int i = 0; i < m_ObjectSprings.size(); ++i) {
		delete m_ObjectSprings[i];
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

		// wind
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
		if (s[i].isEdge()) {
			m_Edges.push_back(&s[i]);
		}
	}

	//2 triangles per quad, 3 pts per triangle
	int width = cloth->getWidth();
	int height = cloth->getHeight();
	int nVertices = p.size();
	int nTriangles = 2 * ((width - 1)*(height - 1));
	int nIndices = 3 * nTriangles;
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

void ClothSimulator::addObject(scene::IMesh* mesh)
{
	scene::SMeshBuffer* buffer = static_cast<scene::SMeshBuffer*>(mesh->getMeshBuffer(0));
	int nVertices = buffer->getVertexCount();
	int nIndices = buffer->getIndexCount();

	// create Particles (pinned)
	for (int i = 0; i < nVertices; ++i) {
		video::S3DVertex& v = buffer->Vertices[i];
		Particle* p = new Particle(v.Pos.X, v.Pos.Y, v.Pos.Z, 0.0f, 0.0f, 0.0f);
		p->pinned = true;
		m_ObjectParticles.push_back(p);
		m_Particles.push_back(p);
	}

	// create triangles list
	// create edges list
	for (int i = 0; i < nIndices; i+=3) {
		int idxA = buffer->Indices[i + 0];
		int idxB = buffer->Indices[i + 1];
		int idxC = buffer->Indices[i + 2];
		Particle* p1 = m_ObjectParticles[idxA];
		Particle* p2 = m_ObjectParticles[idxB];
		Particle* p3 = m_ObjectParticles[idxC];

		// create triangles list
		m_TriangleParticles.push_back(p1);
		m_TriangleParticles.push_back(p2);
		m_TriangleParticles.push_back(p3);

		// create Springs
		Spring* s1 = new Spring(p1, p2, 1.0f, 1.0f, true);
		Spring* s2 = new Spring(p2, p3, 1.0f, 1.0f, true);
		Spring* s3 = new Spring(p3, p1, 1.0f, 1.0f, true);
		m_ObjectSprings.push_back(s1);
		m_ObjectSprings.push_back(s2);
		m_ObjectSprings.push_back(s3);
		m_Springs.push_back(s1);
		m_Springs.push_back(s2);
		m_Springs.push_back(s3);

		// create edges list
		m_Edges.push_back(s1);
		m_Edges.push_back(s2);
		m_Edges.push_back(s3);
	}
}

void ClothSimulator::addTestParticle(Particle* particle)
{
	m_TestParticles.push_back(particle);
	m_Particles.push_back(particle);
}

void ClothSimulator::addTestSpring(Spring* spring)
{
	m_TestSprings.push_back(spring);
	m_Springs.push_back(spring);
	m_Edges.push_back(spring);
}

void ClothSimulator::addTestTriangle(Particle* pA, Particle* pB, Particle* pC)
{
	m_TriangleParticles.push_back(pA);
	m_TriangleParticles.push_back(pB);
	m_TriangleParticles.push_back(pC);
}