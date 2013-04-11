#pragma once
#include "GUILayout.h"

class GUIMainMenuLayout : public GUILayout
{
public:
	GUIMainMenuLayout(const char *name);
	virtual ~GUIMainMenuLayout(void);

	virtual bool Load();
	void mousePressed(MyGUI::WidgetPtr _widget);	
};
