#pragma once
#include "GUILayout.h"

class GUIDescriptionLayout : public GUILayout
{
public:
	GUIDescriptionLayout(const char *name);
	virtual ~GUIDescriptionLayout(void);

	virtual bool Load();
	virtual void Show();
	void mousePressed(MyGUI::WidgetPtr _widget);	

};
