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
	ClothRenderer(scene::ISceneManager* smgr);
	~ClothRenderer();

	void init(const std::vector<Cloth*>& cloths);
	void close();
	void update();

	Particle* getParticleFromNode(scene::ISceneNode* node);
	void setDrawParticles(bool value);
	inline bool getDrawParticles() const { return m_DrawParticles; }
	void setWireframeMode(bool value);
	inline bool getWireframeMode() const { return m_WireframeMode; }

private:
	scene::ISceneManager* m_smgr;
	std::vector<ClothSceneNode*> m_ClothNodes;
	bool m_WireframeMode;

	bool m_DrawParticles;
	std::vector<Particle*> m_Particles;
	std::vector<scene::IMeshSceneNode*> m_ParticleNodes;
	scene::IMesh* m_ParticleMesh;

	scene::IMeshSceneNode* createParticleNode();
};

#endif
