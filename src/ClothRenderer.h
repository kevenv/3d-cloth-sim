#ifndef CLOTH_RENDERER_H
#define CLOTH_RENDERER_H

#include <irrlicht.h>
using namespace irr;
#include <vector>

class ClothSceneNode;
class Cloth;

class ClothRenderer
{
public:
	ClothRenderer(scene::ISceneManager* smgr);
	~ClothRenderer();

	void init(const std::vector<Cloth*>& cloths);
	void close();
	void update();

private:
	scene::ISceneManager* m_smgr;
	std::vector<ClothSceneNode*> m_ClothNodes;
};

#endif
