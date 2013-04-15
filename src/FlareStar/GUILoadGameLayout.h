#pragma once
#include "GUILayout.h"

class GUILoadGameLayout : public GUILayout
{
public:
	GUILoadGameLayout(const char *name);
	virtual ~GUILoadGameLayout(void);

	virtual bool Load();
	virtual void Show(GUILayout *back_layout=NULL);
	void mousePressed(MyGUI::WidgetPtr _widget);
	void LoadGameHandler(MyGUI::WidgetPtr _widget);
protected:
	std::vector<MyGUI::ButtonPtr> GameButtons;

};
