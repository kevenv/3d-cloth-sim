
#include <iostream>
#include <irrlicht.h>
#include "driverChoice.h"
#include "COrientationAxisSceneNode.h"
#include "ClothRenderer.h"
#include "ClothSimulator.h"
#include "Cloth.h"
#include "TestSystems.h"
#include "TestSystemRenderer.h"

using namespace irr;

#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif


class MyEventReceiver : public IEventReceiver
{
public:
	// We'll create a struct to record info on the mouse state
	struct SMouseState
	{
		core::position2di oldPosition;
		core::position2di position;
		core::position2di dv;
		bool LeftButtonDown;
		bool Moved;
		SMouseState() : LeftButtonDown(false), Moved(false) { }
	} MouseState;

	MyEventReceiver(IrrlichtDevice* device, scene::ICameraSceneNode* camera, scene::IMeshSceneNode* lightNode, scene::COrientationAxisSceneNode* axisNode,
                    Cloth* cloth) :
		device(device),
		camera(camera),
		lightNode(lightNode),
		axisNode(axisNode),
		cloth(cloth)
	{

	}

	bool OnEvent(const SEvent& event)
	{
		// Remember the mouse state
		if (event.EventType == irr::EET_MOUSE_INPUT_EVENT)
		{
			switch (event.MouseInput.Event)
			{
			case EMIE_LMOUSE_PRESSED_DOWN:
				MouseState.LeftButtonDown = true;
				break;

			case EMIE_LMOUSE_LEFT_UP:
				MouseState.LeftButtonDown = false;
				break;

			case EMIE_MOUSE_MOVED:
				MouseState.oldPosition = MouseState.position;
				
				MouseState.position.X = event.MouseInput.X;
				MouseState.position.Y = event.MouseInput.Y;

				if (!MouseState.oldPosition.equals(MouseState.position)) {
					MouseState.Moved = true;
					MouseState.dv = MouseState.oldPosition - MouseState.position;
				}
				else {
					MouseState.Moved = false;
					MouseState.dv.X = 0;
					MouseState.dv.Y = 0;
				}
				
				break;

			default:
				// We won't use the wheel
				break;
			}
		}

		if (event.EventType == irr::EET_KEY_INPUT_EVENT && !event.KeyInput.PressedDown)
		{
			switch (event.KeyInput.Key)
			{
			case irr::KEY_KEY_C:
			{
				const core::vector3df pos = camera->getPosition();
				const core::vector3df tgt = camera->getTarget();
				std::cout << "pos= " << pos.X << "," << pos.Y << "," << pos.Z << std::endl;
				std::cout << "tgt= " << tgt.X << "," << tgt.Y << "," << tgt.Z << std::endl;
			}
			break;
			case irr::KEY_KEY_L:
				lightNode->setVisible(!lightNode->isVisible());
				break;
			case irr::KEY_KEY_A:
				axisNode->setVisible(!axisNode->isVisible());
				break;
			case irr::KEY_KEY_D:
				cloth->pinUpCorners(false);
				break;
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
	scene::ICameraSceneNode* camera;
	scene::IMeshSceneNode* lightNode;
	scene::COrientationAxisSceneNode* axisNode;
	Cloth* cloth;
};

void update3DPicking(MyEventReceiver& receiver, Particle* selectedParticle, core::vector2di& clickPos, scene::ISceneManager* smgr, IrrlichtDevice* device, scene::ICameraSceneNode* camera, ClothRenderer& clothRenderer);

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
	//scene::ICameraSceneNode* camera = smgr->addCameraSceneNodeMaya(0, 100.0f, 1.2f);
	camera->setPosition(core::vector3df(44,391,459));
	camera->setTarget(core::vector3df(38, 107, -75));
	camera->setFarValue(42000.0f);

	// disable mouse cursor
	device->getCursorControl()->setVisible(false);

	// floor
	core::dimension2df tileSize(256*10,256*10);
	core::dimension2du tileCount(1,1);
	core::dimension2df textureRepeatCount(10,10);
	scene::IMesh* floorMesh = smgr->getGeometryCreator()->createPlaneMesh(tileSize, tileCount, 0, textureRepeatCount);
	scene::IMeshSceneNode* floorNode = smgr->addMeshSceneNode(floorMesh, 0, -1, core::vector3df(0, 0, 0));
	floorNode->setMaterialTexture(0, driver->getTexture("../assets/floor.png"));
	floorNode->setMaterialType(video::EMT_SOLID);
	floorNode->setMaterialFlag(video::EMF_LIGHTING, false);

	// sphere
	scene::IMesh* sphereMesh = smgr->getGeometryCreator()->createSphereMesh(150.0f, 8, 8);
	scene::IMeshSceneNode* sphereNode = smgr->addMeshSceneNode(sphereMesh, 0, -1, core::vector3df(0, 0, 0));
	sphereNode->setPosition(core::vector3df(0, 0, 0));
	sphereNode->setMaterialType(video::EMT_SOLID);
	sphereNode->setMaterialFlag(video::EMF_LIGHTING, false);
	smgr->getMeshManipulator()->setVertexColors(sphereMesh, video::SColor(255, 163, 73, 164));
	smgr->getMeshManipulator()->recalculateNormals(sphereMesh, true, false);

	// create skydome
	driver->setTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS, false);
	scene::ISceneNode* skydome=smgr->addSkyDomeSceneNode(driver->getTexture("../irrlicht/media/skydome.jpg"),16,8,0.95f,2.0f);
	driver->setTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS, true);

	// axis
	scene::COrientationAxisSceneNode* axisNode = new scene::COrientationAxisSceneNode(smgr->getRootSceneNode(), smgr);
	axisNode->setScale(core::vector3df(300, 300, 300));
	axisNode->setPosition(core::vector3df(0,50,0));
	
