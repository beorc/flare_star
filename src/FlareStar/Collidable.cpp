#include "StdAfx.h"
#include "coldet.h"

#ifdef MEMORY_TEST
#include "mmgr.h"
#endif

#include ".\collidable.h"

#include "Debugging.h"
#include "Room.h"

#include "MeshCollisionDetection.h"
#include "SphereCollisionDetection.h"
#include "MeshAndSphereCollisionDetection.h"

//#include "PhysicsEngine.h"
#include "Utilities.h"
#include "IAAObject.h"
//#include "Types.h"

Collidable::Collidable(void) :
//CollisionDetectionModule(NULL),
Owner(NULL),
Collising(false),
CollisionModelname(NULL)
#ifdef SHOW_COLLID_DATA
,DebugNode(NULL),
DebugOgreEntity(NULL)
#endif
{
}

Collidable::~Collidable(void)
{
    /*if (CollisionDetectionModule)
        delete CollisionDetectionModule;*/
	if (CollisionModelname)
		delete [] CollisionModelname;

#ifdef SHOW_COLLID_DATA  
	Ogre::SceneManager *smgr = CommonDeclarations::GetSceneManager();
	if (DebugNode)
	{
		smgr->destroySceneNode(DebugNode->getName());		
	}	
	if (DebugOgreEntity)
	{
		smgr->destroyMovableObject(DebugOgreEntity);		
	}    
#endif
}

void Collidable::getMeshInformation( const Ogre::Mesh* const mesh, size_t &vertex_count,
                                       Ogre::Vector3* &vertices,
                                       size_t &index_count, unsigned long* &indices,
                                       const Ogre::Vector3 &position,
                                       const Ogre::Quaternion &orient,
                                       const Ogre::Vector3 &scale)
{
    bool added_shared = false;
    size_t current_offset = 0;
    size_t shared_offset = 0;
    size_t next_offset = 0;
    size_t index_offset = 0;


    vertex_count = index_count = 0;

    // Calculate how many vertices and indices we're going to need
    for ( unsigned short i = 0; i < mesh->getNumSubMeshes(); ++i)
    {
        Ogre::SubMesh* submesh = mesh->getSubMesh( i );

        // We only need to add the shared vertices once
        if(submesh->useSharedVertices)
        {
            if( !added_shared )
            {
                vertex_count += mesh->sharedVertexData->vertexCount;
                added_shared = true;
            }
        }
        else
        {
            vertex_count += submesh->vertexData->vertexCount;
        }

        // Add the indices
        index_count += submesh->indexData->indexCount;
    }


    // Allocate space for the vertices and indices
    vertices = new Ogre::Vector3[vertex_count];
    indices = new unsigned long[index_count];

    added_shared = false;

    // Run through the submeshes again, adding the data into the arrays
    for ( unsigned short i = 0; i < mesh->getNumSubMeshes(); ++i)
    {
        Ogre::SubMesh* submesh = mesh->getSubMesh(i);

        Ogre::VertexData* vertex_data = submesh->useSharedVertices ? mesh->sharedVertexData : submesh->vertexData;

        if((!submesh->useSharedVertices)||(submesh->useSharedVertices && !added_shared))
        {
            if(submesh->useSharedVertices)
            {
                added_shared = true;
                shared_offset = current_offset;
            }

            const Ogre::VertexElement* posElem =
                vertex_data->vertexDeclaration->findElementBySemantic(Ogre::VES_POSITION);

            Ogre::HardwareVertexBufferSharedPtr vbuf =
                vertex_data->vertexBufferBinding->getBuffer(posElem->getSource());

            unsigned char* vertex =
                static_cast<unsigned char*>(vbuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));

            // There is _no_ baseVertexPointerToElement() which takes an Ogre::Real or a double
            //  as second argument. So make it float, to avoid trouble when Ogre::Real will
            //  be comiled/typedefed as double:
            //      Ogre::Real* pReal;
            float* pReal;

            for( size_t j = 0; j < vertex_data->vertexCount; ++j, vertex += vbuf->getVertexSize())
            {
                posElem->baseVertexPointerToElement(vertex, &pReal);

                Ogre::Vector3 pt(pReal[0], pReal[1], pReal[2]);

                vertices[current_offset + j] = (orient * (pt * scale)) + position;
            }

            vbuf->unlock();
            next_offset += vertex_data->vertexCount;
        }


        Ogre::IndexData* index_data = submesh->indexData;
        size_t numTris = index_data->indexCount / 3;
        Ogre::HardwareIndexBufferSharedPtr ibuf = index_data->indexBuffer;

        bool use32bitindexes = (ibuf->getType() == Ogre::HardwareIndexBuffer::IT_32BIT);

        unsigned long*  pLong = static_cast<unsigned long*>(ibuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
        unsigned short* pShort = reinterpret_cast<unsigned short*>(pLong);


        size_t offset = (submesh->useSharedVertices)? shared_offset : current_offset;

        if ( use32bitindexes )
        {
            for ( size_t k = 0; k < numTris*3; ++k)
            {
                indices[index_offset++] = pLong[k] + static_cast<unsigned long>(offset);
            }
        }
        else
        {
            for ( size_t k = 0; k < numTris*3; ++k)
            {
                indices[index_offset++] = static_cast<unsigned long>(pShort[k]) +
                    static_cast<unsigned long>(offset);
            }
        }

        ibuf->unlock();
        current_offset = next_offset;
    }
}

