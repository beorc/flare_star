#include "StdAfx.h"

#ifdef MEMORY_TEST
#include "mmgr.h"
#endif

#include "ComixFrameHandler.h"
#include "ScriptManager.h"
#include "CommonDeclarations.h"
#include "ComixImage.h"
#include "AApplication.h"
#include "List.h"
#include "GUISystem.h"
#include "GUILayout.h"

ComixFrameHandler::ComixFrameHandler(void) : 
ScriptFrameHandler(),
TimeInHandler(0),
InactveHandler(IH_NONE),
ExitRequested(false),
TimeLimit(0),
CurrentPage(0)
{
}

ComixFrameHandler::~ComixFrameHandler(void)
{
	Close();
}

void ComixFrameHandler::Step(unsigned timeMs)
{
    size_t active_count = 0;
	ScriptFrameHandler::Step(timeMs);
    
	if (CurrentPage<Pages.Size)
	{
		Pages.Data[CurrentPage]->Step(timeMs);
	} else
	{
		ExitRequested = true;
	}

	TimeInHandler+=timeMs;
	if (!ExitRequested)
	{
		if (TimeLimit>0 && TimeInHandler>TimeLimit && Pages.Data[CurrentPage]->ActiveImagesNumber==0)
		{
			ExitRequested = true;		
		}
	}	
	
    if (ExitRequested)
    {
        if (ScriptHandlers[1])
            ScriptManager::GetInstance()->Call(ScriptHandlers[1], "i", false, 0);
        
        switch(InactveHandler) {
        case IH_GUIINTRO:
            {   				
                //Hide();
                CommonDeclarations::GetApplication()->SetCurrentGameState(AApplication::GS_GUI);
                GUISystem::GetInstance()->GetIntroMenu()->Show();
                //ComixFrameListener::GetInstance()->SetCurrentHandler(NULL);            
                break;
            }        	
		case IH_GAME:
			{                
				//Hide();
				CommonDeclarations::GetApplication()->SetCurrentGameState(AApplication::GS_GAME);
				break;
			}
        };
		Close();
    }
}

bool ComixFrameHandler::Parse(TiXmlElement *xml)
{
    assert(xml);
    if (xml == 0)
        return false; // file could not be opened

	ScriptFrameHandler::Parse(xml);

    //const char *str;       
	
    TiXmlElement *xml_element = 0;
		
    //float corner_left=0, corner_up=0, corner_right=0, corner_down=0;
    //int ambient_r=0, ambient_g=0, ambient_b=0;

	xml_element = xml->FirstChildElement("pages");	
	if (xml_element)
	{
		List<ComixPage*> pages;
		xml_element = xml_element->FirstChildElement("page");	
		while (xml_element)
		{
			ComixPage *page = new ComixPage;
			page->Parse(xml_element);
			pages.PushBack(page);

			xml_element = xml_element->NextSiblingElement("page");
		}

		Pages.Resize(pages.GetSize());
		size_t i =0;
		for (List<ComixPage*>::ListNode *pos = pages.GetBegin(); pos!=NULL ;pos=pos->Next, ++i)
		{
			Pages.Data[i] = pos->Value;
		}
	}
    
    int res = 0, val = 0;

    res = xml->QueryIntAttribute("time_limit", &val);
    //assert(TIXML_SUCCESS == res);

	if (TIXML_SUCCESS == res)
		TimeLimit = (unsigned)val;

    res = xml->QueryIntAttribute("inactive_handler", &val);
    InactveHandler = (EInactveHandler)val;

        
    return false;
}

void ComixFrameHandler::Close()
{
	ScriptFrameHandler::Close();

	for (size_t i=0;i<Pages.Size;++i)
	{		       
		ComixPage *page = Pages.Data[i];
		delete page;
	}
	Pages.Clear();
}

//void ComixFrameHandler::Show(int num)
//{
//	ComixImage *image = Images.Data[num];
//	image->Show();
//}
//
//void ComixFrameHandler::SetActive(size_t num, bool on)
//{
//    assert(num<Images.Size);
//    ComixImage *image = Images.Data[num];
//    image->SetActive(on);
//}

void ComixFrameHandler::NextPage()
{
	if (CurrentPage<Pages.Size)
	{
		Pages.Data[CurrentPage]->Hide();
		++CurrentPage;
	}
}

bool ComixFrameHandler::KeyPressed( const OIS::KeyEvent &arg )
{	
	if( arg.key == OIS::KC_LCONTROL ||
		arg.key == OIS::KC_RCONTROL ||
		arg.key == OIS::KC_LMENU ||
		arg.key == OIS::KC_RMENU ||
		arg.key == OIS::KC_TAB)
		return false;
	
	NextPage();
	return true;
}

bool ComixFrameHandler::MousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	NextPage();
	return true;
}

ComixPage::~ComixPage()
{
	Close();
}

void ComixPage::Close()
{
	for (size_t i=0;i<Images.Size;++i)
	{		       
		ComixImage *image = Images.Data[i];
		delete image;
	}
	Images.Clear();
}

bool ComixPage::Parse(TiXmlElement *xml)
{
	List<ComixImage*> images;

	TiXmlElement *xml_element = NULL;
	xml_element = xml->FirstChildElement("images");	
	if (xml_element)
	{
		xml_element = xml_element->FirstChildElement("image");	
		while (xml_element)
		{
			ComixImage *image = new ComixImage;
			image->Parse(xml_element);
			images.PushBack(image);

			xml_element = xml_element->NextSiblingElement("image");
		}		
	}

	Images.Resize(images.GetSize());
	size_t i =0;
	for (List<ComixImage*>::ListNode *pos = images.GetBegin(); pos!=NULL ;pos=pos->Next, ++i)
	{
		Images.Data[i] = pos->Value;
	}
	return true;
}

void ComixPage::Step(unsigned timeMs)
{
	ActiveImagesNumber = 0;
	for (size_t i =0; i< Images.Size;++i)
	{
		ComixImage *image = Images.Data[i];
		if (image->IsActive())
		{   
			++ActiveImagesNumber;
		}
		image->Step(timeMs);
	}
}

void ComixPage::Hide()
{        
	for (size_t i=0;i<Images.Size;++i)
	{		       
		ComixImage *image = Images.Data[i];
		image->Hide();
	}    
}


