#include "ClothSceneNode.h"

ClothSceneNode::ClothSceneNode(scene::ISceneNode* parent, scene::ISceneManager* mgr, s32 id):
	scene::ISceneNode(parent, mgr, id)
{
	m_material.Wireframe = false;
	m_material.Lighting = false;

	m_vertices[0] = video::S3DVertex(0, 0, 10, 1, 1, 0, video::SColor(255, 0, 255, 255), 0, 1);
	m_vertices[1] = video::S3DVertex(10, 0, -10, 1, 0, 0, video::SColor(255, 255, 0, 255), 1, 1);
	m_vertices[2] = video::S3DVertex(0, 20, 0, 0, 1, 1, video::SColor(255, 255, 255, 0), 1, 0);
	m_vertices[3] = video::S3DVertex(-10, 0, -10, 0, 0, 1, video::SColor(255, 0, 255, 0), 0, 0);

	m_box.reset(m_vertices[0].Pos);
	for (s32 i = 1; i < 4; ++i) {
		m_box.addInternalPoint(m_vertices[i].Pos);
	}
}

void ClothSceneNode::OnRegisterSceneNode()
{
	if (IsVisible) {
		SceneManager->registerNodeForRendering(this);
	}

	ISceneNode::OnRegisterSceneNode();
}

void ClothSceneNode::render()
{
	u16 indices[] = { 0,2,3, 2,1,3, 1,0,3, 2,0,1 };
	video::IVideoDriver* driver = SceneManager->getVideoDriver();

	driver->setMaterial(m_material);
	driver->setTransform(video::ETS_WORLD, AbsoluteTransformation);
	driver->drawVertexPrimitiveList(&m_vertices[0], 4, &indices[0], 4, video::EVT_STANDARD, scene::EPT_TRIANGLES, video::EIT_16BIT);
}