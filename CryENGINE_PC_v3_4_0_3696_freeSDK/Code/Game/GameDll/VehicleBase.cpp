/*************************************************************************
VehicleBase.cpp
Started:
- 10:22:2015   16:01 : Created by Dauji Vaid
*************************************************************************/

#include "StdAfx.h"
#include "VehicleBase.h"

bool VehicleBase::VehicleRearRayCast(IGameObject* vehicle, bool isPlayerCollidingWithVehicle, int rayLength)
{
	Vec3 vehicleFWDdir = vehicle->GetEntity()->GetForwardDir();
	Vec3 vehiclePos = vehicle->GetEntity()->GetPos();
	Vec3 direction = Vec3 (0,-4,0);
	IPhysicalEntity *pSkipSelf = vehicle->GetEntity()->GetPhysics();
	ray_hit rayhit;

	if (gEnv->pPhysicalWorld->RayWorldIntersection(vehiclePos, (-vehicleFWDdir) * rayLength, ent_all, rwi_stop_at_pierceable|rwi_colltype_any, &rayhit, 1, &pSkipSelf, 1))
	{
		isPlayerCollidingWithVehicle = true;
		return true;
	}
	else
	{
		isPlayerCollidingWithVehicle = false;
		return false;
	}
	return false;
}

bool VehicleBase::VehicleFrontRayCast(IGameObject* vehicle, bool isPlayerCollidingWithVehicle, int rayLength)
{
	Vec3 vehicleFWDdir = vehicle->GetEntity()->GetForwardDir();
	Vec3 vehiclePos = vehicle->GetEntity()->GetPos();
	Vec3 direction = Vec3 (0,-4,0);
	IPhysicalEntity *pSkipSelf = vehicle->GetEntity()->GetPhysics();
	ray_hit rayhit;

	if (gEnv->pPhysicalWorld->RayWorldIntersection(vehiclePos, (vehicleFWDdir) * rayLength, ent_all, rwi_stop_at_pierceable|rwi_colltype_any, &rayhit, 1, &pSkipSelf, 1))
	{
		isPlayerCollidingWithVehicle = true;
		return true;
	}
	else
	{
		isPlayerCollidingWithVehicle = false;
		return false;
	}
	return false;
}

void VehicleBase::VehicleRayCastMessage(const char* messageText, ColorF col, float XPos, float yPos, float size, float timeOut)
{
	g_pGame->GetIGameFramework()->GetIPersistantDebug()->Begin("TestAddPersistentText2D", false);
	g_pGame->GetIGameFramework()->GetIPersistantDebug()->AddText(XPos, yPos, size, col, timeOut, messageText);
}