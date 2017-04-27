#ifndef TEST_SYSTEMS_H
#define TEST_SYSTEMS_H

#include "irrlicht.h"
using namespace irr;

class ClothSimulator;

class TestSystems
{
public:
	TestSystems();

	void load(ClothSimulator* sim, scene::ICameraSceneNode* camera);

private:
};

#endif