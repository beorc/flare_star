#pragma once

#include <MyGUI.h>
#include <Ogre.h>

class GUILayout
{
public:
	GUILayout(const char *name);	
	virtual ~GUILayout(void);

	virtual bool Load();
	virtual bool Init();
	virtual void Destroy();
	
	virtual void Hide();
	virtual void Show(GUILayout *back_layout=NULL);

	GUILayout *GetBackLayout() const;	
protected:
	const char *Name;
	MyGUI::VectorWidgetPtr Widgets;
	GUILayout *BackLayout;
};
