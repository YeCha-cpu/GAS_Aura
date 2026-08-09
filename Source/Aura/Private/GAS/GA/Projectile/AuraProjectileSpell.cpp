// Copyright GYQ

#include "GAS/GA/Projectile/AuraProjectileSpell.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Actor/AuraProjectile.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "GAS/GT/AuraGameplayTags.h"
#include "Interface/CombatInterface.h"
#include "Kismet/KismetSystemLibrary.h"

void UAuraProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle,const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,const FGameplayEventData* TriggerEventData)
{
	// 调用父类激活逻辑（基础设置）
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	// 注意：此函数未在此处调用 SpawnProjectile，
	// 因为需要等待目标数据（由外部任务触发）
}

void UAuraProjectileSpell::SpawnProjectile(const FVector& ProjectileTargetLocation)
{
	// 检查是否在服务器运行（确保生成权威性）
	const bool bIsServer = GetAvatarActorFromActorInfo()->HasAuthority();
	if (!bIsServer) return;
	
	// 通过战斗接口获取武器/手掌等发射插槽的位置
	ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetAvatarActorFromActorInfo());
	if (CombatInterface)
	{
		const FVector SocketLocation = CombatInterface->GetCombatSocketLocation();
		FRotator Rotation = (ProjectileTargetLocation - SocketLocation).Rotation();
		Rotation.Pitch = 0.f;
		
		// 构造生成变换，目前只设置了位置（旋转待定）
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(SocketLocation);
		SpawnTransform.SetRotation(Rotation.Quaternion());
		
		// 使用延迟生成（SpawnActorDeferred）以便在 FinishSpawning 前设置属性
		AAuraProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(
			ProjectileClass, 
			SpawnTransform, 
			GetOwningActorFromActorInfo(), 
			Cast<APawn>(GetOwningActorFromActorInfo()), 
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		
		UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo());	// 获取能力系统组件
		FGameplayEffectSpecHandle GESpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), SourceASC->MakeEffectContext());	// 创建伤害效果规格句柄
		FAuraGameplayTags GT = FAuraGameplayTags::Get();	// 获取游戏玩法标签集合
		const float ScaleDamage = Damage.GetValueAtLevel(GetAbilityLevel());	// 根据当前能力等级获取伤害值
		Projectile->DamageEffectSpecHandle = GESpecHandle;	// 将 伤害效果规格句柄 设置给 发射物Actor的伤害效果规格句柄
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(GESpecHandle, GT.Effect_Damage, ScaleDamage);	// 由调用者（SetByCaller）设置伤害值
		
		// 完成生成（触发 BeginPlay 等初始化）
		Projectile->FinishSpawning(SpawnTransform);
	}
}