#include "..\StdAfx.h"
#include "ObjectDescriptionLayout.h"
#include "..\GUISystem.h"
#include "..\CommonDeclarations.h"
#include "..\AApplication.h"
#include "EditorFrameHandler.h"
#include "EditorFrameListener.h"

enum EGeneralType 
{
	GTEmpty,
	GTMesh,
	GTLight,
	GTSkyBox
};

ObjectDescriptionLayout::ObjectDescriptionLayout(const char *name) : 
GUILayout(name),
Description(NULL)
{
}

ObjectDescriptionLayout::~ObjectDescriptionLayout(void)
{
	Destroy();
}

bool ObjectDescriptionLayout::Load()
{
	bool res = GUILayout::Load();
	
	MyGUI::ButtonPtr button;

	MyGUI::Gui *gui = GUISystem::GetInstance()->GetGui();
	
	button = gui->findWidget<MyGUI::Button>("EditorObjectOptionsOK");
	button->eventMouseButtonClick = MyGUI::newDelegate(this, &ObjectDescriptionLayout::OKButtonPressed);
	
	button = gui->findWidget<MyGUI::Button>("EditorObjectOptionsCancel");
	button->eventMouseButtonClick = MyGUI::newDelegate(this, &ObjectDescriptionLayout::CancelButtonPressed);
	
	LightSheet = gui->findWidget<MyGUI::Sheet>("EditorObjectOptionsLightSheet");
	MeshSheet = gui->findWidget<MyGUI::Sheet>("EditorObjectOptionsMeshSheet");
	SkyBoxSheet = gui->findWidget<MyGUI::Sheet>("EditorObjectOptionsSkyBoxSheet");
	
	GeneralName = gui->findWidget<MyGUI::Edit>("EditorObjectOptionsGeneralNameEdit");
	
	GeneralPosX = gui->findWidget<MyGUI::Edit>("EditorObjectOptionsGeneralPosX");
	GeneralPosX->eventKeyButtonPressed =MyGUI::newDelegate(this, &ObjectDescriptionLayout::PositionKeyPressed);
	GeneralPosY = gui->findWidget<MyGUI::Edit>("EditorObjectOptionsGeneralPosY");
	GeneralPosY->eventKeyButtonPressed =MyGUI::newDelegate(this, &ObjectDescriptionLayout::PositionKeyPressed);
	GeneralPosZ = gui->findWidget<MyGUI::Edit>("EditorObjectOptionsGeneralPosZ");
	GeneralPosZ->eventKeyButtonPressed =MyGUI::newDelegate(this, &ObjectDescriptionLayout::PositionKeyPressed);
	
	GeneralRotX = gui->findWidget<MyGUI::Edit>("EditorObjectOptionsGeneralOrientX");
	GeneralRotY = gui->findWidget<MyGUI::Edit>("EditorObjectOptionsGeneralOrientY");
	GeneralRotZ = gui->findWidget<MyGUI::Edit>("EditorObjectOptionsGeneralOrientZ");
	
	GeneralTypeCombo = gui->findWidget<MyGUI::ComboBox>("EditorObjectOptionsGeneralTypeCombo");
	
	GeneralUserDataList = gui->findWidget<MyGUI::List>("EditorObjectOptionsUserDataList");

	GeneralRotXSlider = gui->findWidget<MyGUI::HScroll>("EditorRotXSlider");
	GeneralRotXSlider->eventScrollChangePosition = MyGUI::newDelegate(this, &ObjectDescriptionLayout::OrientationSliderChanged);
	GeneralRotYSlider = gui->findWidget<MyGUI::HScroll>("EditorRotYSlider");
	GeneralRotYSlider->eventScrollChangePosition = MyGUI::newDelegate(this, &ObjectDescriptionLayout::OrientationSliderChanged);
	GeneralRotZSlider = gui->findWidget<MyGUI::HScroll>("EditorRotZSlider");
	GeneralRotZSlider->eventScrollChangePosition = MyGUI::newDelegate(this, &ObjectDescriptionLayout::OrientationSliderChanged);

	return res;
}


