Vars<-{
NumOfKilledPirates=0,
MusicGain=0.0,
FinalSceneActivated=false,
PiratesBorned=false,
TransportSceneTimeCounter=0
}

function PirateKilled( id )
{
	Vars.NumOfKilledPirates++;
	//::print("pirate killed "+id+" "+Vars.NumOfKilledPirates+"\n");
	
	if (Vars.NumOfKilledPirates==39)
	{
		//::print("mode 2 level1 ends!!!\n");
		//конец уровня
		local pos=[1,2,3];
	    pos=GetPivotPos(1);
		
		BlockPlayer(true);
		SwitchCamera(1, pos[0],pos[1],pos[2]+5000, 0,1,0);
		SetScriptsCameraTrack(1,true);
		SetHudVisible(false);
		SetGameState(5);
	}
}

function Timer( val )
{		
	//local pos=[1,2,3];
    //pos=GetPivotPos(1);
	//::print("x="+pos[0]+" y="+pos[1]+" z="+pos[2]+"\n");	
	// get health for 103
	//::print("timer\n");
	local boss_health=0;
	boss_health = GetHealth(103);
	
	//::print("test="+test+"\n");	
	//::print("health="+boss_health+"\n");	
	//boss_health>-333 && 
	if ((boss_health<60 && Vars.NumOfKilledPirates>20) || Vars.NumOfKilledPirates>32)
	{
		//::print("born laokes\n");
		//born laokes
		ActivateObject(106, true);
		//SetScriptHandlersActive(false);
		SetScriptHandler(0, 0, 0);
	}
}

function MeteoRandom( id )
{		
	local pos=[1,2,3], shifts=[0,0,0,0], rand=0.0, radius=0, mass=0, push_force=0;
    
	local dir=[0.0,0.0,1.0];
    dir=GetDirection(id);
	//::print("x="+dir[0]+" y="+dir[1]+" z="+dir[2]+"\n");
	
		//::print(id+" created by incub\n");	
		SetLinDamping(id, 0);
		SetRotDamping(id, 0);
		SetForwardDirection(id,-1,0,0);
		//SetAcceleration(id,-1,0,0);
		rand = Rand();
		//::print("id= "+id+" rand: "+rand+"\n");
		shifts[0] = rand*20000;				
		
		mass = GetMass(id);
		//AddForce(id,0,0,0,0,0,60000+mass*shifts[0]);
		//::print("Add push force. force: "+(60000+mass*shifts[0])+"\n");
		push_force = 1500000+mass*shifts[0]+mass*mass;
		AddForce(id,0,0,0,dir[0]*push_force,dir[1]*push_force,dir[2]*push_force);
		radius = GetRadius(id);
		
		shifts[0] = Rand()*radius-Rand()*radius;
		shifts[1] = Rand()*radius-Rand()*radius;
		shifts[2] = Rand()*radius-Rand()*radius;
		shifts[3] = 3000*mass + Rand()*100000;
		//::print("ost: "+shifts[0]+" - "+shifts[1]+" - "+shifts[2]+"\n");
		//::print("add rot force. point:"+(shifts[0])+", "+(shifts[1])+", "+(shifts[2])+"; force:"+(shifts[3])+"\n");
		AddForce(id,3*shifts[0],3*shifts[1],3*shifts[2],0,0,shifts[3]);
	
	//::print("x="+pos[0]+" y="+pos[1]+" z="+pos[2]+"\n");	
}

function MeteoRandomSided( id )
{		
	local pos=[1,2,3], shifts=[0,0,0,0], rand=0.0, radius=0, mass=0, push_force=0;
    
	local dir=[0.0,0.0,1.0];
    dir=GetDirection(id);
	//::print("x="+dir[0]+" y="+dir[1]+" z="+dir[2]+"\n");
	
		//::print(id+" created by incub\n");	
		SetLinDamping(id, 0);
		SetRotDamping(id, 0);
		SetForwardDirection(id,-1,0,0);
		//SetAcceleration(id,-1,0,0);
		rand = Rand();
		//::print("id= "+id+" rand: "+rand+"\n");
		shifts[0] = rand*20000;				
		
		mass = GetMass(id);
		//AddForce(id,0,0,0,0,0,60000+mass*shifts[0]);
		//::print("Add push force. force: "+(60000+mass*shifts[0])+"\n");
		push_force = 1500000+mass*shifts[0]+mass*mass*500;
		AddForce(id,0,0,0,dir[0]*push_force,dir[1]*push_force,dir[2]*push_force);
		radius = GetRadius(id);
		
		shifts[0] = Rand()*radius-Rand()*radius;
		shifts[1] = Rand()*radius-Rand()*radius;
		shifts[2] = Rand()*radius-Rand()*radius;
		shifts[3] = 3000*mass + Rand()*100000;
		//::print("ost: "+shifts[0]+" - "+shifts[1]+" - "+shifts[2]+"\n");
		//::print("add rot force. point:"+(shifts[0])+", "+(shifts[1])+", "+(shifts[2])+"; force:"+(shifts[3])+"\n");
		AddForce(id,3*shifts[0],3*shifts[1],3*shifts[2],0,0,shifts[3]);
	
	//::print("x="+pos[0]+" y="+pos[1]+" z="+pos[2]+"\n");	
}

