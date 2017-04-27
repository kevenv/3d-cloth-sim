#include "ClothSimulator.h"

#include "Cloth.h"

ClothSimulator::ClothSimulator():
	m_CollisionsEnabled(true),
	m_Reset(false)
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
	for (auto* c : m_Cloths) {
		delete c;
	}

	for (auto* p : m_ObjectParticles) {
		delete p;
	}
	for (auto* s : m_ObjectSprings) {
		delete s;
	}

	for (auto* p : m_TestParticles) {
		delete p;
	}
	for (auto* s : m_TestSprings) {
		delete s;
	}
}

void ClothSimulator::update()
{
	float dt = 0.01f;
	float kd = 0.01f;
	float g = 9.81f;

	if (m_Reset) {
		for (auto* p : m_Particles) {
			p->reset();
		}
		m_Reset = false;
	}
	
	for (auto* p : m_Particles) {
		// gravity force
		p->addForce( p->mass * core::vector3df(0.0f, -g, 0.0f) );
		// viscous drag
		p->addForce( -kd * p->v );
		// wind
		p->addForce(core::vector3df(2.0f,0.0f,2.0f));
	}

	// spring forces
	for (auto* s : m_Springs) {
		s->apply();
	}

	// update velocities (symplectic euler)
	for (auto* p : m_Particles) {
		if (p->pinned) {
			p->f.set(0.0f, 0.0f, 0.0f);
			p->v.set(0.0f, 0.0f, 0.0f);
		}
		else {
			p->v += (p->f / p->mass) * dt;
		}
	}

	// collision responses
	if (m_CollisionsEnabled) {
		m_CollisionsHandler.handleCollisions(*this, dt);
	}

	// update positions (symplectic euler)
	for (auto* p : m_Particles) {
		if (p->pinned) continue;
		p->p += p->v * dt;
		p->f.set(0.0f, 0.0f, 0.0f);
	}
}

void ClothSimulator::addCloth(Cloth* cloth)
{
	m_Cloths.push_back(cloth);

	auto& particles = cloth->getParticles();
	for (auto& p : particles) {
		m_Particles.push_back(&p);
		m_Points.push_back(&p);
	}
	auto& springs = cloth->getSprings();
	for (auto& s : springs) {
		m_Springs.push_back(&s);
		if (s.isEdge()) {
			m_Edges.push_back(&s);
		}
	}

	//2 triangles per quad, 3 pts per triangle
	int width = cloth->getWidth();
	int height = cloth->getHeight();
	int nVertices = particles.size();
	int nTriangles = 2 * ((width - 1)*(height - 1));
	int nIndices = 3 * nTriangles;
	for (int y = 0; y < height - 1; ++y) {
		for (int x = 0; x < width - 1; ++x) {
			m_Triangles.push_back(&particles[cloth->idx2Dto1D(x, y)]);
			m_Triangles.push_back(&particles[cloth->idx2Dto1D(x + 1, y)]);
			m_Triangles.push_back(&particles[cloth->idx2Dto1D(x, y + 1)]);

			m_Triangles.push_back(&particles[cloth->idx2Dto1D(x, y + 1)]);
			m_Triangles.push_back(&particles[cloth->idx2Dto1D(x + 1, y)]);
			m_Triangles.push_back(&particles[cloth->idx2Dto1D(x + 1, y + 1)]);
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
		m_Points.push_back(p);
	}

	for (int i = 0; i < nIndices; i+=3) {
		int idxA = buffer->Indices[i + 0];
		int idxB = buffer->Indices[i + 1];
		int idxC = buffer->Indices[i + 2];
		Particle* p1 = m_ObjectParticles[idxA];
		Particle* p2 = m_ObjectParticles[idxB];
		Particle* p3 = m_ObjectParticles[idxC];

		// create triangles list
		m_Triangles.push_back(p1);
		m_Triangles.push_back(p2);
		m_Triangles.push_back(p3);

		// create Springs
		Spring* s1 = new Spring(p1, p2, 1.0f, 1.0f, true);
		Spring* s2 = new Spring(p2, p3, 1.0f, 1.0f, true);
		Spring* s3 = new Spring(p3, p1, 1.0f, 1.0f, true);
		m_ObjectSprings.push_back(s1);
		m_ObjectSprings.push_back(s2);
		m_ObjectSprings.push_back(s3);

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
	m_Points.push_back(particle);
}

void ClothSimulator::addTestSpring(Spring* spring)
{
	m_TestSprings.push_back(spring);
	m_Springs.push_back(spring);
	m_Edges.push_back(spring);
}

void ClothSimulator::addTestTriangle(Particle* pA, Particle* pB, Particle* pC)
{
	m_Triangles.push_back(pA);
	m_Triangles.push_back(pB);
	m_Triangles.push_back(pC);
}