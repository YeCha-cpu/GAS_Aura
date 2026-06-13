// Copyright GYQ

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayEffectTypes.h"
#include "AuraEffectActor.generated.h"

class UGameplayEffect;
class UAbilitySystemComponent;

// 应用效果的策略(重叠开始或结束时应用)
UENUM(BlueprintType)
enum class EEffectApplcationPolicy : uint8
{
	ApplyOnOverlap,
	ApplyOnEndOverlap,
	DoNotApply
};

// 移除效果的策略
UENUM(BlueprintType)
enum class EEffectRemovalPolicy : uint8
{
	RemoveOnEndOverlap,
	DoNotRemove
};

UCLASS()
class AURA_API AAuraEffectActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AAuraEffectActor();

protected:
	virtual void BeginPlay() override;

	// 应用 GE 的函数
	UFUNCTION(BlueprintCallable)
	void ApplyEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> GameplayEffectClass);
	
	// 重叠
	UFUNCTION(BlueprintCallable)
	void OnOverlap(AActor* TargetActor);
	UFUNCTION(BlueprintCallable)
	void OnEndOverlap(AActor* TargetActor);
	
	bool bDestoryOnEffectRemoval = false;
	
	// 三种类型的 GameplayEffect（即时、持续、无限）
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied GE")
	TSubclassOf<UGameplayEffect> InstantGameplayEffectClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied GE")
	TSubclassOf<UGameplayEffect> DurationGameplayEffectClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied GE")
	TSubclassOf<UGameplayEffect> InfiniteGameplayEffectClass;
	
	// 三种类型的 GameplayEffect对应的应用策略（默认不应用）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Applied GE|应用策略")
	EEffectApplcationPolicy InstantGameplayEffectPolicy = EEffectApplcationPolicy::DoNotApply;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Applied GE|应用策略")
	EEffectApplcationPolicy DurationGameplayEffectPolicy = EEffectApplcationPolicy::DoNotApply;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Applied GE|应用策略")
	EEffectApplcationPolicy InfiniteGameplayEffectPolicy = EEffectApplcationPolicy::DoNotApply;
	
	// 无限 GameplayEffect对应的移除策略
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Applied GE|移除策略")
	EEffectRemovalPolicy InfinitGameplayEffectRemovalPolicy = EEffectRemovalPolicy::RemoveOnEndOverlap;
	
	// 激活的 GameplayEffect
	TMap<FActiveGameplayEffectHandle, UAbilitySystemComponent*> ActiveEffectHandles;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Applied GE|角色等级")
	float ActorLevel = 1.f;
	
private:	


};