void GetChildText(const TiXmlElement* xmlElement, Ogre::UTFString& text)
{
    //Get the first element 
    const TiXmlNode* childNode = xmlElement->FirstChild();
    while (childNode != 0)
    {
        if (childNode->Type() == TiXmlNode::TEXT)
        {
            const TiXmlText* textNode = childNode->ToText();
            if (textNode != 0)
            {
                text = textNode->Value();
                break;
            }
        }
        childNode = xmlElement->NextSibling();
    }    
}

void ObjectDescriptionLayout::Parse(TiXmlElement *xml)
{
	Description = xml;
			
	TiXmlElement *temp;
	const char *str;
	
	//GeneralTypeCombo->setItemSelect(EGeneralType::Empty);
	
	str = Description->Value();
	if (strcmp(str,"skyBox")==0)
	{
		GeneralTypeCombo->setItemSelect(GTSkyBox);
	} else	
	if (strcmp(str,"node")==0)
	{
		if (SkyBoxSheet)
			SkyBoxSheet->removeSheet();
		
		TiXmlElement *temp;
		temp = Description->FirstChildElement("light");	
		if (temp)
		{
			GeneralTypeCombo->setItemSelect(GTLight);
		} else
		{
			if (LightSheet)
				LightSheet->removeSheet();		
		
			temp = Description->FirstChildElement("entity");	
			if (temp)
			{
				GeneralTypeCombo->setItemSelect(GTMesh);
			} else
			{			
				if (MeshSheet)
					MeshSheet->removeSheet();
			}
		}
	}
	
	// position
	temp = xml->FirstChildElement("position");
	if (temp)
	{		
		GeneralPosX->setCaption(temp->Attribute("x"));
		GeneralPosY->setCaption(temp->Attribute("y"));
		GeneralPosZ->setCaption(temp->Attribute("z"));
	}
	
	temp = xml->FirstChildElement("rotation");
	if (temp)
	{
		int res;
		float qx, qy, qz, qw;
		res = temp->QueryFloatAttribute("qx", &qx);
		assert(TIXML_SUCCESS==res);
		res = temp->QueryFloatAttribute("qy", &qy);
		assert(TIXML_SUCCESS==res);
		res = temp->QueryFloatAttribute("qz", &qz);
		assert(TIXML_SUCCESS==res);
		res = temp->QueryFloatAttribute("qw", &qw);
		assert(TIXML_SUCCESS==res);

		Ogre::Quaternion q(qx, qy, qz, qw);
		
		char content[100];
		sprintf_s(content, sizeof(content), "%d", (size_t)q.getPitch().valueDegrees() );		
		GeneralRotX->setCaption(content);
		sprintf_s(content, sizeof(content), "%d", (size_t)q.getYaw().valueDegrees() );
		GeneralRotY->setCaption(content);
		sprintf_s(content, sizeof(content), "%d", (size_t)q.getRoll().valueDegrees() );
		GeneralRotZ->setCaption(content);

		GeneralRotXSlider->setScrollPosition((size_t)q.getPitch().valueDegrees());
		GeneralRotYSlider->setScrollPosition((size_t)q.getYaw().valueDegrees());
		GeneralRotZSlider->setScrollPosition((size_t)q.getRoll().valueDegrees());

		PrevRotation[0] = GeneralRotXSlider->getScrollPosition();
		PrevRotation[1] = GeneralRotYSlider->getScrollPosition();
		PrevRotation[2] = GeneralRotZSlider->getScrollPosition();
	}
	
	temp = xml->FirstChildElement("userData");
	if (temp)
	{
		Ogre::UTFString userdata;
		GetChildText(temp, userdata);
		GeneralUserDataList->addItem(userdata);
	}	
}

TiXmlElement *ObjectDescriptionLayout::GetSkyBox()
{
	TiXmlElement *xml = new TiXmlElement("skyBox");
	return xml;
}

TiXmlElement *ObjectDescriptionLayout::GetEntity()
{
	TiXmlElement *xml = new TiXmlElement("entity");
	return xml;
}

