#pragma once
#include "../GUILayout.h"

class EnvDescriptionLayout : public GUILayout
{
public:
	EnvDescriptionLayout(const char *name);
	virtual ~EnvDescriptionLayout(void);

	virtual bool Load();
	virtual void Parse(TiXmlElement *);
	void CommitToConfig();
protected:
	void OKButtonPressed(MyGUI::WidgetPtr _widget);
	void CancelButtonPressed(MyGUI::WidgetPtr _widget);
			
	TiXmlElement *Description;
	
	MyGUI::SheetPtr LightSheet, MeshSheet, SkyBoxSheet;
	
	MyGUI::EditPtr GeneralName, GeneralPosX, GeneralPosY, GeneralPosZ, GeneralRotX, GeneralRotY, GeneralRotZ;
	MyGUI::ComboBoxPtr GeneralTypeCombo;
	MyGUI::ListPtr GeneralUserDataList;
};
