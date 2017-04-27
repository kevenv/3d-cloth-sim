#ifndef TEST_SYSTEM_RENDERER_H
#define TEST_SYSTEM_RENDERER_H

#include <vector>
#include <irrlicht.h>
using namespace irr;

class Particle;
class Spring;

class TestSystemRenderer
{
public:
	TestSystemRenderer(scene::ISceneManager* smgr);

	void init(const std::vector<Particle*>& particles, const std::vector<Spring*>& springs, const std::vector<Particle*> triangles);
	void close();
	void update();
	void render(video::IVideoDriver* driver);

private:
	scene::ISceneManager* m_smgr;

	std::vector<Particle*> m_Particles;
	std::vector<Spring*> m_Springs;
	std::vector<Particle*> m_Triangles;
	std::vector<scene::IMeshSceneNode*> m_ParticleNodes;
	std::vector<scene::IMesh*> m_ParticleMeshs;

	void drawLine(video::IVideoDriver* driver, core::vector3df& start, core::vector3df& end, video::SColor& color);
};

#endif