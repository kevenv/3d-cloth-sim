#include "ClothRenderer.h"

#include "ClothSceneNode.h"
#include "Cloth.h"
#include "Particle.h"

ClothRenderer::ClothRenderer(scene::ISceneManager* smgr):
	m_smgr(smgr),
	m_ParticleMesh(NULL)
{

}

ClothRenderer::~ClothRenderer()
{

}

void ClothRenderer::init(const std::vector<Cloth*>& cloths)
{
	m_ParticleMesh = m_smgr->getGeometryCreator()->createSphereMesh(5.0f);

	for (int i = 0; i < cloths.size(); ++i) {
		Cloth* cloth = cloths[i];
		ClothSceneNode* clothNode = new ClothSceneNode(cloth, m_smgr->getRootSceneNode(), m_smgr);
		m_ClothNodes.push_back(clothNode);
				
		auto& particles = cloth->getParticles();
		for (auto& p : particles) {
			scene::IMeshSceneNode* node = createParticleNode();
			node->setPosition(p.p);
			m_Particles.push_back(&p);
		}
	}
}

void ClothRenderer::close()
{
	for (int i = 0; i < m_ClothNodes.size(); ++i) {
		m_ClothNodes[i]->drop(); //delete
	}

	m_ParticleMesh->drop();
}

void ClothRenderer::update()
{
	for (int i = 0; i < m_ClothNodes.size(); ++i) {
		m_ClothNodes[i]->update();
	}
	
	for (int i = 0; i < m_Particles.size(); ++i) {
		m_ParticleNodes[i]->setPosition(m_Particles[i]->p);
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