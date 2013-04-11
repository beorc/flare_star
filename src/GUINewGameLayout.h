#pragma once
#include "GUILayout.h"

class GUINewGameLayout : public GUILayout
{
public:
	GUINewGameLayout(const char *name);
	virtual ~GUINewGameLayout(void);

	virtual bool Load();
	virtual void Show(GUILayout *back_layout=NULL);
	void mousePressed(MyGUI::WidgetPtr _widget);	

};
