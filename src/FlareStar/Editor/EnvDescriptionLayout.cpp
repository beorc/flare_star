#include "..\StdAfx.h"
#include "EnvDescriptionLayout.h"
#include "..\GUISystem.h"
#include "..\CommonDeclarations.h"
#include "..\AApplication.h"
#include "EditorFrameHandler.h"
#include "EditorFrameListener.h"

enum EGeneralType 
{
	Empty,
	Mesh,
	Light,
	SkyBox
};

EnvDescriptionLayout::EnvDescriptionLayout(const char *name) : 
GUILayout(name),
Description(NULL)
{
}

EnvDescriptionLayout::~EnvDescriptionLayout(void)
{
}

bool EnvDescriptionLayout::Load()
{
	bool res = GUILayout::Load();
	
	MyGUI::ButtonPtr button;

	MyGUI::Gui *gui = GUISystem::GetInstance()->GetGui();
	
	button = gui->findWidget<MyGUI::Button>("EditorObjectOptionsOK");
	button->eventMouseButtonClick = MyGUI::newDelegate(this, &EnvDescriptionLayout::OKButtonPressed);
	
	button = gui->findWidget<MyGUI::Button>("EditorObjectOptionsCancel");
	button->eventMouseButtonClick = MyGUI::newDelegate(this, &EnvDescriptionLayout::CancelButtonPressed);
	
	LightSheet = gui->findWidget<MyGUI::Sheet>("EditorObjectOptionsLightSheet");
	MeshSheet = gui->findWidget<MyGUI::Sheet>("EditorObjectOptionsMeshSheet");
	SkyBoxSheet = gui->findWidget<MyGUI::Sheet>("EditorObjectOptionsSkyBoxSheet");
	
	GeneralName = gui->findWidget<MyGUI::Edit>("EditorObjectOptionsGeneralNameEdit");
	
	GeneralPosX = gui->findWidget<MyGUI::Edit>("EditorObjectOptionsGeneralPosX");
	GeneralPosY = gui->findWidget<MyGUI::Edit>("EditorObjectOptionsGeneralPosY");
	GeneralPosZ = gui->findWidget<MyGUI::Edit>("EditorObjectOptionsGeneralPosZ");
	
	GeneralRotX = gui->findWidget<MyGUI::Edit>("EditorObjectOptionsGeneralOrientX");
	GeneralRotY = gui->findWidget<MyGUI::Edit>("EditorObjectOptionsGeneralOrientY");
	GeneralRotZ = gui->findWidget<MyGUI::Edit>("EditorObjectOptionsGeneralOrientZ");
	
	GeneralTypeCombo = gui->findWidget<MyGUI::ComboBox>("EditorObjectOptionsGeneralTypeCombo");
	
	GeneralUserDataList = gui->findWidget<MyGUI::List>("EditorObjectOptionsUserDataList");
				
	return res;
}

void EnvDescriptionLayout::Parse(TiXmlElement *xml)
{
	Description = xml;
}

void EnvDescriptionLayout::CommitToConfig()
{
}

void EnvDescriptionLayout::OKButtonPressed(MyGUI::WidgetPtr _widget) 
{			
	if (Description) //edit
	{
	} else // new
	{
		TiXmlElement *xml = new TiXmlElement("node");
		EditorFrameHandler *handler = EditorFrameListener::GetInstance()->GetCurrentHandler();
		handler->AddObject(xml);
	}
}

void EnvDescriptionLayout::CancelButtonPressed(MyGUI::WidgetPtr _widget) 
{	
	
}