void    Collidable::InitCollisionModel()
{
	CommonDeclarations::CollisionObjectsPool *pool = CommonDeclarations::GetCollisionObjectsPool();	
    //CommonDeclarations::CollisionObjectsPool::iterator iRes = pool->find(CollisionModelname);
	CollisionModel3D *hashres=NULL;
	bool bres = pool->Find(CollisionModelname, &hashres);

    if (!bres) 
    {
        CollisionModel = newCollisionModel3D(false);

        size_t vertex_count,index_count;
        Ogre::Vector3* vertices;
        unsigned long* indices;

        Ogre::SceneManager *SceneMgr = CommonDeclarations::GetSceneManager();

        Ogre::Entity *entity = SceneMgr->createEntity("tmpcollis", CollisionModelname);

        getMeshInformation(entity->getMesh().getPointer(),vertex_count,vertices,index_count,indices,Ogre::Vector3(0,0,0),Ogre::Quaternion::IDENTITY,Ogre::Vector3(1,1,1));

        SceneMgr->destroyEntity(entity);

        size_t index;
        int numTris = (int)index_count / 3;
        CollisionModel->setTriangleNumber(numTris);
        for (unsigned i=0;i<index_count;i+=3)
        {
            index = indices[i];
            CollisionModel->addTriangle(vertices[indices[i+0]].x,vertices[indices[i+0]].y,vertices[indices[i+0]].z,
                vertices[indices[i+1]].x,vertices[indices[i+1]].y,vertices[indices[i+1]].z,
                vertices[indices[i+2]].x,vertices[indices[i+2]].y,vertices[indices[i+2]].z);
        }
        CollisionModel->finalize();

        delete[] vertices;
        delete[] indices;

		pool->Insert(CollisionModelname,CollisionModel);

    } else
    {
        CollisionModel = hashres;
    }

#ifdef SHOW_COLLID_DATA
	Ogre::SceneManager *smgr = CommonDeclarations::GetSceneManager();
	DebugOgreEntity = smgr->createEntity(CommonDeclarations::GenGUID(), CollisionModelname);

	DebugNode = smgr->getRootSceneNode()->createChildSceneNode();		
	DebugNode->attachObject((Ogre::MovableObject*)DebugOgreEntity);
#endif

}

bool	Collidable::Collide(ICollidable* object)
{
    bool collide=CollisionDetectionModule->Collide(object);

	Collising = collide;

    return collide;
}

bool	Collidable::Collide(const Ogre::Ray &ray)
{
    return CollisionDetectionModule->Collide(ray);    
}

