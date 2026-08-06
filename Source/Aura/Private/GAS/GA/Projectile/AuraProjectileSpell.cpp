// Copyright GYQ


#include "GAS/GA/Projectile/AuraProjectileSpell.h"

#include "Actor/AuraProjectile.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "Interface/CombatInterface.h"
#include "Kismet/KismetStringLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

void UAuraProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                           const FGameplayAbilityActorInfo* ActorInfo, 
                                           const FGameplayAbilityActivationInfo ActivationInfo,
                                           const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	const bool bIsServer = HasAuthority(&ActivationInfo);
	if (!bIsServer) return;
	
	// 通过角色继承的战斗接口，来获取武器尖端的插槽位置，进而在插槽处创建发射物 
	ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetAvatarActorFromActorInfo());
	if (CombatInterface)
	{
		const FVector SocketLocation = CombatInterface->GetCombatSocketLocation();
		
		// 创建发射物
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(SocketLocation);
		//TODO: 还需设置旋转
		
		AAuraProjectile*  Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(
			ProjectileClass, 
			SpawnTransform, 
			GetOwningActorFromActorInfo(), 
			Cast<APawn>(GetOwningActorFromActorInfo()), 
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		
		//TODO：给予这个发射物一个能造成伤害的 Gameplay effect spec
		
		Projectile->FinishSpawning(SpawnTransform);
	}
	

	
}
