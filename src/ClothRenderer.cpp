#include "ClothRenderer.h"

#include "ClothSceneNode.h"

ClothRenderer::ClothRenderer(scene::ISceneManager* smgr):
	m_smgr(smgr)
{

}

ClothRenderer::~ClothRenderer()
{

}

void ClothRenderer::init(const std::vector<Cloth*>& cloths)
{
	for (int i = 0; i < cloths.size(); ++i) {
		Cloth* cloth = cloths[i];
		ClothSceneNode* clothNode = new ClothSceneNode(cloth, m_smgr->getRootSceneNode(), m_smgr);
		clothNode->setScale(core::vector3df(10, 10, 10));
		clothNode->setPosition(core::vector3df(-200,100,-100));
		m_ClothNodes.push_back(clothNode);
	}
}

void ClothRenderer::close()
{
	for (int i = 0; i < m_ClothNodes.size(); ++i) {
		m_ClothNodes[i]->drop(); //delete
	}
}

void ClothRenderer::update()
{
	for (int i = 0; i < m_ClothNodes.size(); ++i) {
		m_ClothNodes[i]->update();
	}
}
