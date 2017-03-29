/*
   by vitek
   http://irrlicht.sourceforge.net/forum//viewtopic.php?t=16768

   modified by Keven Villeneuve
*/
#ifndef _C_ORIENTATION_AXIS_SCENE_NODE_H_INCLUDED__
#define _C_ORIENTATION_AXIS_SCENE_NODE_H_INCLUDED__

#include "ISceneNode.h"
#include "S3DVertex.h"

namespace irr
{
namespace scene
{

//! Displays X, Y and Z axes as Red, Green and Blue pyramids for debugging
class COrientationAxisSceneNode : public ISceneNode
{
public:
   //! constructor
   COrientationAxisSceneNode(ISceneNode* parent, ISceneManager* mgr, s32 id=-1,
      const core::vector3df& position = core::vector3df(0, 0, 0),
      const core::vector3df& rotation = core::vector3df(0, 0, 0),
      const core::vector3df& scale    = core::vector3df(1, 1, 1));

   //! destructor
   virtual ~COrientationAxisSceneNode()
   {
   }

   //! pre render event
   virtual void OnRegisterSceneNode();

   //! render
   virtual void render();

   //! returns the axis aligned bounding box of this node
   virtual const core::aabbox3d<f32>& getBoundingBox() const
   {
      return Box;
   }

   //! returns the material
   virtual video::SMaterial& getMaterial(s32   i)
   {
      return Material;
   }
   
   //! returns amount of materials used by this scene node.
   virtual s32 getMaterialCount()
   {
      return 1;
   }
   
private:
   core::aabbox3d<f32> Box;
   video::SMaterial Material;
   video::S3DVertex Vertices[15];
   u16 Indices[36];
};

}; // namespace scene
}; // namespace irr

#endif // _C_ORIENTATION_AXIS_SCENE_NODE_H_INCLUDED__