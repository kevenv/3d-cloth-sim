#include "ClothSceneNode.h"
#include "Cloth.h"
#include <vector>

ClothSceneNode::ClothSceneNode(Cloth* cloth, scene::ISceneNode* parent, scene::ISceneManager* mgr, s32 id):
	scene::ISceneNode(parent, mgr, id),
	m_Cloth(cloth)
{
	m_material.Wireframe = true;
	m_material.Lighting = false;
	m_material.BackfaceCulling = false;

	m_Mesh = new scene::SMesh();

	//create
	scene::SMeshBuffer* buf = new scene::SMeshBuffer();
	m_Mesh->addMeshBuffer(buf);

	int width = m_Cloth->getWidth();
	int height = m_Cloth->getHeight();
	m_nVertices = m_Cloth->getParticles().size();

	buf->Vertices.set_used(m_nVertices);
	updateVertices(buf);
	
	//2 triangles per quad, 3 pts per triangle
	int nTriangles = 2*((width-1)*(height-1));
	m_nIndices = 3*nTriangles;
	buf->Indices.set_used(m_nIndices);
	int i = 0;
	for (int x = 0; x < width-1; ++x) {
		for (int y = 0; y < height-1; ++y) {
			buf->Indices[i+0] = idx2dTo1d(x,y);
			buf->Indices[i+1] = idx2dTo1d(x+1,y);
			buf->Indices[i+2] = idx2dTo1d(x,y+1);

			buf->Indices[i+3] = idx2dTo1d(x,y+1);
			buf->Indices[i+4] = idx2dTo1d(x+1,y);
			buf->Indices[i+5] = idx2dTo1d(x+1,y+1);
			i += 6;
		}
	}
	
	setDirty();
}

ClothSceneNode::~ClothSceneNode()
{
	scene::IMeshBuffer* buf = m_Mesh->getMeshBuffer(0);
	buf->drop();
	m_Mesh->drop();
}

void ClothSceneNode::updateVertices(scene::SMeshBuffer* buffer)
{
	std::vector<Particle>& particles = m_Cloth->getParticles();
	for (int i = 0; i < particles.size(); ++i) {
		Particle& p = particles[i];

		video::S3DVertex& v = buffer->Vertices[i];
		v.Pos.set(p.p.X, p.p.Y, p.p.Z);
		v.Normal.set(0.0, 0.0, 0.0); //TODO:
		v.Color = video::SColor(255, 0, 0, 255);
		//v.TCoords.set();
	}
}

void ClothSceneNode::update()
{
	scene::SMeshBuffer* buf = static_cast<scene::SMeshBuffer*>(m_Mesh->getMeshBuffer(0));
	updateVertices(buf);
	setDirty();
}

void ClothSceneNode::OnRegisterSceneNode()
{
	if (IsVisible) {
		SceneManager->registerNodeForRendering(this);
	}

	ISceneNode::OnRegisterSceneNode();
}

void ClothSceneNode::render()
{
	video::IVideoDriver* driver = SceneManager->getVideoDriver();

	scene::IMeshBuffer* buf = m_Mesh->getMeshBuffer(0);
	u16* indices = buf->getIndices();

	driver->setMaterial(m_material);
	driver->setTransform(video::ETS_WORLD, AbsoluteTransformation);
	driver->drawVertexPrimitiveList(buf->getVertices(), m_nVertices, &indices[0], m_nIndices/3, video::EVT_STANDARD, scene::EPT_TRIANGLES, video::EIT_16BIT);
}

void ClothSceneNode::setDirty()
{
	// set dirty flag to make sure that hardware copies of this
	// buffer are also updated, see IMesh::setHardwareMappingHint
	m_Mesh->setDirty();
	m_Mesh->recalculateBoundingBox();
}

int ClothSceneNode::idx2dTo1d(int x, int y)
{
	return x + y*m_Cloth->getWidth();
}