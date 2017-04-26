#include "ClothSceneNode.h"
#include "Cloth.h"
#include <vector>

ClothSceneNode::ClothSceneNode(Cloth* cloth, scene::ISceneNode* parent, scene::ISceneManager* mgr, s32 id):
	scene::ISceneNode(parent, mgr, id),
	m_Cloth(cloth)
{
	m_material.Wireframe = false;
	m_material.Lighting = true;
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
			buf->Indices[i+0] = m_Cloth->idx2Dto1D(x,y);
			buf->Indices[i+1] = m_Cloth->idx2Dto1D(x+1,y);
			buf->Indices[i+2] = m_Cloth->idx2Dto1D(x,y+1);

			buf->Indices[i+3] = m_Cloth->idx2Dto1D(x,y+1);
			buf->Indices[i+4] = m_Cloth->idx2Dto1D(x+1,y);
			buf->Indices[i+5] = m_Cloth->idx2Dto1D(x+1,y+1);

			i += 6;
		}
	}

	updateNormals(buf);
	
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
		if (p.pinned) {
			v.Color = video::SColor(255, 255, 0, 0);
		}
		else {
			v.Color = video::SColor(255, 0, 0, 255);
		}
	}
}

void ClothSceneNode::updateNormals(scene::SMeshBuffer* buffer)
{
	const u32 vtxcnt = buffer->getVertexCount();
	for (int i = 0; i != vtxcnt; ++i) {
		buffer->getNormal(i).set(0.f, 0.f, 0.f);
	}

	int nIndices = buffer->getIndexCount();
	for (int i = 0; i < nIndices; i += 3) {
		video::S3DVertex& a = buffer->Vertices[buffer->Indices[i + 0]];
		video::S3DVertex& b = buffer->Vertices[buffer->Indices[i + 1]];
		video::S3DVertex& c = buffer->Vertices[buffer->Indices[i + 2]];
		
		//calc normal
		core::vector3df U(b.Pos - a.Pos);
		core::vector3df V(c.Pos - a.Pos);
		core::vector3df n(U.crossProduct(V));

		a.Normal += n;
		b.Normal += n;
		c.Normal += n;
	}
	
	for (int i = 0; i < vtxcnt; ++i) {
		buffer->getNormal(i).normalize();
	}
}

void ClothSceneNode::update()
{
	scene::SMeshBuffer* buf = static_cast<scene::SMeshBuffer*>(m_Mesh->getMeshBuffer(0));
	updateVertices(buf);
	updateNormals(buf);
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