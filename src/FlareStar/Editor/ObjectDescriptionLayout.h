#pragma once
#include "../GUILayout.h"

class ObjectDescriptionLayout : public GUILayout
{
public:
	ObjectDescriptionLayout(const char *name);
	virtual ~ObjectDescriptionLayout(void);

	virtual bool Load();
	virtual void Parse( TiXmlElement * );
	void CommitToConfig( TiXmlElement *& );
	
	void Merge(TiXmlElement *xml_dest, TiXmlElement *xml_src);
protected:
	void OKButtonPressed(MyGUI::WidgetPtr _widget);
	void CancelButtonPressed(MyGUI::WidgetPtr _widget);

	void PositionKeyPressed(MyGUI::WidgetPtr _sender, MyGUI::KeyCode _key, MyGUI::Char _char);
	void OrientationSliderChanged(MyGUI::WidgetPtr _sender, size_t _position);
			
	TiXmlElement *Description;
	
	MyGUI::SheetPtr LightSheet, MeshSheet, SkyBoxSheet;
	
	MyGUI::EditPtr GeneralName, GeneralPosX, GeneralPosY, GeneralPosZ, GeneralRotX, GeneralRotY, GeneralRotZ;
	MyGUI::ComboBoxPtr GeneralTypeCombo;
	MyGUI::ListPtr GeneralUserDataList;

	MyGUI::HScrollPtr GeneralRotXSlider, GeneralRotYSlider, GeneralRotZSlider;

	int PrevRotation[3];

	virtual TiXmlElement *GetSkyBox();
	virtual TiXmlElement *GetEntity();
	virtual TiXmlElement *GetLight();
	void	GetNode( Ogre::Node *node, TiXmlElement *&xml );

	void MergeAttributes(TiXmlElement *xml_dest, TiXmlElement *xml_src);
	TiXmlElement *FindChild(TiXmlElement *xml, const char *name) const;
};
