#ifndef CLOTH_RENDERER_H
#define CLOTH_RENDERER_H

#include <vector>
#include <irrlicht.h>
using namespace irr;

class ClothSceneNode;
class Cloth;
class Particle;

class ClothRenderer
{
public:
	ClothRenderer(scene::ISceneManager* smgr, bool drawParticles);
	~ClothRenderer();

	void init(const std::vector<Cloth*>& cloths);
	void close();
	void update();

	Particle* getParticleFromNode(scene::ISceneNode* node);

private:
	scene::ISceneManager* m_smgr;
	std::vector<ClothSceneNode*> m_ClothNodes;

	bool m_DrawParticles;
	std::vector<Particle*> m_Particles;
	std::vector<scene::IMeshSceneNode*> m_ParticleNodes;
	scene::IMesh* m_ParticleMesh;

	scene::IMeshSceneNode* createParticleNode();
};

#endif
