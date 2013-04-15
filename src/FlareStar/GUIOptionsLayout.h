#pragma once
#include "GUILayout.h"

class GUIOptionsLayout : public GUILayout
{
public:
	GUIOptionsLayout(const char *name);
	virtual ~GUIOptionsLayout(void);

	virtual bool Load();
	virtual void Show(GUILayout *back_layout=NULL);
	void mousePressed(MyGUI::WidgetPtr _widget);	
};
