/*************************************************************************
VehicleBase.h
Started:
- 10:22:2015   16:01 : Created by Dauji Vaid
*************************************************************************/

#ifndef _VEHICLEBASE_
#define _VEHICLEBASE_

#include "IGameObject.h"
#include "Player.h"

class VehicleBase
{
protected:
	VehicleBase() {};
	virtual ~VehicleBase() {};
	protected:
	bool activateMessage;
	bool VehicleRearRayCast(IGameObject* vehicle, bool isPlayerCollidingWithVehicle, int rayLength);
	bool VehicleFrontRayCast(IGameObject* vehicle, bool isPlayerCollidingWithVehicle, int rayLength);
	void VehicleRayCastMessage(const char* messageText, ColorF col, float XPos, float yPos, float size, float timeOut);
};

#endif 