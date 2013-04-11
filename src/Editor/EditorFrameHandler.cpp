#include "..\StdAfx.h"

#ifdef MEMORY_TEST
#include "mmgr.h"
#endif

#include "EditorFrameHandler.h"
#include "EditorFrameListener.h"
#include "ScriptManager.h"
#include "CommonDeclarations.h"
#include "ComixImage.h"
#include "AApplication.h"
#include "List.h"
#include "GUISystem.h"
#include "GUILayout.h"
#include "IAAObject.h"
#include "Utilities.h"
#include "AACamera.h"
#include "Collidable.h"
#include <coldet.h>
#include "ObjectDescriptionLayout.h"

enum POPUP_MENU_MAIN
{
	ITEM_LOAD,
	ITEM_SAVE,
	ITEM_SAVE_AS,
	ITEM_CLEAR,
	ITEM_SETTINGS,	
	ITEM_QUIT
};

EditorFrameHandler::EditorFrameHandler(const char *scene_config_path) : 
ScriptFrameHandler(),
ExitRequested(false),
SceneRootElement(NULL),
CollisionModels(1000),
EditorCamera(NULL),
CameraMoveDirection(Ogre::Vector3::ZERO),
CameraVelocity(30),
SelectedObject(NULL),
ObjectDescription(NULL)
{
	ConfigPath = AAUtilities::StringCopy(scene_config_path);

	TiXmlDocument *xml = new TiXmlDocument(ConfigPath);	
	    
	bool loadOkay = xml->LoadFile();
	assert(loadOkay);
    
	if (!loadOkay)
	{
		Ogre::StringUtil::StrStreamType errorMessage;
		errorMessage << "There was an error with the xml file: " << scene_config_path;

		OGRE_EXCEPT
			(
			Ogre::Exception::ERR_INVALID_STATE,
			errorMessage.str(), 
			"XMLConfigurable::Parse"
			);
		return;
	}

	SceneRootElement = xml->FirstChildElement()->Clone()->ToElement();
	delete xml;
	assert( SceneRootElement );
}

EditorFrameHandler::~EditorFrameHandler(void)
{
	Close();
	
	delete [] ConfigPath;	
}

void EditorFrameHandler::Step(unsigned timeMs)
{    
	//ScriptFrameHandler::Step(timeMs);

	if (EditorCamera)
		EditorCamera->setPosition(EditorCamera->getPosition()+EditorCamera->getOrientation()*CameraMoveDirection*CameraVelocity);
	    
    if (ExitRequested)
    {
        if (ScriptHandlers[1])
            ScriptManager::GetInstance()->Call(ScriptHandlers[1], "i", false, 0);
        
		CommonDeclarations::GetApplication()->SetCurrentGameState(AApplication::GS_GAME);

		Close();
    }
}

bool EditorFrameHandler::Parse(TiXmlElement *xml)
{
    assert(xml);
    if (xml == 0)
        return false; // file could not be opened

	ScriptFrameHandler::Parse(xml);
            
    return false;
}

void EditorFrameHandler::Exit()
{
	CommonDeclarations::GetApplication()->SetCurrentCamera(CommonDeclarations::GetCamera()->GetOgreCamera());
	Ogre::SceneManager *SceneMgr = CommonDeclarations::GetSceneManager();
	SceneMgr->destroyCamera(EditorCamera);
	EditorCamera = NULL;

	ExitGUI();

	MyGUI::Gui *gui = GUISystem::GetInstance()->GetGui();
	gui->hidePointer();
}

void EditorFrameHandler::Close()
{
	Exit();

	Ogre::SceneManager *scene_manager = CommonDeclarations::GetSceneManager();
	for (EditableObjectsContainer::ListNode *pos = EditableObjects.GetBegin();pos!=NULL;pos=pos->Next)
	{		
		if (pos->Value.EditNode)
		{
			scene_manager->destroySceneNode(pos->Value.EditNode->getName());
			pos->Value.EditNode = NULL;
		}

		if (pos->Value.EditEntity)
		{			
			scene_manager->destroyEntity(pos->Value.EditEntity);
			pos->Value.EditEntity = NULL;
		}
	}

	delete SceneRootElement;
	delete ObjectDescription;

	ScriptFrameHandler::Close();
}