	// light
	scene::ILightSceneNode *node = smgr->addLightSceneNode(0, core::vector3df(-265, 364, 264), video::SColorf(1.0f, 0.6f, 0.7f, 1.0f), 500.0f);
	node->getLightData().Attenuation.set(0.f, 1.f / 500.f, 0.f);
	scene::IMesh* lightMesh = smgr->getGeometryCreator()->createSphereMesh(50.0f, 4, 4);
	scene::IMeshSceneNode* lightNode = smgr->addMeshSceneNode(lightMesh, 0, -1, node->getPosition());
	//lightNode->setScale(core::vector3df(node->getRadius()));
	lightNode->setMaterialType(video::EMT_SOLID);
	lightNode->setMaterialFlag(video::EMF_LIGHTING, false);
	smgr->getMeshManipulator()->setVertexColors(lightMesh, video::SColor(255, 0, 255, 0));
	
	// Cloth
	ClothSimulator clothSimulator;
	clothSimulator.init();
	clothSimulator.addObject(sphereMesh);
	//clothSimulator.addObject(floorMesh);
	TestSystems test;
	//test.load(&clothSimulator, camera);
	TestSystemRenderer testRenderer(smgr);
	//testRenderer.init(clothSimulator.getTestParticles(), clothSimulator.getTestSprings(), clothSimulator.getTriangles());
	Cloth* cloth = new Cloth();
	cloth->setPosition(core::vector3df(-50, 200, 20));
	cloth->setRotation(core::vector3df(0, 0, 0));
	cloth->generate(8, 9, 2.5f);
	cloth->pinUpCorners();
	clothSimulator.addCloth(cloth);
	/*
	{
		Cloth* cloth = new Cloth();
		cloth->setPosition(core::vector3df(-200, 100, 00));
		cloth->setRotation(core::vector3df(-90, 0, 0));
		cloth->generate(8, 9, 2.5f);
		cloth->pinAll();
		clothSimulator.addCloth(cloth);
	}
	*/
	ClothRenderer clothRenderer(smgr, true /*draw particles*/);
	clothRenderer.init(clothSimulator.getCloths());

	// create event receiver
	MyEventReceiver receiver(device, camera, lightNode, axisNode, cloth);
	device->setEventReceiver(&receiver);

	// 3D picking
	scene::ISceneCollisionManager* collMan = smgr->getSceneCollisionManager();
	Particle* selectedParticle = NULL;
	core::vector2di clickPos;

	/*
	That's it, draw everything.
	*/

	int lastFPS = -1;

	while(device->run())
	if (device->isWindowActive())
	{
		//update3DPicking(receiver, selectedParticle, clickPos, smgr, device, camera, clothRenderer);
		clothSimulator.update();
		clothRenderer.update();
		testRenderer.update();

		driver->beginScene(true, true, 0 );
		smgr->drawAll();
		env->drawAll();
		testRenderer.render(driver);
		driver->endScene();

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
	testRenderer.close();
	axisNode->drop();
	floorMesh->drop();
	sphereMesh->drop();
	lightMesh->drop();
	device->drop();
	
	return 0;
}

void update3DPicking(MyEventReceiver& receiver, Particle* selectedParticle, core::vector2di& clickPos, scene::ISceneManager* smgr, IrrlichtDevice* device, scene::ICameraSceneNode* camera, ClothRenderer& clothRenderer)
{
	scene::ISceneCollisionManager* collMan = smgr->getSceneCollisionManager();
	
	if (receiver.MouseState.LeftButtonDown) {
		core::line3d<f32> ray = collMan->getRayFromScreenCoordinates(device->getCursorControl()->getPosition(), camera);
		core::vector3df intersection; // Tracks the current intersection point with the level or a mesh
		core::triangle3df hitTriangle; // Used to show with triangle has been hit
		scene::ISceneNode * selectedSceneNode = collMan->getSceneNodeAndCollisionPointFromRay(
			ray,
			intersection, // This will be the position of the collision
			hitTriangle, // This will be the triangle hit in the collision
			1, // This ensures that only nodes that we have
			   // set up to be pickable are considered
			0); // Check the entire scene (this is actually the implicit default)
		if (!selectedParticle && selectedSceneNode)
		{
			selectedParticle = clothRenderer.getParticleFromNode(selectedSceneNode);
			if (selectedParticle) {
				camera->setInputReceiverEnabled(false);
				clickPos = receiver.MouseState.position;
			}
		}
	}
	else {
		selectedParticle = NULL;
		camera->setInputReceiverEnabled(true);
	}

	if (selectedParticle && receiver.MouseState.Moved) {
		core::vector2di dv = clickPos - receiver.MouseState.position;

		core::matrix4 viewMat;
		core::matrix4 projMat;

		/*float x = (2.0f * dv.X) / 800 - 1.0f;
		float y = 1.0f - (2.0f * dv.Y) / 600;
		float z = -1.0f;
		core::vector3df ray_clip(x, y, z);

		camera->getProjectionMatrix().getInverse(projMat);
		camera->getViewMatrix().getInverse(viewMat);
		projMat.transformVect(ray_clip);
		ray_clip.Z = -1.0f;
		viewMat.transformVect(ray_clip);
		ray_clip.normalize();*/

		camera->getProjectionMatrix().getInverse(projMat);
		camera->getViewMatrix().getInverse(viewMat);
		projMat *= viewMat;

		core::vector3df v(dv.X, dv.Y, 0.0f);
		projMat.transformVect(v);

		std::cout << v.X << "," << v.Y << "," << v.Z << std::endl;
		selectedParticle->addForce(v*2.5f);
	}
}