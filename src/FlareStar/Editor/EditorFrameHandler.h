#pragma once

#include "IFrameHandler.h"

class TiXmlElement;

#include "ScriptFrameHandler.h"
#include "Array.h"
#include "List.h"
#include "HashMap.h"
#include <MyGUI.h>
#include "ObjectsListLayout.h"

class CollisionModel3D;
class IAAObject;
class ObjectDescriptionLayout;


class EditorFrameHandler : public ScriptFrameHandler
{
public:
	struct SEditableObject
	{
		Ogre::UTFString Name;
		CollisionModel3D *CollisionModel;
		Ogre::Node *EditNode;
		Ogre::Entity *EditEntity;
		IAAObject *Object;
	};
	struct SEditableDescription
	{	
		SEditableDescription() :
		EditNode(NULL),
		EditElement(NULL)
		{}
		Ogre::Node *EditNode;
		TiXmlElement *EditElement; 
	};
	typedef AAHashMap<CollisionModel3D*> CollisionModelsContainer;
	typedef List<SEditableObject> EditableObjectsContainer;
    EditorFrameHandler(const char*);
    ~EditorFrameHandler(void);

	void Init();
    void Step(unsigned timeMs);
    bool Parse(TiXmlElement *xml);
	void Close();
	void Exit();

	bool KeyPressed( const OIS::KeyEvent &arg );
	bool KeyReleased( const OIS::KeyEvent &arg );
	bool MousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
	bool MouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
	bool MouseMoved( const OIS::MouseEvent &arg );

	CollisionModel3D *GetCollisionModel(const char *modelname);

	bool SelectObject(Ogre::UTFString name);
	inline Ogre::UTFString GetSelectedObjectName() const {return SelectedObjectName;}
	bool DeleteSelectedObject();
	inline SEditableDescription *GetSelectedObject() {return SelectedObject;}
	void Save();
	void AddObject(TiXmlElement *xml);
	void NotifyPopupMenuAccept(MyGUI::WidgetPtr _sender, MyGUI::PopupMenuPtr _menu, size_t _index);
	
	
protected:    
	void InitGUI();
	void ExitGUI();
	void NotifyQuit();

	bool ExitRequested;
	TiXmlElement *SceneRootElement;
	char *ConfigPath;
	EditableObjectsContainer EditableObjects;
	CollisionModelsContainer CollisionModels;
	Ogre::Camera *EditorCamera;
	int CameraVelocity;
	Ogre::Vector3 CameraMoveDirection;
	Ogre::Radian mRotX, mRotY;

	// GUI

	MyGUI::MenuBarPtr MainMenuBar;
	MyGUI::PopupMenuPtr PopupMenuFile;
	MyGUI::PopupMenuPtr PopupMenuOptions;
	//std::vector<MyGUI::PopupMenu::ItemInfo> widgetMenus;

	std::map<Ogre::UTFString, SEditableDescription> EditorNodes;

	ObjectsListLayout ObjectsList;
	SEditableDescription *SelectedObject;
	Ogre::UTFString SelectedObjectName;
	ObjectDescriptionLayout *ObjectDescription;
};