CollisionModel3D *EditorFrameHandler::GetCollisionModel(const char *modelname)
{	
	CollisionModel3D *hashres=NULL, *CollisionModel = NULL;
	bool bres = CollisionModels.Find(modelname, &hashres);

    if (!bres)
    {
        CollisionModel = newCollisionModel3D(false);

        size_t vertex_count,index_count;
        Ogre::Vector3* vertices;
        unsigned long* indices;

        Ogre::SceneManager *SceneMgr = CommonDeclarations::GetSceneManager();

        Ogre::Entity *entity = SceneMgr->createEntity("tmpcollis", modelname);

		Collidable::getMeshInformation(entity->getMesh().getPointer(),vertex_count,vertices,index_count,indices,Ogre::Vector3(0,0,0),Ogre::Quaternion::IDENTITY,Ogre::Vector3(1,1,1));

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

		CollisionModels.Insert(modelname,CollisionModel);

    } else
    {
        CollisionModel = hashres;
    }
	return CollisionModel;
}

void EditorFrameHandler::InitGUI()
{
	MyGUI::Gui *gui = GUISystem::GetInstance()->GetGui();

	// создание меню
	MainMenuBar = gui->createWidget<MyGUI::MenuBar>("MenuBar", MyGUI::IntCoord(0, 0, gui->getViewWidth(), 28), MyGUI::Align::Top | MyGUI::Align::HStretch, "Overlapped", "LayoutEditor_MenuBar");
	MainMenuBar->addItem("File");
	MainMenuBar->addItem("Options");
	// FIXME менюбар сунуть в лейаут
	//interfaceWidgets.push_back(bar);

	PopupMenuFile = MainMenuBar->getItemMenu(0);
	PopupMenuFile->addItem("Load");
	PopupMenuFile->addItem("Save");
	PopupMenuFile->addItem("Save_as");
	PopupMenuFile->addItem("Clear", false, true);
	PopupMenuFile->addItem("Settings");	
	PopupMenuFile->addItem("Quit");
	
	PopupMenuOptions = MainMenuBar->getItemMenu(1);	
	PopupMenuOptions->addItem("Environment");

	//for (List<Ogre::UTFString>::ListNode *pos = XMLEditorNodes.GetBegin();pos!=NULL;pos=pos->Next)
	ObjectsList.Load();

	std::map<Ogre::UTFString, SEditableDescription>::iterator iPos = EditorNodes.begin(), iEnd = EditorNodes.end();
	for (;iPos!=iEnd;++iPos)
	{
		//PopupMenuWidgets->addItem(iPos->first);
		ObjectsList.AddItem(iPos->first);
	}
	
	MainMenuBar->eventPopupMenuAccept = MyGUI::newDelegate(this, &EditorFrameHandler::NotifyPopupMenuAccept);
}

void EditorFrameHandler::ExitGUI()
{
	MyGUI::Gui *gui = GUISystem::GetInstance()->GetGui();
	MainMenuBar->deleteAllItems();
	gui->destroyWidget(MainMenuBar);	
	ObjectsList.Close();
}

void EditorFrameHandler::Init()
{
	Ogre::SceneManager *scene_manager = CommonDeclarations::GetSceneManager();
	EditorCamera = scene_manager->createCamera("EditorCamera");
	EditorCamera->setNearClipDistance(5);		
	EditorCamera->setFarClipDistance(0);

	EditorCamera->setPosition(CommonDeclarations::GetCamera()->GetOgreCamera()->getWorldPosition());

	CommonDeclarations::GetApplication()->SetCurrentCamera(EditorCamera);

	/*CommonDeclarations::ObjectsPool *objects = CommonDeclarations::GetEditableObjects();
	for (CommonDeclarations::ObjectsPool::ListNode *pos = objects->GetBegin();pos!=NULL;pos=pos->Next)
	{		
		SEditableObject obj;
		obj.Object = pos->Value;
		IScenable *scen = pos->Value->GetScenable();
		if (scen)
		{
			obj.EditNode = NULL;
			obj.EditEntity = NULL;
			ICollidable *collid = pos->Value->GetCollidable();
			if (collid)
			{
				obj.CollisionModel = collid->GetCollisionModel();
			} else
			{				
				const char *modelname = scen->GetModelName();
				if (NULL == modelname)
					modelname = AAUtilities::StringCopy("cube.mesh");
				obj.CollisionModel = GetCollisionModel(modelname);
			}				
		} else
		{
			obj.EditNode = scene_manager->getRootSceneNode()->createChildSceneNode();	
			char *buffer = CommonDeclarations::GenGUID();			
			obj.EditEntity = scene_manager->createEntity(buffer, "cube.mesh");					
			delete [] buffer;

			obj.EditNode->attachObject(obj.EditEntity);

			obj.CollisionModel = GetCollisionModel("cube.mesh");
		}
		EditableObjects.PushBack(obj);		
	}
	delete objects;*/

	//
	CommonDeclarations::ObjectsPool *objects = CommonDeclarations::GetEditableObjects();
	const char *str;
	TiXmlElement *node = 0, *nodes, *env;
	env = SceneRootElement->FirstChildElement("environment");	
	if (env)
	{
		node = env->FirstChildElement("skyBox");	
		if (node)
		{			
			SEditableDescription descr;
				
			descr.EditNode = NULL;
			descr.EditElement = node;
			EditorNodes.insert(std::make_pair("SkyBox", descr));
		}
	}
	
	nodes = SceneRootElement->FirstChildElement("nodes");	
	if (nodes)
	{
		node = nodes->FirstChildElement("node");	
		
		Ogre::Node::ChildNodeIterator iFirst = scene_manager->getRootSceneNode()->getChildIterator(), iPos = iFirst;
		
		while (node)
		{
			str = node->Attribute("name");
			SEditableDescription descr;
			Ogre::Node *t = NULL;
			iPos = iFirst;
			while (iPos.hasMoreElements()) 
			{
				t = iPos.getNext();
				if (t->getName()==str)
				{
					descr.EditNode = t;
					break;
				}
			}
			
			descr.EditElement = node;
			EditorNodes.insert(std::make_pair(str, descr));
			
			node = node->NextSiblingElement("node");
		}
	}
	delete objects;
	//

	MyGUI::Gui *gui = GUISystem::GetInstance()->GetGui();
	OIS::Mouse *mouse = EditorFrameListener::GetInstance()->GetMouse();

	InitGUI();

	const OIS::MouseState &ms = mouse->getMouseState();
	int x=ms.width/2,
		y=ms.height/2;
	mouse->setMousePosition(x,y);
    gui->setPointerPosition(x, y);	
	gui->showPointer();
}