bool	Collidable::Collide(const Ogre::Vector3 &origin, const float &radius)
{
    return CollisionDetectionModule->Collide(origin, radius);
}

void Collidable::AddCollision(ICollidable *obj)
{
	if (obj)
		Collisions.PushBack(obj);
}

void Collidable::ClearCollisions()
{
	Collisions.Clear();
}

bool Collidable::IsInCollisions(ICollidable *obj)
{
	if (obj) 
	{
		//std::vector<ICollidable *>::iterator iPos = Collisions.begin(), iEnd = Collisions.end();

		for (List<ICollidable *>::ListNode *pos = Collisions.GetBegin();pos!=NULL;pos=pos->Next)
		{
			if (pos->Value==obj)
				return true;
		}
		return false;
	}
    return false;
}

void Collidable::UpdateCollisionModelTranformation()
{
	float m[16];
	GetCollisionModelTranformation(m);

	#ifdef SHOW_COLLID_DATA
	if (DebugNode)
	{	
		IScenable *scn = Owner->GetScenable();
		DebugNode->setPosition(scn->GetPosition());
		DebugNode->setOrientation(scn->GetAbsoluteOrientation());
	}
	#endif

	CollisionModel->setTransform(m);
}

inline void Collidable::GetCollisionModelTranformation(float m[])
{
	Ogre::Matrix4 transformation_matrix;

    IScenable *scn = Owner->GetScenable();
    assert(scn);
	transformation_matrix.makeTransform(scn->GetPosition(),scn->GetScale(),scn->GetAbsoluteOrientation());

	int k=0;
	for (int i=0;i<4;++i)
		for (int j=0;j<4;++j)
			m[k++]=transformation_matrix[j][i];
}

CollisionModel3D * Collidable::GetCollisionModel()
{
    return CollisionModel;
}

bool Collidable::Parse(TiXmlElement* xml)
{
    if (xml == 0)
        return false; // file could not be opened

    const char *str;

    str = xml->Value();

    assert(strcmp(str,"obj")==0 || strcmp(str,"node")==0 || strcmp(str,"player")==0 || strcmp(str,"ammo")==0);

    str = xml->Attribute("cfg");

    if (str)
    {
        return XMLConfigurable::Parse(str);
    }

    str = xml->Attribute("collismodel");
    assert(str);    
    if (str) 
    {
		if (CollisionModelname)
			delete [] CollisionModelname;
		CollisionModelname = AAUtilities::StringCopy(str);
    }

    str = xml->Attribute("collisdetection");
    if (str) 
    {
        if (strcmp(str,"mesh_sphere")==0)
            CollisionDetectionModule.reset(new MeshAndSphereCollisionDetection(this));		
        else
            if (strcmp(str,"mesh")==0)
                CollisionDetectionModule.reset(new MeshCollisionDetection(this));		
            else
                if (strcmp(str,"sphere")==0)
                    CollisionDetectionModule.reset(new SphereCollisionDetection(this));
                else
                    assert(false);

    } else	
        CollisionDetectionModule.reset(new MeshCollisionDetection(this));

    return true;
}

CollisionDetection *Collidable::GetCollisionDetection() const
{
    return CollisionDetectionModule.get();
}

void Collidable::SetCollisionDetection(CollisionDetection *module)
{
	CollisionDetectionModule.reset(module);
}

//bool Collidable::IsCollided()
//{
//    return false;
//}

void Collidable::SetOwner(IAAObject *owner)
{
	Owner=owner;
}

const IAAObject *Collidable::GetOwner() const
{
	return Owner;
}

bool Collidable::GetCollising() const
{
	return Collising;
}

void Collidable::SaveTo(TiXmlElement * root) const
{
	root->SetAttribute("collismodel", CollisionModelname);	

	if (CollisionDetectionModule.get())
		CollisionDetectionModule->SaveTo(root);
}

bool Collidable::HaveCollisions() const
{
    return Collisions.IsEmpty();
}

char *Collidable::GetCollisionModelname() const
{
	return CollisionModelname;
}




