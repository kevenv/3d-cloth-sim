#ifndef CLOTH_SCENE_NODE_H
#define CLOTH_SCENE_NODE_H

#include <irrlicht.h>

using namespace irr;
class Cloth;

class ClothSceneNode : public scene::ISceneNode
{
public:
	ClothSceneNode(Cloth* cloth, scene::ISceneNode* parent, scene::ISceneManager* mgr, s32 id = -1);
	virtual ~ClothSceneNode();

	void update();

	virtual void OnRegisterSceneNode();
	virtual void render();

	virtual const core::aabbox3d<f32>& getBoundingBox() const { return m_box; }
	virtual u32 getMaterialCount() const { return 1; }
	virtual video::SMaterial& getMaterial(u32 i) { return m_material; }

private:
	core::aabbox3d<f32> m_box;
	video::SMaterial m_material;

	scene::SMesh* m_Mesh;
	Cloth* m_Cloth;

	int m_nVertices;
	int m_nIndices;

	int idx2dTo1d(int x, int y);
};

#endif
