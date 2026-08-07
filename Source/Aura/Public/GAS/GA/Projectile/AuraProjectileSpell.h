// Copyright GYQ

#pragma once

#include "CoreMinimal.h"
#include "GAS/GA/AuraGameplayAbility.h"
#include "AuraProjectileSpell.generated.h"

class AAuraProjectile;

/**
 * 投射物技能类
 * 功能：在服务器端生成一个投射物 Actor
 * 注：目标数据由外部（如 AbilityTask）提供，通过蓝图调用 SpawnProjectile
 */
UCLASS()
class AURA_API UAuraProjectileSpell : public UAuraGameplayAbility
{
	GENERATED_BODY()
	
protected:
	/** 技能激活入口（由 GAS 自动调用） */
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	/** 
	 * 生成投射物（蓝图可调用）
	 * 注意：仅在服务器执行（函数内部做了权限检查）
	 * 需要在蓝图/任务中调用，通常绑定到 TargetDataUnderMouse 的 ValidData 委托
	 */
	UFUNCTION(BlueprintCallable, Category = "Projectile")
	void SpawnProjectile(const FVector& ProjectileTargetLocation);
	
	/** 要生成的投射物类型（在蓝图子类或实例中设置） */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AAuraProjectile> ProjectileClass;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> DamageEffectClass;
};