bool EditorFrameHandler::KeyPressed( const OIS::KeyEvent &arg )
{
	/*if( arg.key == OIS::KC_ESCAPE )
	{	
		NotifyQuit();		
		return true;		
	}

	if( arg.key == OIS::KC_RETURN )
	{	
		TiXmlDocument doc;
		
		doc.LinkEndChild( SceneRootElement );

		doc.SaveFile(ConfigPath);
		return true;		
	}*/
	if( arg.key == OIS::KC_A )
	{	
		CameraMoveDirection.x -= 1;
	}
	if( arg.key == OIS::KC_D )
	{	
		CameraMoveDirection.x += 1;
	}

	if( arg.key == OIS::KC_W )
	{
		CameraMoveDirection.z -= 1;
	}
	if( arg.key == OIS::KC_S )
	{
		CameraMoveDirection.z += 1;
	}

	if( arg.key == OIS::KC_E )
	{
		CameraMoveDirection.y -= 1;
	}
	if( arg.key == OIS::KC_Q )
	{
		CameraMoveDirection.y += 1;
	}
	

	return false;
}

inline void GetCollisionModelTranformation(IScenable *scn, float m[])
{
	Ogre::Matrix4 transformation_matrix;

    //IScenable *scn = Owner->GetScenable();
    assert(scn);
	transformation_matrix.makeTransform(scn->GetPosition(),scn->GetScale(),scn->GetAbsoluteOrientation());

	int k=0;
	for (int i=0;i<4;++i)
		for (int j=0;j<4;++j)
			m[k++]=transformation_matrix[j][i];
}

bool EditorFrameHandler::MousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{	
	/*Ogre::Matrix4 vm = EditorCamera->getViewMatrix();
	Ogre::Matrix4 pm = EditorCamera->getProjectionMatrix();

	Ogre::Vector3 dir(arg.state.X.rel, arg.state.Y.rel, 1);
	dir = pm.inverse()*dir;

	float origin[3] = {0,0,0}, direction[3] = {dir.x, dir.y, 1};

	Ogre::Vector3 cam_pos = EditorCamera->getWorldPosition();

	int min_dist = INT_MAX;
	IAAObject *selected_object = NULL;
	
	for (EditableObjectsContainer::ListNode *pos = EditableObjects.GetBegin();pos!=NULL;pos=pos->Next)
	{
		IAAObject *obj = pos->Value.Object;
		CollisionModel3D *collision = pos->Value.CollisionModel;
		IScenable *scen = obj->GetScenable();

		if (scen)
		{
			float m[16];
			GetCollisionModelTranformation(scen, m);
			collision->setTransform(m);
			if (collision->rayCollision(origin, direction))
			{
				float point[3];
				collision->getCollisionPoint(point, false);
				Ogre::Vector3 dest(point[0], point[1], point[2]);

				dest = dest - cam_pos;
				int sqlength = dest.squaredLength();
				if (sqlength<min_dist)
				{
					min_dist = sqlength;
					selected_object = pos->Value.Object;
				}
			}
		}
	}*/

	return false;
}

