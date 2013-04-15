#pragma once
#include "GUILayout.h"

class GUIIntroMenuLayout : public GUILayout
{
public:
	GUIIntroMenuLayout(const char *name);
	virtual ~GUIIntroMenuLayout(void);

	virtual bool Load();
	void mousePressed(MyGUI::WidgetPtr _widget);

	virtual void Hide();
	virtual void Show(GUILayout *back_layout=NULL);

protected:
};
