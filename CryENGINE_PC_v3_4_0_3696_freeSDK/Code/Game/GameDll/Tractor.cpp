/*************************************************************************
Tractor.cpp
Started:
- 10:22:2015   16:01 : Created by Dauji Vaid
*************************************************************************/

#include "StdAfx.h"
#include "Tractor.h"

bool CTractor::isAlreadyConnectedToRope;
int CTractor::TractorID;

CTractor::CTractor()
{
	gEnv->pSystem->GetIInput()->AddEventListener(this);	
}

CTractor::~CTractor()
{
	gEnv->pSystem->GetIInput()->RemoveEventListener(this);
}

bool CTractor::Init( IGameObject * pGameObject )
{
	SetGameObject(pGameObject);
	GetEntity()->Activate(true);

	return true;
}

void CTractor::PostInit( IGameObject * pGameObject )
{
	TractorID = GetEntity()->GetId();
	// CryLogAlways("TRACTOR_ID: %d", TractorID);	//3
	pGameObject->EnableUpdateSlot(this, 0);
	GetEntity()->LoadGeometry(0, "Objects/Objects/vehicles/tractor/tractor.cgf");
	SEntityPhysicalizeParams PhysParams;
	PhysParams.mass = 30;
	PhysParams.type = PE_RIGID;
	GetEntity()->Physicalize(PhysParams);

	humvee = g_pGame->GetIGameFramework()->GetGameObject(CHumvee::HumveeID);
	tractor = g_pGame->GetIGameFramework()->GetGameObject(TractorID);
	displayMessageTractorRear = false;
	displayMessageTractorFront = false;
	buttonPressedTractorRear = false;
	buttonPressedTractorFront = false;
	append = false;
	appendTwice = false;
	isAlreadyConnectedToRope = false;
}

void CTractor::Update( SEntityUpdateContext& ctx, int updateSlot )
{
	if (gEnv->IsEditorGameMode())
	{
		isOtherVehicleAround = CTowingMechanism::IsVehiclesCloseBy();

		isPlayerCollidingWithTractorRear = VehicleBase::VehicleRearRayCast(tractor, isPlayerCollidingWithTractorRear, 3);
		isPlayerCollidingWithTractorFront = VehicleBase::VehicleFrontRayCast(tractor, isPlayerCollidingWithTractorFront, 3);

		if (isPlayerCollidingWithTractorRear == true)
		{
			if (!displayMessageTractorRear)
			{
				if(isOtherVehicleAround)
				{
					VehicleBase::VehicleRayCastMessage("Tractor (1): Press T to attach rope in back", ColorF(1,0,0), 10,10,5,3);
				}
				else
				{
					VehicleBase::VehicleRayCastMessage("ERROR: Other vechile is not close enough", ColorF(1,0,0), 10,80,5,3);
				}
				displayMessageTractorRear = true;
			}

			if (buttonPressedTractorRear == true && isOtherVehicleAround)
			{
				if(!append)
				{
					if (CTowingMechanism::Nodes.size() == 0)
					{
						if (!isAlreadyConnectedToRope)
						{
							CTowingMechanism::IsEntitesConnected = true;
							CTowingMechanism::Nodes.insert(CTowingMechanism::Nodes.begin(), 0);
							CTowingMechanism::GetNodeOne(GetEntity()->GetId());	
							isAlreadyConnectedToRope = true;
							VehicleBase::VehicleRayCastMessage("Tractor (1): Connected (1/2)", ColorF(1,0,0), 10,80,5,3);
						}
					}
					else if (CTowingMechanism::Nodes.size() == 1)
					{
						if (!isAlreadyConnectedToRope)
						{
							CTowingMechanism::GetNodeTwo(GetEntity()->GetId());
							CTowingMechanism::NodesManagement(0);
						}
						else
						{
							VehicleBase::VehicleRayCastMessage("ERROR: Can't connect to same vehicle", ColorF(1,0,0), 10,80,5,3);
						}

					}
					else if (CTowingMechanism::Nodes.size() == 2)
					{
						VehicleBase::VehicleRayCastMessage("Rope Disconnected", ColorF(1,0,0), 10,80,5,3);
						CTowingMechanism::Nodes.clear();
						CTowingMechanism::IsEntitesConnected = false;
						CTowingMechanism::NodesManagement(2);	
						isAlreadyConnectedToRope = false;
						CHumvee::isAlreadyConnectedToRope_Humvee = false;
						CTowingMechanism::EntityOneIsChild = false;
						CTowingMechanism::EntityTwoIsChild = false;
						append = false;
					}
					else {}
					append = true;
				}
			}
		}
		else
		{
			displayMessageTractorRear = false;
			buttonPressedTractorRear = false;
			append = false;
		}

		if (isPlayerCollidingWithTractorFront == true)
		{
			if (!displayMessageTractorFront)
			{
				if (isOtherVehicleAround == true)
				{
					VehicleBase::VehicleRayCastMessage("Tractor (1): Press T to attach rope in-front", ColorF(1,0,0), 10,10,5,3);
				}
				else
				{
					VehicleBase::VehicleRayCastMessage("ERROR: Other vehivle not close enough", ColorF(1,0,0), 10,80,5,3);
				}
				displayMessageTractorFront = true;
			}

			if (buttonPressedTractorFront == true && isOtherVehicleAround)
			{
				if(!appendTwice)
				{
					if (CTowingMechanism::Nodes.size() == 0)
					{
						if (!isAlreadyConnectedToRope)
						{
							CTowingMechanism::IsEntitesConnected = true;
							CTowingMechanism::Nodes.push_back(1);
							CTowingMechanism::GetNodeOne(GetEntity()->GetId());	
							isAlreadyConnectedToRope = true;
							VehicleBase::VehicleRayCastMessage("Tractor (1): Connected (1/2)", ColorF(1,0,0), 10,80,5,3);
						}
					}
					else if (CTowingMechanism::Nodes.size() == 1)
					{
						if (!isAlreadyConnectedToRope)
						{
							CTowingMechanism::GetNodeTwo(GetEntity()->GetId());
							CTowingMechanism::NodesManagement(1);
						}
						else
						{
							VehicleBase::VehicleRayCastMessage("ERROR: Can't connect to same vehicle", ColorF(1,0,0), 10,40,5,3);
						}
					}
					else if (CTowingMechanism::Nodes.size() == 2)
					{
						VehicleBase::VehicleRayCastMessage("Rope Disconnected", ColorF(1,0,0), 10,80,5,3);
						CTowingMechanism::Nodes.clear();
						CTowingMechanism::IsEntitesConnected = false;
						CTowingMechanism::NodesManagement(2);
						isAlreadyConnectedToRope = false;						
						CHumvee::isAlreadyConnectedToRope_Humvee = false;
						CTowingMechanism::EntityOneIsChild = false;
						CTowingMechanism::EntityTwoIsChild = false;
						appendTwice = false;
					}
					else {}
					appendTwice = true;
				}
			}
		}
		else
		{
			displayMessageTractorFront = false;
			buttonPressedTractorFront = false;
			appendTwice = false;
		}
	}
}

bool CTractor::OnInputEvent( const SInputEvent &event )
{
	if (event.keyId == eKI_T && event.state == eIS_Pressed && isPlayerCollidingWithTractorRear)
	{
		buttonPressedTractorRear = true;
	}

	if (event.keyId == eKI_T && event.state == eIS_Pressed && isPlayerCollidingWithTractorFront)
	{
		buttonPressedTractorFront = true;
	}

	return false;
}