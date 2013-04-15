#include "StdAfx.h"

#ifdef MEMORY_TEST
#include "mmgr.h"
#endif

#include "CombatFrameHandler.h"
#include "ScriptManager.h"
#include "CommonDeclarations.h"
#include "ComixImage.h"
#include "AApplication.h"
#include "List.h"
#include "GUISystem.h"
#include "GUILayout.h"
#include "IAAObject.h"
#include "Player.h"

CombatFrameHandler::CombatFrameHandler(void) : 
ScriptFrameHandler(),
ExitRequested(false),
CameraPosition(Ogre::Vector3::ZERO),
CameraNode(NULL)
{
}

CombatFrameHandler::~CombatFrameHandler(void)
{
	Close();
}

void CombatFrameHandler::Step(unsigned timeMs)
{    
	//ScriptFrameHandler::Step(timeMs);

	//CommonDeclarations::GetApplication()->GetTurnBasedCamera()->setPosition(CameraPosition);

	CommonDeclarations::ObjectsPool *objects = CommonDeclarations::GetObjects();
	
	Ogre::Vector3 curr_position, new_position;
	for (CommonDeclarations::ObjectsPool::ListNode *pos = objects->GetBegin();pos!=NULL;pos=pos->Next)
	{
		ICombatable *tb = pos->Value->GetCombatable();
		if (tb)
		{
			tb->Step(timeMs);			
		}
	}
	
    if (ExitRequested)
    {
        if (ScriptHandlers[1])
            ScriptManager::GetInstance()->Call(ScriptHandlers[1], "i", false, 0);
        
		CommonDeclarations::GetApplication()->SetCurrentGameState(AApplication::GS_GAME);

		Close();
    }
}

bool CombatFrameHandler::ConfigureGlobal(TiXmlElement *xml)
{
    assert(xml);
    if (xml == 0)
        return false; // file could not be opened

	TiXmlElement *xml_element, *camera_xml;
	xml_element = xml->FirstChildElement("turn_based");
	//assert(strcmp(str,"effects")==0);
	if (xml_element)
	{
		camera_xml = xml_element->FirstChildElement("camera");
		if (camera_xml)
		{
			int x, y ,z, resx, resy, resz;
			resx = xml_element->QueryIntAttribute("x", &x);	
			assert(TIXML_SUCCESS==resx);
			resy = xml_element->QueryIntAttribute("y", &y);	
			assert(TIXML_SUCCESS==resy);
			resz = xml_element->QueryIntAttribute("z", &z);	
			assert(TIXML_SUCCESS==resz);

			if (TIXML_SUCCESS==resx)
			{		
				CameraPosition.x = x;
			}
			if (TIXML_SUCCESS==resy)
			{
				CameraPosition.y = y;
			}
			if (TIXML_SUCCESS==resz)
			{
				CameraPosition.z = z;
			}
		}		
	}
            
    return false;
}

bool CombatFrameHandler::Parse(TiXmlElement *xml)
{
    assert(xml);
    if (xml == 0)
        return false; // file could not be opened

	ScriptFrameHandler::Parse(xml);
            
    return false;
}

void CombatFrameHandler::Close()
{
	Exit();
	ScriptFrameHandler::Close();
}

bool CombatFrameHandler::KeyPressed( const OIS::KeyEvent &arg )
{	
	if( arg.key == OIS::KC_LCONTROL ||
		arg.key == OIS::KC_RCONTROL ||
		arg.key == OIS::KC_LMENU ||
		arg.key == OIS::KC_RMENU ||
		arg.key == OIS::KC_TAB)
		return false;
	
	return true;
}

bool CombatFrameHandler::MousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	return true;
}

void CombatFrameHandler::Start()
{
}

void CombatFrameHandler::Stop()
{
}

void CombatFrameHandler::Init()
{
	TiXmlElement *game_config = CommonDeclarations::GetGameConfig();
    assert(game_config);
    if (game_config)
        ConfigureGlobal(game_config);

	Ogre::Camera *cam = CommonDeclarations::GetApplication()->GetCombatCamera();
	CameraNode = cam->getParentSceneNode();
	if (NULL==CameraNode)
		CameraNode = CommonDeclarations::GetSceneManager()->getRootSceneNode()->createChildSceneNode();
	Ogre::Vector3 pos = CommonDeclarations::GetPlayer()->GetPosition();
	pos.y = 0;
	CameraNode->setPosition(pos);
	CameraNode->attachObject(cam);
	cam->setPosition(CameraPosition);
	cam->lookAt(CameraNode->getPosition());
}

