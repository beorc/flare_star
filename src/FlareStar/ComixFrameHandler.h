#pragma once

#include "IFrameHandler.h"

class TiXmlElement;

#include "ScriptFrameHandler.h"
#include "Array.h"
//#include "List.h"

class ComixImage;

struct ComixPage {
	~ComixPage();
	
	bool Parse(TiXmlElement *xml);
	void Step(unsigned timeMs);
	void Close();
	void Hide();
	
	Array<ComixImage*> Images;
	int ActiveImagesNumber;
	//unsigned Timing, TimeLimit, TimeOnPage;			
};

class ComixFrameHandler : public ScriptFrameHandler
{
public:
    enum EInactveHandler {IH_NONE, IH_GUIINTRO, IH_GAME};
    ComixFrameHandler(void);
    ~ComixFrameHandler(void);

    void Step(unsigned timeMs);
    bool Parse(TiXmlElement *xml);
	void Close();

	void NextPage();

	//void Show(int num);
    //void Hide();
    
    //void SetActive(size_t num, bool on);

	virtual bool KeyPressed( const OIS::KeyEvent &arg );
	virtual bool MousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
	    

protected:
    Array<ComixPage*> Pages;
    unsigned Timing, TimeLimit, TimeInHandler;
    EInactveHandler InactveHandler;
	bool ExitRequested;
	size_t CurrentPage;
};
