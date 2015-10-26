/*************************************************************************
Humvee.h
Started:
- 10:22:2015   16:01 : Created by Dauji Vaid
*************************************************************************/

#ifndef _Humvee_
#define _Humvee_

#include "IGameObject.h"
#include "IEntity.h"
#include "VehicleBase.h"
#include "Player.h"
#include "IInput.h"
#include "TowingMechanism.h"
#include "Tractor.h"


struct CHumvee : public CGameObjectExtensionHelper<CHumvee, IGameObjectExtension>, public VehicleBase, public IInputEventListener
{
	CHumvee();
	~CHumvee();

	virtual void GetMemoryUsage(ICrySizer *pSizer) const {}
	virtual bool Init( IGameObject * pGameObject );
	virtual void PostInit( IGameObject * pGameObject );
	virtual void InitClient(int channelId){}
	virtual void PostInitClient(int channelId) {}
	virtual bool ReloadExtension( IGameObject * pGameObject, const SEntitySpawnParams &params ) {return true;}	
	virtual void PostReloadExtension( IGameObject * pGameObject, const SEntitySpawnParams &params ) {}
	virtual bool GetEntityPoolSignature( TSerialize signature ) {return true;}	
	virtual void Release() {};
	virtual void FullSerialize( TSerialize ser ) {}
	virtual bool NetSerialize( TSerialize ser, EEntityAspects aspect, uint8 profile, int pflags ) {return true;}	
	virtual void PostSerialize() {}
	virtual void SerializeSpawnInfo( TSerialize ser ) {}
	virtual ISerializableInfoPtr GetSpawnInfo() {return true;}	
	virtual void Update( SEntityUpdateContext& ctx, int updateSlot );
	virtual void HandleEvent( const SGameObjectEvent& event ) {}	
	virtual void ProcessEvent( SEntityEvent& event ) {}	
	virtual void SetChannelId(uint16 id) {}
	virtual void SetAuthority( bool auth ) {}
	virtual void PostUpdate( float frameTime ) {}
	virtual void PostRemoteSpawn(){}
	virtual bool OnInputEvent( const SInputEvent &event );
	virtual bool OnInputEventUI( const SInputEvent &event ) {	return false;	}
	virtual int GetPriority() const { return 0; }
	static bool isAlreadyConnectedToRope_Humvee;

	static int HumveeID;

private:
	IGameObject* tractor;
	IGameObject* humvee;
	bool displayMessageHumveeRear;
	bool displayMessageHumveeFront;
	bool isPlayerCollidingWithHumveeRear;
	bool isPlayerCollidingWithHumveeFront;
	bool append;
	bool appendTwice;
	bool buttonPressedHumveeRear;
	bool buttonPressedHumveeFront;
	bool isOtherVehicleAround;
};

#endif