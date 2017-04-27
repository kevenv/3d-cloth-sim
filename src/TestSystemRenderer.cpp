#include "TestSystemRenderer.h"

#include "Particle.h"
#include "Spring.h"

TestSystemRenderer::TestSystemRenderer(scene::ISceneManager* smgr):
	m_smgr(smgr)
{

}

void TestSystemRenderer::init(const std::vector<Particle*>& particles, const std::vector<Spring*>& springs)
{
	for (auto* p : particles) {
		scene::IMesh* mesh = m_smgr->getGeometryCreator()->createSphereMesh(5.0f);
		scene::IMeshSceneNode* node = m_smgr->addMeshSceneNode(mesh, 0, -1, core::vector3df(0, 0, 0));
		node->setMaterialType(video::EMT_SOLID);
		node->setMaterialFlag(video::EMF_LIGHTING, false);
		m_ParticleNodes.push_back(node);
		m_ParticleMeshs.push_back(mesh);

		node->setPosition(p->p);
		m_Particles.push_back(p);
	}

	for (auto* s : springs) {
		m_Springs.push_back(s);
	}
}

void TestSystemRenderer::close()
{
	for (auto* m : m_ParticleMeshs) {
		m->drop();
	}
}

void TestSystemRenderer::update()
{
	for (int i = 0; i < m_Particles.size(); ++i) {
		m_ParticleNodes[i]->setPosition(m_Particles[i]->p);
	}
}

void TestSystemRenderer::render(video::IVideoDriver* driver)
{
	video::SColor color(255, 255, 0, 0);
	float delta = 0.4f;
	core::vector3df dy(0.0f, delta, 0.0f);
	core::vector3df dz(0.0f, 0.0f, delta);

	for (auto* s : m_Springs) {
		auto* p1 = s->getP1();
		auto* p2 = s->getP2();
		core::vector3df start(p1->p);
		core::vector3df end(p2->p);
		driver->draw3DLine(start, end, color);

		//cheap trick to make the line bolder
		driver->draw3DLine(start - dy, end - dy, color);
		driver->draw3DLine(start + dy, end + dy, color);
		driver->draw3DLine(start - dz, end - dz, color);
		driver->draw3DLine(start + dz, end + dz, color);
	}
}