void CombatFrameHandler::InitObjects()
{
	CommonDeclarations::ObjectsPool *objects = CommonDeclarations::GetObjects();
	
	Ogre::Vector3 curr_position, new_position;
	for (CommonDeclarations::ObjectsPool::ListNode *pos = objects->GetBegin();pos!=NULL;pos=pos->Next)
	{
		ICombatable *tb = pos->Value->GetCombatable();
		/*if (tb)
		{
			ICombatable::SRealTimeState *state = tb->GetRealTimeState();
			new_position = pos->Value->GetPosition();			
			state->Position = new_position;
			tb->SetRealTimeState(state);
			new_position.y = 0;
			pos->Value->SetPosition(new_position);			
		}*/
	}
}

void CombatFrameHandler::Exit()
{
	CommonDeclarations::ObjectsPool *objects = CommonDeclarations::GetObjects();
	
	Ogre::Vector3 curr_position, new_position;
	for (CommonDeclarations::ObjectsPool::ListNode *pos = objects->GetBegin();pos!=NULL;pos=pos->Next)
	{
		ICombatable *tb = pos->Value->GetCombatable();
		/*if (tb)
		{
			ICombatable::SRealTimeState *state = tb->GetRealTimeState();
			pos->Value->SetPosition(state->Position);
		}*/
	}
}

/*
bool WW_Map_View::Find_Way(WW_Character *pers, Map_Cell dest)
{
	bool checked=false;
	int x,y, step=1 , i,j, min, module, number;
	bool found=false;
	Map_Cell beg_cell, lu, rd, curr_cell;
	pers->Get_Pos(&(beg_cell.x), &(beg_cell.y));
	//pers->Get_Pos(&x,&y);
	//Get_Map_Coords_From_Screen_Coords(x, y, &(beg_cell.x), &(beg_cell.y));
	vector<Map_Cell> conquring_cells;

	if (beg_cell.x==dest.x && beg_cell.y==dest.y)
		return true;

	
	Update_Search_Map();
	Search_Map[beg_cell.x][beg_cell.y]=2;
	//Draw_Cell(beg_cell.x, beg_cell.y,255,0,0);

	//Show();
	
	while (!found)
	{
		lu.x=beg_cell.x-step;
		lu.y=beg_cell.y-step;
		rd.x=beg_cell.x+step;
		rd.y=beg_cell.y+step;
		Check_Bounds_For_Cell(&lu);
		Check_Bounds_For_Cell(&rd);
				

		checked=false;
		for (y=lu.y;y<=rd.y;y++)
		{			
			for (x=lu.x;x<=rd.x;x++)
			{		
				
				if (Search_Map[x][y]==step+1)
				{
					
					for (i=-1;i<=1;i++)
					{
						if (Is_X_Out_Matrix(x+i))
								continue;
						for (j=-1;j<=1;j++)
						{
							//
							if (Is_Y_Out_Matrix(y+j))
								continue;
							if (Search_Map[x+i][y+j]==0)
							{
								
								Search_Map[x+i][y+j]=step+2;
								//Draw_Cell(x+i, y+j, 0,0,0);    
								checked=true;
							}
														
							if ( x+i==dest.x && y+j==dest.y)
								found=true;
							
						}
					}				
										
				}
				
			}
			
		}
				
		if (!checked)
		{
			WW_Log::Write_Message("No such way");
			return false;
		}

				
		++step;
		
	}

	
	curr_cell.x=dest.x;
	curr_cell.y=dest.y;
			
	
	for (step;step>1;step--)
	{	
		
		Search_Map[curr_cell.x][curr_cell.y]=0;
		//Draw_Cell(curr_cell.x, curr_cell.y);
		pers->Add_Step_To_Way(curr_cell);			

		checked=false;
		conquring_cells.clear();
		        
		for (i=-1;i<=1;i++)
		{
			if (Is_X_Out_Matrix(curr_cell.x+i))
				continue;
			for (j=-1;j<=1;j++)
			{				
				if (Is_Y_Out_Matrix(curr_cell.y+j))
					continue;
				min=Search_Map[curr_cell.x+i][curr_cell.y+j];
				if (Search_Map[curr_cell.x+i][curr_cell.y+j]==step)
				{
					conquring_cells.push_back(Map_Cell(curr_cell.x+i,curr_cell.y+j));					
					checked=true;			
				}			
				
			}
			
		}

		    if (checked)
			{
				min=module_of_shift(conquring_cells[0],beg_cell);
				number=0;
				for ( i=1;i<conquring_cells.size();i++)
				{
					module=module_of_shift(conquring_cells[i],beg_cell);
					if (min>module)
					{
						min=module;
						number=i;
					}
				}
				curr_cell.x=conquring_cells[number].x;
			    curr_cell.y=conquring_cells[number].y;				
			}	

		if (!checked)
		{
			WW_Log::Write_Message("Not found next step in good way");			
			exit(0);
		}			
				
		
	}	

	return true;
	
}
*/