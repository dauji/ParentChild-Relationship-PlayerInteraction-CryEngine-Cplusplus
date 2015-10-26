#include "StdAfx.h"
#include "MyTurret.h"
#include "Projectile.h"
#include "WeaponSystem.h"
#include "IVehicleSystem.h"
#include "Actor.h"

bool CMyTurret::Init(IGameObject * pGameObject)
{
   SetGameObject(pGameObject);
   return true;
}

void CMyTurret::PostInit(IGameObject * pGameObject)
{
   pGameObject->EnableUpdateSlot(this, 0);   
   //This is the geometry that will represent your turret. Using the Mounted Gun here.
   //Good idea to scale it up in the editor so you can see it better
   GetEntity()->LoadGeometry(0, "Objects/weapons/mounted_gun/gun_mounted_tp.cgf");
   //The next two lines ensure that the rocket trail effect is cached and available
 //  CItemParticleEffectCache& particleCache = g_pGame->GetGameSharedParametersStorage()->GetItemResourceCache().GetParticleEffectCache();
 //  particleCache.CacheParticle("weapon_fx.rocketlauncher.tracer.tracer");
}

void CMyTurret::Update(SEntityUpdateContext& ctx, int slot)
{
   IEntity *turret = GetEntity();

   //If in the editor, return
   if(!gEnv->IsEditorGameMode())
      return;

   //If not in the editor (ie if in the game), do the following
   gEnv->pLog->Log("Updating now");

   Vec3 vPlayerPos;

   //Next lines are to get the player's position
   //That will be the target to aim for
   CActor *pClientActor=0;
   pClientActor=static_cast<CActor *>(g_pGame->GetIGameFramework()->GetClientActor());
   //First check if in a vehicle
   if (IVehicle* pVehicle = pClientActor->GetLinkedVehicle())
   {
      vPlayerPos = pVehicle->GetEntity()->GetPos();
   } else {//If not in a vehicle
      IEntity * pPlayer = g_pGame->GetIGameFramework()->GetClientActor()->GetEntity();
      vPlayerPos = pPlayer->GetPos();
   }
   vPlayerPos += Vec3(0,0,2);
   Vec3 vTurretPos = turret->GetPos();
   Vec3 vForwardDir = vPlayerPos - vTurretPos;

   vForwardDir.Normalize();

   //rotate the turret slowly towards the target
   Quat originalRot = turret->GetRotation();
   Quat rot = originalRot;
   rot.SetRotationVDir(vForwardDir);
   Quat targetRot = originalRot + (rot - originalRot)/10.0F;
   turret->SetRotation(targetRot);

   //Only fire every now and then, not every frame
   if (cry_frand() * 500.0f < 10.0f) {
      //Create a rocket and launch it at the target
      CProjectile *pRocket;
      IEntityClass *pClass = gEnv->pEntitySystem->GetClassRegistry()->FindClass("Rocket");
      if(pClass) {
         pRocket=g_pGame->GetWeaponSystem()->SpawnAmmo(pClass);
         pRocket->Launch(vTurretPos, vForwardDir ,vForwardDir, 0.1f);
      }
   }
}