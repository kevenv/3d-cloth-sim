#ifndef CLOTH_RENDERER_H
#define CLOTH_RENDERER_H

#include <irrlicht.h>
using namespace irr;
#include <vector>

class ClothSceneNode;
class Cloth;
class Particle;

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
	std::vector<Particle*> m_Particles;
	std::vector<scene::IMeshSceneNode*> m_ParticleNodes;
	scene::IMesh* m_ParticleMesh;

	scene::IMeshSceneNode* createParticleNode();
};

#endif
