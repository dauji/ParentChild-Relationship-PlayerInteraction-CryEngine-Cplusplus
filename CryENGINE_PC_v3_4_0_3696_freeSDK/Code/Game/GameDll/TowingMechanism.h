/*************************************************************************
TowingMechanism.h
Started:
- 10:22:2015   16:01 : Created by Dauji Vaid
*************************************************************************/

#ifndef _TOWINGMECHANISM_
#define _TOWINGMECHANISM_

#include "IGameObject.h"
#include <vector>
#include <list>
#include "Player.h"
#include "Tractor.h"
#include "Humvee.h"

class CTowingMechanism : public CGameObjectExtensionHelper<CTowingMechanism, IGameObjectExtension>
{
	public:
	CTowingMechanism() {};
	~CTowingMechanism() {};
	
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

	static std::vector<int> Nodes;
	static void NodesManagement(int nodeStateProvided);
	static std::list<int> nodeConnectors;
	static int pEntityOneID;
	static int pEntityTwoID;
	static IGameObject* EntityOne;
	static IGameObject* EntityTwo;
	static void GetNodeOne(int EntityOneID);
	static void GetNodeTwo(int EntityTwoID);
	static bool EntityOneIsChild;
	static bool EntityTwoIsChild;
	static bool IsEntitesConnected;
	static bool IsVehiclesCloseBy();
	static void ActivateTowing();
	static void DeactivateTowing();
	static IGameObject* humvee;
	static IGameObject* tractor;

private:
	static void DisplayMessage(const char* messageText, ColorF col, float XPos, float yPos, float size, float timeOut);

};



#endif 