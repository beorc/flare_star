#include "PanelObjectsList.h"
#include "GUISystem.h"
#include "EditorFrameListener.h"
#include "EditorFrameHandler.h"
#include "ObjectDescriptionLayout.h"

PanelObjectsList::PanelObjectsList() :
	PanelBase(),
	CurrentHeight(0),
	HeightStep(26), 
	WidthStep(3),	
	Height(24),
	Width(155),
	ObjectDescription(NULL)
{
}

PanelObjectsList::~PanelObjectsList()
{
	delete ObjectDescription;
}

void PanelObjectsList::initialiseCell(PanelCell * _cell)
{
	PanelBase::initialiseCell(_cell);

	mPanelCell->setCaption("Objects");
	
	//for (size_t pos=0; pos<16; ++pos) 
	//{
	//	MyGUI::ButtonPtr checkbox = mWidgetClient->createWidget<MyGUI::Button>("CheckBox", MyGUI::IntCoord(width_step, height_current, width, height), MyGUI::Align::Left | MyGUI::Align::Top | MyGUI::Align::HStretch);
	//	//text->setTextAlign(MyGUI::Align::Right | MyGUI::Align::VCenter);
	//	checkbox->setCaption(MyGUI::utility::toString("line ", pos + 1, " : "));
	//	checkbox->eventMouseButtonClick = MyGUI::newDelegate(this, &PanelObjectsList::mousePressed);
	//	mItemsCheckBox.push_back(checkbox);
	//	
	//	height_current += height_step;
	//}
	
	ButtonAdd = mWidgetClient->createWidget<MyGUI::Button>("Button", MyGUI::IntCoord(WidthStep, CurrentHeight, Width, Height), MyGUI::Align::Left | MyGUI::Align::Bottom);
	ButtonAdd->setCaption("Add");
	ButtonAdd->eventMouseButtonClick = MyGUI::newDelegate(this, &PanelObjectsList::NewObject);
	
	ButtonRemove = mWidgetClient->createWidget<MyGUI::Button>("Button", MyGUI::IntCoord(WidthStep, CurrentHeight, Width, Height), MyGUI::Align::Right | MyGUI::Align::Bottom);
	ButtonRemove->setCaption("Remove");
	ButtonRemove->eventMouseButtonClick = MyGUI::newDelegate(this, &PanelObjectsList::DeleteCurrentObject);
		
	mPanelCell->setClientHeight(CurrentHeight, false);
}

void PanelObjectsList::UpdateItemsView()
{
	CurrentHeight = 0;
	size_t count = mItemsCheckBox.size();
	for (size_t pos=0; pos<count; ++pos) 
	{
		CurrentHeight = pos*HeightStep;
		mItemsCheckBox[pos]->setPosition(MyGUI::IntCoord(WidthStep, CurrentHeight, Width, Height));
	}
	ButtonAdd->setPosition(MyGUI::IntCoord(WidthStep, CurrentHeight+10, 70, Height));
	ButtonRemove->setPosition(MyGUI::IntCoord(WidthStep+90, CurrentHeight+10, 70, Height));
	mPanelCell->setClientHeight(CurrentHeight+80, false);
}

void PanelObjectsList::shutdownCell()
{
	mItemsCheckBox.clear();
	//mItemsEdit.clear();
	PanelBase::shutdownCell();
}

void PanelObjectsList::mousePressed(MyGUI::WidgetPtr _widget) 
{	
	EditorFrameHandler *handler = EditorFrameListener::GetInstance()->GetCurrentHandler();
	
	size_t count = mItemsCheckBox.size();
	for (size_t pos=0; pos<count; ++pos) 
	{
		((MyGUI::ButtonPtr)mItemsCheckBox[pos])->setStateCheck(false);
	}
		
	if (handler->SelectObject(_widget->getCaption()))
		((MyGUI::ButtonPtr)_widget)->setStateCheck(true);
	else
		((MyGUI::ButtonPtr)_widget)->setStateCheck(false);
}

void PanelObjectsList::AddItem(Ogre::UTFString name)
{		
		MyGUI::ButtonPtr checkbox = mWidgetClient->createWidget<MyGUI::Button>("CheckBox", MyGUI::IntCoord(WidthStep, CurrentHeight, Width, Height), MyGUI::Align::Left | MyGUI::Align::Top | MyGUI::Align::HStretch);		
		checkbox->setCaption(name);		
		checkbox->eventMouseButtonClick = MyGUI::newDelegate(this, &PanelObjectsList::mousePressed);
		mItemsCheckBox.push_back(checkbox);
		
		UpdateItemsView();
}

void PanelObjectsList::RemoveItem(Ogre::UTFString name)
{
	MyGUI::Gui *gui = GUISystem::GetInstance()->GetGui();
	MyGUI::VectorWidgetPtr::iterator iPos = mItemsCheckBox.begin(), iEnd = mItemsCheckBox.end();
	for (;iPos!=iEnd;++iPos)
	{
		if ((*iPos)->getCaption()==name)
		{
			gui->destroyChildWidget(*iPos);
			mItemsCheckBox.erase(iPos);
					
			UpdateItemsView();
			break;
		}
	}	
}

void PanelObjectsList::NewObject(MyGUI::WidgetPtr)
{
	RemoveSelection();
	MyGUI::Gui *gui = GUISystem::GetInstance()->GetGui();
	if (ObjectDescription)
	{
		//ObjectDescription->Hide();
		//gui->destroyWidget(ObjectDescription.get());
		ObjectDescription->Destroy();
		delete ObjectDescription;
		ObjectDescription = NULL;
	}

	
	ObjectDescription = new ObjectDescriptionLayout("EditorObjectOptions.layout");
	ObjectDescription->Load();
	ObjectDescription->Show();
}

void PanelObjectsList::DeleteCurrentObject(MyGUI::WidgetPtr)
{
	EditorFrameHandler *handler = EditorFrameListener::GetInstance()->GetCurrentHandler();
	
	Ogre::UTFString str = handler->GetSelectedObjectName();
	
	if (handler->DeleteSelectedObject())
		RemoveItem(str);
}

void PanelObjectsList::RemoveSelection() 
{	
	EditorFrameHandler *handler = EditorFrameListener::GetInstance()->GetCurrentHandler();
	
	size_t count = mItemsCheckBox.size();
	for (size_t pos=0; pos<count; ++pos) 
	{
		((MyGUI::ButtonPtr)mItemsCheckBox[pos])->setStateCheck(false);
	}
		
	handler->SelectObject("");
}