bool EditorFrameHandler::KeyReleased( const OIS::KeyEvent &arg )
{
	if( arg.key == OIS::KC_A )
	{	
		CameraMoveDirection.x += 1;		
	}
	if( arg.key == OIS::KC_D )
	{	
		CameraMoveDirection.x -= 1;		
	}

	if( arg.key == OIS::KC_W )
	{	
		CameraMoveDirection.z += 1;		
	}
	if( arg.key == OIS::KC_S )
	{	
		CameraMoveDirection.z -= 1;
	}

	if( arg.key == OIS::KC_E )
	{	
		CameraMoveDirection.y += 1;		
	}
	if( arg.key == OIS::KC_Q )
	{	
		CameraMoveDirection.y -= 1;
	}

	return false;
}

bool EditorFrameHandler::MouseMoved( const OIS::MouseEvent &arg )
{	
	if( arg.state.buttonDown( OIS::MB_Right ) )
    {				
		mRotX = Ogre::Degree(-arg.state.X.rel * 0.13);				
        mRotY = Ogre::Degree(-arg.state.Y.rel * 0.13);

		//	
		//EditorCamera->pitch(Ogre::Degree(mRotY));
		//EditorCamera->yaw(Ogre::Degree(mRotX));
		//

		Ogre::Quaternion q, orientation;
		q.FromAngleAxis(mRotY, Ogre::Vector3::UNIT_X);
		orientation.FromAngleAxis(mRotX, Ogre::Vector3::UNIT_Y);
		orientation = orientation*q;

		Ogre::Vector3 lookdir(orientation*Ogre::Vector3::NEGATIVE_UNIT_Z);

		Ogre::Vector3 xVec = Ogre::Vector3::UNIT_Y.crossProduct(lookdir);
		xVec.normalise();
		Ogre::Vector3 yVec = lookdir.crossProduct(xVec);
		yVec.normalise();
		Ogre::Quaternion unitZToTarget = Ogre::Quaternion(xVec, yVec, lookdir);

		orientation = Ogre::Quaternion(-unitZToTarget.y, -unitZToTarget.z, unitZToTarget.w, unitZToTarget.x);

		EditorCamera->setOrientation(EditorCamera->getOrientation()*orientation);
    }	
	return true;
}

bool EditorFrameHandler::MouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	return false;
}

bool EditorFrameHandler::SelectObject(Ogre::UTFString name)
{
	MyGUI::Gui *gui = GUISystem::GetInstance()->GetGui();
	if (ObjectDescription)
	{
		//ObjectDescription->Hide();
		//gui->destroyWidget(ObjectDescription.get());
		ObjectDescription->Destroy();		
	}
	if (SelectedObjectName == name || name.empty())
	{
		delete ObjectDescription;
		ObjectDescription = NULL;
		return false;
	}

	std::map<Ogre::UTFString, SEditableDescription>::iterator iRes = EditorNodes.find(name);
	assert(EditorNodes.end()!=iRes);
	if (EditorNodes.end()!=iRes)
	{
		SelectedObjectName = name;
		SelectedObject = &iRes->second;
		ObjectDescription = new ObjectDescriptionLayout("EditorObjectOptions.layout");
		ObjectDescription->Load();
		ObjectDescription->Parse(SelectedObject->EditElement);
		ObjectDescription->Show();
	}
	return true;
}

void EditorFrameHandler::Save()
{
	TiXmlDocument doc;
	
	doc.LinkEndChild( SceneRootElement->Clone() );

	doc.SaveFile(ConfigPath);
}

void EditorFrameHandler::NotifyPopupMenuAccept(MyGUI::WidgetPtr _sender, MyGUI::PopupMenuPtr _menu, size_t _index)
{
	if (PopupMenuFile == _menu) {
		switch(_index) {
			case ITEM_LOAD:
				//notifyLoadSaveAs(false);
				break;
			case ITEM_SAVE:
				Save();				
				break;
			case ITEM_SAVE_AS:
				//notifyLoadSaveAs(true);
				break;
			case ITEM_SETTINGS:
				//notifySettings();
				break;	
			case ITEM_CLEAR:
				//notifyClear();
				break;
			case ITEM_QUIT:
				NotifyQuit();
				break;
		}
	}
}

void EditorFrameHandler::NotifyQuit()
{		
	//Exit();

	EditorFrameListener *listener = EditorFrameListener::GetInstance();
	listener->RequestShutDown();
}

bool EditorFrameHandler::DeleteSelectedObject()
{
	return true;
}

void EditorFrameHandler::AddObject(TiXmlElement *xml)
{
	TiXmlElement *nodes = SceneRootElement->FirstChildElement("nodes");
	if (nodes)
		nodes->LinkEndChild(xml);
}