function MeteoDummy( id )
{		
	local pos=[1,2,3], shifts=[0,0,0,0], rand=0.0, radius=0, mass=0, push_force=0;
    
	local dir=[0.0,0.0,1.0];
    dir=GetDirection(id);
	//::print("x="+dir[0]+" y="+dir[1]+" z="+dir[2]+"\n");
	
		//::print(id+" created by incub\n");	
		SetLinDamping(id, 0);
		SetRotDamping(id, 0);
		SetForwardDirection(id,-1,0,0);
		//SetAcceleration(id,-1,0,0);
		rand = Rand();
		//::print("id= "+id+" rand: "+rand+"\n");
		shifts[0] = rand*20000;				
		
		mass = GetMass(id);
		//AddForce(id,0,0,0,0,0,60000+mass*shifts[0]);
		//::print("Add push force. force: "+(60000+mass*shifts[0])+"\n");
		push_force = 2000000+mass*shifts[0]+mass*mass*600;
		AddForce(id,0,0,0,dir[0]*push_force,dir[1]*push_force,dir[2]*push_force);
		radius = GetRadius(id);
		
		shifts[0] = Rand()*radius-Rand()*radius;
		shifts[1] = Rand()*radius-Rand()*radius;
		shifts[2] = Rand()*radius-Rand()*radius;
		shifts[3] = 3000*mass + Rand()*100000;
		//::print("ost: "+shifts[0]+" - "+shifts[1]+" - "+shifts[2]+"\n");
		//::print("add rot force. point:"+(shifts[0])+", "+(shifts[1])+", "+(shifts[2])+"; force:"+(shifts[3])+"\n");
		AddForce(id,3*shifts[0],3*shifts[1],3*shifts[2],0,0,shifts[3]);
	
	//::print("x="+pos[0]+" y="+pos[1]+" z="+pos[2]+"\n");	
}


function MeteoConst( id )
{		
//::print("MeteoConst called\n");	
	local pos=[1,2,3], shifts=[0,0,0,0], rand=0.0, radius=0, mass=0;    	
	
		SetLinDamping(id, 0);
		SetRotDamping(id, 0);
		SetForwardDirection(id,-1,0,0);
		
		rand = Rand();
				
		mass = GetMass(id);		
		AddForce(id,0,0,0,0,0,1500000+mass*1000);
		radius = GetRadius(id);
		
		shifts[0] = radius+Rand()*radius;
		shifts[1] = radius+Rand()*radius;
		shifts[2] = radius+Rand()*radius;
		shifts[3] = 2200*mass + Rand()*100000;		
		
	AddForce(id,3*shifts[0],3*shifts[1],3*shifts[2],0,0,shifts[3]);
	
}

function TransportCreated( id )
{
//::print("transport created called\n");
	SetVisible(id, false);
}

function OnOutOfRooms( id )
{		
//::print("out of rooms called\n");
	if (id>200 || id==103)
		Destroy(id);	
}

function FinalSceneIn( mode )
{
	/*if (Vars.FinalSceneActivated==false)
	{
		Vars.FinalSceneActivated=true;
		SetGameState(3);
		SetHandler(1, 23);
		SetRoomOnly(1, 0);
	}
	*/	
}

function TransportScene( mode )
{	
		SetGameState(5);
		//::print("final scene script called\n");		
		
		Vars.MusicGain = GetMusicGain();
		
		if (Vars.MusicGain>0)
			SetMusicGain(0.1);
		
		ProcessEffect(-1, 21, false);
		//SetFog(0, 0, 0,0,0, 0,0,0);
		
		BlockPlayer(true);	
		ActivateObject(1, false);
		
		for(local i=2;i<12;i+=1)
		{
		    ActivateObject(i,false);
			SwitchTrigger(i,false);
		}
		ActivateObject(12,false);
		
		SetVisible(100, true);
		ActivateObject(100, true);
		ActivateObject(101, true);
				
		SwitchCamera(1, -10000,10000,-240000, 0,1,0);	
		SetScriptsCameraTrack(100,true);	
		
		SetHudVisible(false);
		
		SetScriptHandler(0, "TransportSceneTimer", 1000);
}

function TransportSceneTimer( val )
{	
	local boss_health=0;
	boss_health = GetHealth(100);
	
	if (boss_health<0)
	{
		//::print("transport timer"+Vars.TransportSceneTimeCounter+"\n");
		if (Vars.TransportSceneTimeCounter>3)
		{
			SetGameState(1);	
			Destroy(101);
			SwitchCamera(0);
						
			BlockPlayer(false);
			ActivateObject(1, true);
			SetCompasTarget(110,1);
			SetAutoPilotTarget(110);
		
			SetScriptHandler(0, 0, 0);
			
			SetHudVisible(true);
	
			if (Vars.MusicGain>0)
				SetMusicGain(Vars.MusicGain);
		} else
		{
			Vars.TransportSceneTimeCounter++;
		}
	}
}

function TransportKilled( id )
{
//::print("transport killed\n");
	
	SetScriptsCameraTrack(0,false);
}

function GoPiratesGo( mode )
{
	//SwitchCamera(2, 80000,80000,-260000, -1,-1,-1);
	//SetGameState(6);
	//SetHandler(2, 37, -50000, -50000, -230000, 50000, 50000, -130000);
	
	if (Vars.PiratesBorned==false)
	{
		Vars.PiratesBorned==true;
		//::print("go pirates called\n");		
		
		SetCompasTarget(0,0);		
		//SetCompasTarget(0, 0, -180000);
		SetAutoPilotTarget(0, 0, -180000);
		
		ActivateObject(102, true);
		//SetScriptHandlersActive(true);
		SetScriptHandler(0, "Timer", 1000);
		//SetScriptHandler(0, 0, 0);
	}	
}

function Start( id )
{		
	SetCompasTarget(100, 0);
	SetAutoPilotTarget(100);
	//SetGameState(3);
	//SetHandler(1, 22);
}
