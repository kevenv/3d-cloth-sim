#include "ClothRenderer.h"

#include "ClothSceneNode.h"
#include "Cloth.h"
#include "Particle.h"

ClothRenderer::ClothRenderer(scene::ISceneManager* smgr, bool drawParticles):
	m_smgr(smgr),
	m_ParticleMesh(NULL),
	m_DrawParticles(drawParticles)
{

}

ClothRenderer::~ClothRenderer()
{

}

void ClothRenderer::init(const std::vector<Cloth*>& cloths)
{
	if (m_DrawParticles) {
		m_ParticleMesh = m_smgr->getGeometryCreator()->createSphereMesh(5.0f);
	}

	for (auto* cloth : cloths) {
		ClothSceneNode* clothNode = new ClothSceneNode(cloth, m_smgr->getRootSceneNode(), m_smgr);
		m_ClothNodes.push_back(clothNode);
				
		if (m_DrawParticles) {
			auto& particles = cloth->getParticles();
			for (auto& p : particles) {
				scene::IMeshSceneNode* node = createParticleNode();
				node->setPosition(p.p);
				m_Particles.push_back(&p);
			}
		}
	}
}

void ClothRenderer::close()
{
	for (auto* n : m_ClothNodes) {
		n->drop(); //delete
	}

	if (m_DrawParticles) {
		m_ParticleMesh->drop();
	}
}

void ClothRenderer::update()
{
	for (auto* c : m_ClothNodes) {
		c->update();
	}
	
	if (m_DrawParticles) {
		for (int i = 0; i < m_Particles.size(); ++i) {
			m_ParticleNodes[i]->setPosition(m_Particles[i]->p);
		}
	}
}

scene::IMeshSceneNode* ClothRenderer::createParticleNode()
{
	scene::IMeshSceneNode* node = m_smgr->addMeshSceneNode(m_ParticleMesh, 0, -1, core::vector3df(0, 0, 0));
	node->setMaterialType(video::EMT_SOLID);
	node->setMaterialFlag(video::EMF_LIGHTING, false);
	node->setID(1); // PICKABLE_ID

	scene::ITriangleSelector* selector = m_smgr->createOctreeTriangleSelector(m_ParticleMesh, node);
	node->setTriangleSelector(selector);
	selector->drop();

	m_ParticleNodes.push_back(node);

	return node;
}

Particle* ClothRenderer::getParticleFromNode(scene::ISceneNode* node)
{
	for (int i = 0; i < m_ParticleNodes.size(); ++i) { //todo: use map instead
		if (m_ParticleNodes[i] == node) {
			return m_Particles[i];
		}
	}

	return nullptr;
}