TiXmlElement *ObjectDescriptionLayout::GetLight()
{
	TiXmlElement *xml = new TiXmlElement("light");
	return xml;
}

void ObjectDescriptionLayout::GetNode( Ogre::Node *node, TiXmlElement *&xml )
{
	TiXmlElement *temp = new TiXmlElement("position");
	Ogre::Vector3 pos = node->getWorldPosition();
	
	temp->SetAttribute("x", pos.x);
	temp->SetAttribute("y", pos.y);
	temp->SetAttribute("z", pos.z);
	
	xml->LinkEndChild(temp);

	Ogre::Quaternion q = node->getOrientation();
	
	temp = new TiXmlElement("rotation");
	temp->SetAttribute("qx", q.x);
	temp->SetAttribute("qy", q.y);
	temp->SetAttribute("qz", q.z);
	temp->SetAttribute("qw", q.w);
	xml->LinkEndChild(temp);

	size_t count = GeneralUserDataList->getItemCount();

	Ogre::UTFString caption;
    std::string text;
	text.clear();
	for (size_t i=0; i<count; ++i)
	{
		caption = GeneralUserDataList->getItem(i);
		text += AAUtilities::WStringToString(caption.asWStr());
	}
	
	if (!text.empty())
	{
		temp = new TiXmlElement("userData");
		
		TiXmlText *cdata = new TiXmlText(text.c_str());
		cdata->SetCDATA(true);
		//cdata->SetValue(text);

		temp->LinkEndChild(cdata);
		xml->LinkEndChild(temp);
	}	
}

void ObjectDescriptionLayout::CommitToConfig( TiXmlElement *&xml )
{
	TiXmlElement *temp;
	
	EditorFrameHandler::SEditableDescription *selected = EditorFrameListener::GetInstance()->GetCurrentHandler()->GetSelectedObject();

	size_t type = GeneralTypeCombo->getItemSelect();

	switch (type)
	{
		case GTSkyBox:
		{
			xml = new TiXmlElement("skyBox");
			temp = GetSkyBox();
			xml->LinkEndChild(temp);
			break;
		}
		case GTLight:
		{
			xml = new TiXmlElement("node");
			if (selected->EditNode)
				GetNode(selected->EditNode, xml);
			temp = GetLight();
			xml->LinkEndChild(temp);
			break;
		}
		case GTMesh:
		{
			xml = new TiXmlElement("node");
			if (selected->EditNode)
				GetNode(selected->EditNode, xml);
			temp = GetEntity();			
			xml->LinkEndChild(temp);
			break;
		}
		default: assert(false);
	};	
}

void ObjectDescriptionLayout::MergeAttributes(TiXmlElement *xml_dest, TiXmlElement *xml_src)
{
	for ( TiXmlAttribute *attr = xml_src->FirstAttribute(); attr != NULL; attr = attr->Next() )
	{
		xml_dest->SetAttribute( attr->Name(), attr->Value() );		
	}
}

TiXmlElement *ObjectDescriptionLayout::FindChild(TiXmlElement *xml, const char *name) const
{
	TiXmlElement* childNode = xml->FirstChildElement();
    while (childNode != 0)
    {		
		if ( strcmp(childNode->Value(), name)==0 )
		{
			return childNode;
		}
        childNode = childNode->NextSiblingElement();
    }
	return NULL;
}

void ObjectDescriptionLayout::Merge(TiXmlElement *xml_dest, TiXmlElement *xml_src)
{	
	// clear destination without destroying root node
	/*xml_dest->Clear();

	for ( TiXmlAttribute *attr = xml_dest->FirstAttribute(); attr != NULL; )
	{
		TiXmlAttribute *next = attr->Next();
		xml_dest->RemoveAttribute( attr->Name() );
		attr = next;
	}*/

	// copy src to dest

	MergeAttributes(xml_dest, xml_src);

	TiXmlElement* childNode = xml_src->FirstChildElement(), *dest_child = NULL;
    while (childNode != 0)
    {
		dest_child = FindChild(xml_dest, childNode->Value());
		if (dest_child)
		{			
			Merge(dest_child, childNode);			
		}
		else
			xml_dest->LinkEndChild( childNode->Clone() );
        childNode = childNode->NextSiblingElement();
    }
}

