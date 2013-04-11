#include "StdAfx.h"

#ifdef MEMORY_TEST
#include "mmgr.h"
#endif

#include ".\guilayout.h"
#include "CommonDeclarations.h"
#include "Utilities.h"
#include "GUISystem.h"

GUILayout::GUILayout(const char *name) :
BackLayout(NULL)
{
	Name = AAUtilities::StringCopy(name);
}

GUILayout::~GUILayout(void)
{
	if (Name)
		delete [] Name;	
}

void GUILayout::Destroy()
{
	if (!Widgets.empty())
	{
		MyGUI::Gui *gui = GUISystem::GetInstance()->GetGui();
		gui->destroyWidgetsVector(Widgets);
		Widgets.clear();
	}
}

bool GUILayout::Load()
{
	Widgets = MyGUI::LayoutManager::getInstance().load(Name, GUI_RESOURCE_GROUP);
	Hide();

	// set callback
	/*MyGUI::ButtonPtr button = GUI->findWidget<MyGUI::Button>("ButtonQuit");
	button->eventMouseButtonClick = MyGUI::newDelegate(this, &GUISystem::mousePressed);*/
	//
	

	return true;
}

bool GUILayout::Init()
{	
	return true;
}

void GUILayout::Show(GUILayout *back_layout)
{	
	BackLayout = back_layout;
	MyGUI::VectorWidgetPtr::iterator iPos = Widgets.begin(), iEnd=Widgets.end();
	for (;iPos!=iEnd;++iPos)
	{
		(*iPos)->show();
	}
}

void GUILayout::Hide()
{
	MyGUI::VectorWidgetPtr::iterator iPos = Widgets.begin(), iEnd=Widgets.end();
	for (;iPos!=iEnd;++iPos)
	{
		(*iPos)->hide();
	}
}

GUILayout *GUILayout::GetBackLayout() const
{
	return BackLayout;
}

