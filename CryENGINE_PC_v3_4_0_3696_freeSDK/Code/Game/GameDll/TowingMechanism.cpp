/*************************************************************************
TowingMechanism.cpp
Started:
- 10:22:2015   16:01 : Created by Dauji Vaid
*************************************************************************/

#include "StdAfx.h"
#include "TowingMechanism.h"

std::vector<int> CTowingMechanism::Nodes;
std::list<int> CTowingMechanism::nodeConnectors;
int CTowingMechanism::pEntityOneID;
int CTowingMechanism::pEntityTwoID;
bool CTowingMechanism::EntityOneIsChild;
bool CTowingMechanism::EntityTwoIsChild;
bool CTowingMechanism::IsEntitesConnected;
IGameObject* CTowingMechanism::EntityOne;
IGameObject* CTowingMechanism::EntityTwo;
IGameObject* CTowingMechanism::humvee;
IGameObject* CTowingMechanism::tractor;

void CTowingMechanism::ActivateTowing()
{
	if (EntityOneIsChild == true) 
	{
		EntityTwoIsChild = false;
		EntityOne->GetEntity()->SetRotation(EntityTwo->GetEntity()->GetRotation());
		EntityOne->GetEntity()->SetPos(EntityTwo->GetEntity()->GetPos() - Vec3(3,3,0));
	}

	if (EntityTwoIsChild == true) 
	{
		EntityOneIsChild = false;
		EntityTwo->GetEntity()->SetRotation(EntityOne->GetEntity()->GetRotation());
		EntityTwo->GetEntity()->SetPos(EntityOne->GetEntity()->GetPos() - Vec3(3,3,0));
	}
}

void CTowingMechanism::DeactivateTowing()
{
	EntityOneIsChild = false;
	EntityTwoIsChild = false;

	EntityTwo->GetEntity()->SetRotation(EntityTwo->GetEntity()->GetRotation());
	EntityOne->GetEntity()->SetRotation(EntityOne->GetEntity()->GetRotation());
}

bool CTowingMechanism::IsVehiclesCloseBy()
{
	Vec3 entityOnePos = tractor->GetEntity()->GetWorldPos();
	Vec3 entityTwoPos = humvee->GetEntity()->GetWorldPos();
	float distance = entityOnePos.GetDistance(entityTwoPos);

	if (distance < 6.0) { return true; }
	else { return false; }
	return false;
}

bool CTowingMechanism::Init( IGameObject * pGameObject )
{	
	SetGameObject(pGameObject);
	GetEntity()->Activate(true);
	Nodes.clear();
	return true;
}

void CTowingMechanism::PostInit( IGameObject * pGameObject )
{
	pGameObject->EnableUpdateSlot(this, 0);

	tractor = g_pGame->GetIGameFramework()->GetGameObject(CTractor::TractorID);
	humvee = g_pGame->GetIGameFramework()->GetGameObject(CHumvee::HumveeID);
	Nodes.clear();
	EntityOneIsChild = false;
	EntityTwoIsChild = false;
	EntityOneIsChild = false;
	EntityTwoIsChild = false;
	IsEntitesConnected = false;
}

void CTowingMechanism::Update( SEntityUpdateContext& ctx, int updateSlot )
{
	if (gEnv->IsEditorGameMode())
	{
		if (IsEntitesConnected)
		{
			ActivateTowing();
		}
	}
}

void CTowingMechanism::DisplayMessage(const char* messageText, ColorF col, float XPos, float yPos, float size, float timeOut)
{
	g_pGame->GetIGameFramework()->GetIPersistantDebug()->Begin("TestAddPersistentText2D", false);
	g_pGame->GetIGameFramework()->GetIPersistantDebug()->AddText(XPos, yPos, size, col, timeOut, messageText);
}

void CTowingMechanism::NodesManagement(int nodeStateProvided)
{
	if (nodeStateProvided == 0)
	{
		if (CTowingMechanism::Nodes[0] == 1)
		{
			CTowingMechanism::Nodes.push_back(0);

			EntityOne = g_pGame->GetIGameFramework()->GetGameObject(pEntityOneID);
			EntityTwo = g_pGame->GetIGameFramework()->GetGameObject(pEntityTwoID);
			DisplayMessage("Rope Connected (2/2)", ColorF(1,0,0), 10,70,5,3);

			EntityOneIsChild = true;
		}
		else if (CTowingMechanism::Nodes[0] == 0)
		{
			DisplayMessage("Error: Can't connect here! Try other side", ColorF(1,0,0), 10,70,5,3);
		}
	}

	if (nodeStateProvided == 1)
	{
		if (CTowingMechanism::Nodes[0] == 1)
		{
			DisplayMessage("Error: Can't connect here! Try other side", ColorF(1,0,0), 10,70,5,3);
		}
		else if (CTowingMechanism::Nodes[0] == 0)
		{
			CTowingMechanism::Nodes.push_back(1);

			EntityOne = g_pGame->GetIGameFramework()->GetGameObject(pEntityOneID);
			EntityTwo = g_pGame->GetIGameFramework()->GetGameObject(pEntityTwoID);	
			DisplayMessage("Rope Connected (2/2)", ColorF(1,0,0), 10,70,5,3);

			EntityTwoIsChild = true;
		}
	}

	if (nodeStateProvided  == 2) 
	{
		DeactivateTowing();
	}
}

void CTowingMechanism::GetNodeOne(int EntityOneID)
{
	pEntityOneID = EntityOneID;
}
void  CTowingMechanism::GetNodeTwo(int EntityTwoID)
{
	pEntityTwoID = EntityTwoID;
}