void ObjectDescriptionLayout::OKButtonPressed(MyGUI::WidgetPtr _widget) 
{			
	TiXmlElement *xml = NULL;
	CommitToConfig(xml);

	if (Description) //edit
	{
		Merge(Description, xml);
	} else // new
	{		
		EditorFrameHandler *handler = EditorFrameListener::GetInstance()->GetCurrentHandler();
		handler->AddObject(xml);
	}
}

void ObjectDescriptionLayout::CancelButtonPressed(MyGUI::WidgetPtr _widget) 
{	
	
}

void ObjectDescriptionLayout::PositionKeyPressed(MyGUI::WidgetPtr _sender, MyGUI::KeyCode _key, MyGUI::Char _char)
{
	if (MyGUI::KC_RETURN == _key)
	{
		/*if ( strcmp(_sender->getName().c_str(), "EditorObjectOptionsGeneralPosX") == 0 )
		{
		} else
			if ( strcmp(_sender->getName().c_str(), "EditorObjectOptionsGeneralPosY") == 0 )
			{
			} else
				if ( strcmp(_sender->getName().c_str(), "EditorObjectOptionsGeneralPosZ") == 0 )
				{
				} */
		
		EditorFrameHandler::SEditableDescription *selected = EditorFrameListener::GetInstance()->GetCurrentHandler()->GetSelectedObject();
		if (!selected->EditNode)
		{		
			selected->EditNode = CommonDeclarations::GetSceneManager()->createSceneNode();
		}

		//if (selected->EditNode)
		{
			Ogre::UTFString caption;
			std::string text;
									
			double x=0, y=0, z=0;

			caption = GeneralPosX->getCaption();
			text = AAUtilities::WStringToString(caption.asWStr());
			x = atof( text.c_str() );

			caption = GeneralPosY->getCaption();
			text = AAUtilities::WStringToString(caption.asWStr());
			y = atof( text.c_str() );

			caption = GeneralPosZ->getCaption();
			text = AAUtilities::WStringToString(caption.asWStr());
			z = atof( text.c_str() );

			selected->EditNode->setPosition( Ogre::Vector3(x, y, z) );

		}
	}
}

void ObjectDescriptionLayout::OrientationSliderChanged(MyGUI::WidgetPtr _sender, size_t _position)
{
	char buff[100];
	EditorFrameHandler::SEditableDescription *selected = EditorFrameListener::GetInstance()->GetCurrentHandler()->GetSelectedObject();

	if (!selected->EditNode)
	{		
		selected->EditNode = CommonDeclarations::GetSceneManager()->createSceneNode();
	}

	//if (selected->EditNode)
	{
		if ( strcmp(_sender->getName().c_str(), "EditorRotXSlider") == 0 )
		{
			int angle = _position - PrevRotation[0];
			selected->EditNode->rotate( Ogre::Vector3::UNIT_X, Ogre::Degree(angle), Ogre::Node::TS_LOCAL );
			PrevRotation[0] = _position;
			
			GeneralRotX->setCaption( itoa(_position, buff, 10) );
		} else
			if ( strcmp(_sender->getName().c_str(), "EditorRotYSlider") == 0 )
			{
				int angle = _position - PrevRotation[1];
				selected->EditNode->rotate( Ogre::Vector3::UNIT_Y, Ogre::Degree(angle), Ogre::Node::TS_LOCAL );
				PrevRotation[1] = _position;

				GeneralRotY->setCaption( itoa(_position, buff, 10) );
			} else
				if ( strcmp(_sender->getName().c_str(), "EditorRotZSlider") == 0 )
				{
					int angle = _position - PrevRotation[2];
					selected->EditNode->rotate( Ogre::Vector3::UNIT_Z, Ogre::Degree(angle), Ogre::Node::TS_LOCAL );
					PrevRotation[2] = _position;

					GeneralRotZ->setCaption( itoa(_position, buff, 10) );
				}
	}
}
