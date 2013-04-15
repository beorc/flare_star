#pragma once
#include "GUILayout.h"

class GUIMessageLayout : public GUILayout
{
public:
	GUIMessageLayout(const char *name);
	virtual ~GUIMessageLayout(void);

	virtual bool Load();
	virtual void Show();
	void mousePressed(MyGUI::WidgetPtr _widget);	

};
