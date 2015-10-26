/*************************************************************************
Humvee.cpp
Started:
- 10:22:2015   16:01 : Created by Dauji Vaid
*************************************************************************/

#include "StdAfx.h"
#include "Humvee.h"

bool CHumvee::isAlreadyConnectedToRope_Humvee;
int CHumvee::HumveeID;

CHumvee::CHumvee()
{
	gEnv->pSystem->GetIInput()->AddEventListener(this);
}

CHumvee::~CHumvee()
{
	gEnv->pSystem->GetIInput()->RemoveEventListener(this);
}

bool CHumvee::Init( IGameObject * pGameObject )
{
	SetGameObject(pGameObject);
	GetEntity()->Activate(true);

	return true;
}

void CHumvee::PostInit( IGameObject * pGameObject )
{
	HumveeID = GetEntity()->GetId();
	// CryLogAlways("HUMVEE_ID: %d", HumveeID); //4
	pGameObject->EnableUpdateSlot(this, 0);
	GetEntity()->LoadGeometry(0, "Objects/Objects/vehicles/tractor/tractor.cgf");
	SEntityPhysicalizeParams PhysParams;
	PhysParams.mass = 30;
	PhysParams.type = PE_RIGID;
	GetEntity()->Physicalize(PhysParams);

	IMaterial* whiteMat = gEnv->p3DEngine->GetMaterialManager()->LoadMaterial("materials/material_default");
	GetEntity()->SetMaterial(whiteMat);
	tractor = g_pGame->GetIGameFramework()->GetGameObject(CTractor::TractorID);
	humvee = g_pGame->GetIGameFramework()->GetGameObject(HumveeID);
	displayMessageHumveeRear = false;
	displayMessageHumveeFront = false;
	buttonPressedHumveeRear = false;
	buttonPressedHumveeFront = false;
	append = false;
	appendTwice = false;
	isAlreadyConnectedToRope_Humvee = false;
}

void CHumvee::Update( SEntityUpdateContext& ctx, int updateSlot )
{
	if (gEnv->IsEditorGameMode())
	{
		isOtherVehicleAround = CTowingMechanism::IsVehiclesCloseBy();

		isPlayerCollidingWithHumveeFront = VehicleBase::VehicleFrontRayCast(humvee, isPlayerCollidingWithHumveeFront, 3);
		isPlayerCollidingWithHumveeRear = VehicleBase::VehicleRearRayCast(humvee, isPlayerCollidingWithHumveeRear, 3);
		if (isPlayerCollidingWithHumveeFront == true)
		{
			if (!displayMessageHumveeFront)
			{
				if(isOtherVehicleAround)
				{
					VehicleBase::VehicleRayCastMessage("Tractor (2): Press T to attach rope in-front", ColorF(1,0,0), 10,10,5,3);
				}
				else
				{
					VehicleBase::VehicleRayCastMessage("ERROR: Other vehicle not close enough", ColorF(1,0,0), 10,80,5,3);
				}
				displayMessageHumveeFront = true;
			}		
			if (buttonPressedHumveeFront == true && isOtherVehicleAround == true)
			{
				if(!appendTwice)
				{
					if (CTowingMechanism::Nodes.size() == 0)
					{
						if (!isAlreadyConnectedToRope_Humvee)
						{
							CTowingMechanism::IsEntitesConnected = true;
							CTowingMechanism::Nodes.insert(CTowingMechanism::Nodes.begin(), 1);
							CTowingMechanism::GetNodeOne(GetEntity()->GetId());
							isAlreadyConnectedToRope_Humvee = true;
							VehicleBase::VehicleRayCastMessage("Tractor (2): Connected (1/2)", ColorF(1,0,0), 10,80,5,3);
						}
					}
					else if (CTowingMechanism::Nodes.size() == 1)
					{
						if (!isAlreadyConnectedToRope_Humvee)
						{
							CTowingMechanism::GetNodeTwo(GetEntity()->GetId());

							CTowingMechanism::NodesManagement(1);

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
						isAlreadyConnectedToRope_Humvee = false;
						CTractor::isAlreadyConnectedToRope = false;
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
			displayMessageHumveeFront = false;
			buttonPressedHumveeFront = false;
			appendTwice = false;
		}

		if (isPlayerCollidingWithHumveeRear == true)
		{
			if (!displayMessageHumveeRear)
			{
				if(isOtherVehicleAround)
				{
					VehicleBase::VehicleRayCastMessage("Tractor (2): Press O to attach rope in back", ColorF(1,0,0), 10,10,5,3);
				}
				else
				{
					VehicleBase::VehicleRayCastMessage("ERROR: Other vehicle not close enough", ColorF(1,0,0), 10,80,5,3);
				}
				displayMessageHumveeRear = true;
			}

			if (buttonPressedHumveeRear == true && isOtherVehicleAround == true)
			{
				if(!append)
				{
					if (CTowingMechanism::Nodes.size() == 0)
					{
						if (!isAlreadyConnectedToRope_Humvee)
						{
							isAlreadyConnectedToRope_Humvee = true;
							CTowingMechanism::IsEntitesConnected = true;
							CTowingMechanism::Nodes.insert(CTowingMechanism::Nodes.begin(), 0);
							CTowingMechanism::GetNodeOne(GetEntity()->GetId());
							VehicleBase::VehicleRayCastMessage("Tractor (2): Connected (1/2)", ColorF(1,0,0), 10,80,5,3);
						}
					}
					else if (CTowingMechanism::Nodes.size() == 1)
					{
						if (!isAlreadyConnectedToRope_Humvee)
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
						isAlreadyConnectedToRope_Humvee = false;
						CTractor::isAlreadyConnectedToRope = false;
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
			displayMessageHumveeRear = false;
			buttonPressedHumveeRear = false;
			append = false;
		}
	}
}

bool CHumvee::OnInputEvent( const SInputEvent &pevent )
{
	if (pevent.keyId == eKI_T && pevent.state == eIS_Pressed && isPlayerCollidingWithHumveeRear)
	{
		buttonPressedHumveeRear = true;
	}

	if (pevent.keyId == eKI_T && pevent.state == eIS_Pressed && isPlayerCollidingWithHumveeFront)
	{
		buttonPressedHumveeFront = true;
	}

	return false;
}