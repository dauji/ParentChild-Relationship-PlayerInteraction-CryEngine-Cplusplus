#ifndef __MYTURRET_H__
#define __MYTURRET_H__

#include "Weapon.h"

class CMyTurret : public CGameObjectExtensionHelper<CMyTurret, IGameObjectExtension>
{
   public:

   CMyTurret() {}
   ~CMyTurret() {}

   virtual void GetMemoryUsage(ICrySizer *pSizer) const {}
   virtual bool Init( IGameObject * pGameObject );
   virtual void PostInit( IGameObject * pGameObject );
   virtual void InitClient(int channelId) {}
   virtual void PostInitClient(int channelId) {}
   virtual bool ReloadExtension( IGameObject * pGameObject, const SEntitySpawnParams &params ) {return true;}
   virtual void PostReloadExtension( IGameObject * pGameObject, const SEntitySpawnParams &params ) {}
   virtual bool GetEntityPoolSignature( TSerialize signature ) {return true;}
   virtual void Release() {}
   virtual void FullSerialize( TSerialize ser ) {}
   virtual bool NetSerialize( TSerialize ser, EEntityAspects aspect, uint8 profile, int pflags ) {return true;}
   virtual void PostSerialize() {}
   virtual void SerializeSpawnInfo( TSerialize ser ) {}
   virtual ISerializableInfoPtr GetSpawnInfo() {return 0;}
   virtual void Update( SEntityUpdateContext& ctx, int updateSlot );
   virtual void HandleEvent( const SGameObjectEvent& event ) {}
   virtual void ProcessEvent( SEntityEvent& event ) {}
   virtual void SetChannelId(uint16 id) {}
   virtual void SetAuthority( bool auth ) {}
   virtual void PostUpdate( float frameTime ) {}
   virtual void PostRemoteSpawn() {}
};

#endif