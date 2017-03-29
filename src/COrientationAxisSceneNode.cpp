/*
   by vitek
   http://irrlicht.sourceforge.net/forum//viewtopic.php?t=16768

   modified by Keven Villeneuve
*/
#include "COrientationAxisSceneNode.h"
#include "ISceneManager.h"
#include "IVideoDriver.h"

namespace irr
{
namespace scene
{

COrientationAxisSceneNode::COrientationAxisSceneNode(ISceneNode* parent, ISceneManager* mgr, s32 id,
   const core::vector3df& position,
   const core::vector3df& rotation,
   const core::vector3df& scale)
   : ISceneNode(parent, mgr, id, position, rotation, scale)
{
   const video::SColor red  (255, 255,   0,  0);
   const video::SColor green(255, 0, 255,  0);
   const video::SColor blue (255, 0,   0, 255);

   // i'm to lazy to do texture mapping
   const f32 tu = 0.f;
   const f32 tv = 0.f;

   // the X axis
   Vertices[ 0] = video::S3DVertex(1.f , 0.f, 0.f, 1.f, 0.f, 0.f, red, tu, tv); // the point
   Vertices[ 1] = video::S3DVertex(.03f, -.03f, -.03f, 0.f, -.707106f, -.707106f, red, tu, tv);
   Vertices[ 2] = video::S3DVertex(.03f, -.03f,  .03f, 0.f, -.707106f,  .707106f, red, tu, tv);
   Vertices[ 3] = video::S3DVertex(.03f,  .03f,  .03f, 0.f,  .707106f,  .707106f, red, tu, tv);
   Vertices[ 4] = video::S3DVertex(.03f,  .03f, -.03f, 0.f,  .707106f, -.707106f, red, tu, tv);

   // the Y axis
   Vertices[ 5] = video::S3DVertex(0.f, 1.f, 0.f, 0.f, 1.f, 0.f, green, tu, tv); // the point
   Vertices[ 6] = video::S3DVertex(-.03f, .03f, -.03f, -.707106f, 0.f, -.707106f, green, tu, tv);
   Vertices[ 7] = video::S3DVertex(-.03f, .03f,  .03f, -.707106f, 0.f,  .707106f, green, tu, tv);
   Vertices[ 8] = video::S3DVertex( .03f, .03f,  .03f,  .707106f, 0.f,  .707106f, green, tu, tv);
   Vertices[ 9] = video::S3DVertex( .03f, .03f, -.03f,  .707106f, 0.f, -.707106f, green, tu, tv);

   // the Z axis
   Vertices[10] = video::S3DVertex(0.f, 0.f, 1.f, 0.f, 0.f, 1.f, blue, tu, tv); // the point
   Vertices[11] = video::S3DVertex(-.03f, -.03f, .03f, -.707106f, -.707106f, 0.f, blue, tu, tv);
   Vertices[12] = video::S3DVertex(-.03f,  .03f, .03f, -.707106f,  .707106f, 0.f, blue, tu, tv);
   Vertices[13] = video::S3DVertex( .03f,  .03f, .03f,  .707106f,  .707106f, 0.f, blue, tu, tv);
   Vertices[14] = video::S3DVertex( .03f, -.03f, .03f,  .707106f, -.707106f, 0.f, blue, tu, tv);

   // X axis indices
   Indices[ 0] = 0; Indices[ 1] = 2; Indices[ 2] = 1;
   Indices[ 3] = 0; Indices[ 4] = 3; Indices[ 5] = 2;
   Indices[ 6] = 0; Indices[ 7] = 4; Indices[ 8] = 3;
   Indices[ 9] = 0; Indices[10] = 1; Indices[11] = 4;

   // Y axis indices
   Indices[12] = 5; Indices[13] = 6; Indices[14] = 7;
   Indices[15] = 5; Indices[16] = 7; Indices[17] = 8;
   Indices[18] = 5; Indices[19] = 8; Indices[20] = 9;
   Indices[21] = 5; Indices[22] = 9; Indices[23] = 6;

   // Z axis indices
   Indices[24] = 10; Indices[25] = 12; Indices[26] = 11;
   Indices[27] = 10; Indices[28] = 13; Indices[29] = 12;
   Indices[30] = 10; Indices[31] = 14; Indices[32] = 13;
   Indices[33] = 10; Indices[34] = 11; Indices[35] = 14;

   Box.reset(Vertices[0].Pos);

   u32 u;
   for (u = 1; u < 15; ++u)
      Box.addInternalPoint(Vertices[u].Pos);

   Material.Lighting = false;
   Material.BackfaceCulling = false;
}

void COrientationAxisSceneNode::OnRegisterSceneNode()
{
	if (IsVisible)
		SceneManager->registerNodeForRendering(this);

	ISceneNode::OnRegisterSceneNode();
}

void COrientationAxisSceneNode::render()
{
   video::IVideoDriver* driver = SceneManager->getVideoDriver();
   if (!driver)
      return;

   driver->setTransform(video::ETS_WORLD, AbsoluteTransformation);
   driver->setMaterial(Material);

   driver->drawIndexedTriangleList(Vertices, 15, Indices, 12);
}

}; // namespace scene
}; // namespace irr