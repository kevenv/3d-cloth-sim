#ifndef CLOTH_SCENE_NODE_H
#define CLOTH_SCENE_NODE_H

#include <irrlicht.h>

using namespace irr;

class ClothSceneNode : public scene::ISceneNode
{
public:
	ClothSceneNode(scene::ISceneNode* parent, scene::ISceneManager* mgr, s32 id = -1);

	virtual void OnRegisterSceneNode();
	virtual void render();

	virtual const core::aabbox3d<f32>& getBoundingBox() const { return m_box; }
	virtual u32 getMaterialCount() const { return 1; }
	virtual video::SMaterial& getMaterial(u32 i) { return m_material; }

private:
	core::aabbox3d<f32> m_box;
	video::S3DVertex m_vertices[4];
	video::SMaterial m_material;
};

#endif
