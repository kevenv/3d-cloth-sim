
#include <iostream>
#include <irrlicht.h>
#include "driverChoice.h"
#include "COrientationAxisSceneNode.h"
#include "ClothRenderer.h"
#include "ClothSimulator.h"

using namespace irr;

#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif


class MyEventReceiver : public IEventReceiver
{
public:

	MyEventReceiver(IrrlichtDevice* device) :
		device(device)
	{

	}

	bool OnEvent(const SEvent& event)
	{
		if (event.EventType == irr::EET_KEY_INPUT_EVENT && !event.KeyInput.PressedDown)
		{
			switch (event.KeyInput.Key)
			{
			case irr::KEY_ESCAPE:
				device->closeDevice();
				return true;
			default:
				break;
			}
		}

		return false;
	}

private:
	IrrlichtDevice* device;
};

int main()
{
	// force OpenGL
	video::E_DRIVER_TYPE driverType = video::E_DRIVER_TYPE::EDT_OPENGL;

	// create device with full flexibility over creation parameters
	// you can add more parameters if desired, check irr::SIrrlichtCreationParameters
	irr::SIrrlichtCreationParameters params;
	params.DriverType = driverType;
	params.WindowSize = core::dimension2d<u32>(800, 600);
	IrrlichtDevice* device = createDeviceEx(params);
	if (device == 0)
		return 1; // could not create selected driver.
		
	/*
	First, we add standard stuff to the scene: A nice irrlicht engine
	logo, a small help text, a user controlled camera, and we disable
	the mouse cursor.
	*/
	video::IVideoDriver* driver = device->getVideoDriver();
	scene::ISceneManager* smgr = device->getSceneManager();
	gui::IGUIEnvironment* env = device->getGUIEnvironment();

	driver->setTextureCreationFlag(video::ETCF_ALWAYS_32_BIT, true);

	// add camera
	scene::ICameraSceneNode* camera = smgr->addCameraSceneNodeFPS(0, 100.0f, 1.2f);
	//smgr->addCameraSceneNodeMaya(0, 100.0f, 1.2f);
	camera->setPosition(core::vector3df(0,750,-879));
	camera->setTarget(core::vector3df(13, 105, 80));
	camera->setFarValue(42000.0f);

	// disable mouse cursor
	device->getCursorControl()->setVisible(false);

	// floor
	core::dimension2df tileSize(256,256);
	core::dimension2du tileCount(10,10);
	core::dimension2df textureRepeatCount(10,10);
	scene::IMesh* floorMesh = smgr->getGeometryCreator()->createPlaneMesh(tileSize, tileCount, 0, textureRepeatCount);
	scene::IMeshSceneNode* floorNode = smgr->addMeshSceneNode(floorMesh, 0, -1, core::vector3df(0, 0, 0));
	floorNode->setMaterialTexture(0, driver->getTexture("../assets/floor.png"));
	floorNode->setMaterialType(video::EMT_SOLID);
	floorNode->setMaterialFlag(video::EMF_LIGHTING, false);

	// cube
	scene::IMesh* cubeMesh = smgr->getGeometryCreator()->createCubeMesh(core::vector3df(1.0f, 1.0f, 1.0f));
	scene::IMeshSceneNode* cubeNode = smgr->addMeshSceneNode(cubeMesh, 0, -1, core::vector3df(0, 0, 0));
	cubeNode->setScale(core::vector3df(250, 250, 250));
	cubeNode->setPosition(core::vector3df(0,0,0));
	cubeNode->setMaterialType(video::EMT_SOLID);
	cubeNode->setMaterialFlag(video::EMF_LIGHTING, false);

	// create skydome
	driver->setTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS, false);
	scene::ISceneNode* skydome=smgr->addSkyDomeSceneNode(driver->getTexture("../irrlicht/media/skydome.jpg"),16,8,0.95f,2.0f);
	driver->setTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS, true);

	// axis
	scene::COrientationAxisSceneNode* axisNode = new scene::COrientationAxisSceneNode(smgr->getRootSceneNode(), smgr);
	axisNode->setScale(core::vector3df(300, 300, 300));
	axisNode->setPosition(core::vector3df(0,50,0));
	
	// light is just for nice effects
	scene::ILightSceneNode *node = smgr->addLightSceneNode(0, core::vector3df(0, 100, 0),
		video::SColorf(1.0f, 0.6f, 0.7f, 1.0f), 500.0f);
	if (node)
	{
		node->getLightData().Attenuation.set(0.f, 1.f / 500.f, 0.f);
		scene::ISceneNodeAnimator* anim = smgr->createFlyCircleAnimator(core::vector3df(0, 150, 0), 250.0f);
		if (anim)
		{
			node->addAnimator(anim);
			anim->drop();
		}
	}

	// Cloth
	ClothSimulator clothSimulator;
	clothSimulator.init();
	clothSimulator.addCloth();
	ClothRenderer clothRenderer(smgr);
	clothRenderer.init(clothSimulator.getCloths());

	// create event receiver
	MyEventReceiver receiver(device);
	device->setEventReceiver(&receiver);

	/*
	That's it, draw everything.
	*/

	int lastFPS = -1;

	while(device->run())
	if (device->isWindowActive())
	{
		clothSimulator.update();
		clothRenderer.update();

		driver->beginScene(true, true, 0 );
		smgr->drawAll();
		env->drawAll();
		driver->endScene();

		/*const core::vector3df pos = camera->getPosition();
		const core::vector3df tgt = camera->getTarget();
		std::cout << "pos= " << pos.X << "," << pos.Y << "," << pos.Z << std::endl;
		std::cout << "tgt= " << tgt.X << "," << tgt.Y << "," << tgt.Z << std::endl;*/

		// display frames per second in window title
		int fps = driver->getFPS();
		if (lastFPS != fps)
		{
			core::stringw str = L"Terrain Renderer - Irrlicht Engine [";
			str += driver->getName();
			str += "] FPS:";
			str += fps;
			device->setWindowCaption(str.c_str());
			lastFPS = fps;
		}
	}
	
	clothSimulator.close();
	clothRenderer.close();
	axisNode->drop();
	floorMesh->drop();
	cubeMesh->drop();
	device->drop();
	
	return 0;
}
