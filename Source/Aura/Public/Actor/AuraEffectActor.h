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
	
	// 重叠时
	UFUNCTION(BlueprintCallable)
	void OnOverlap(AActor* TargetActor);
	UFUNCTION(BlueprintCallable)
	void OnEndOverlap(AActor* TargetActor);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied GE")
	bool bDestoryOnEffectRemoval = false;
	
	// 三种类型的 GameplayEffect类（即时、持续、无限）
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied GE|类型")
	TSubclassOf<UGameplayEffect> InstantGameplayEffectClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied GE|类型")
	TSubclassOf<UGameplayEffect> DurationGameplayEffectClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied GE|类型")
	TSubclassOf<UGameplayEffect> InfiniteGameplayEffectClass;
	
	// 三种类型的 GameplayEffect对应的 应用策略枚举（默认不应用）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Applied GE|应用策略")
	EEffectApplcationPolicy InstantGameplayEffectPolicy = EEffectApplcationPolicy::DoNotApply;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Applied GE|应用策略")
	EEffectApplcationPolicy DurationGameplayEffectPolicy = EEffectApplcationPolicy::DoNotApply;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Applied GE|应用策略")
	EEffectApplcationPolicy InfiniteGameplayEffectPolicy = EEffectApplcationPolicy::DoNotApply;
	
	// 无限 GameplayEffect类型 对应的 移除策略枚举（默认结束重叠时移除）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Applied GE|移除策略")
	EEffectRemovalPolicy InfinitGameplayEffectRemovalPolicy = EEffectRemovalPolicy::RemoveOnEndOverlap;
	
	// 激活的 GameplayEffect Map，键是激活 GameplayEffect 句柄，值是技能系统组件
	TMap<FActiveGameplayEffectHandle, UAbilitySystemComponent*> ActiveEffectHandles;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Applied GE|角色等级")
	float ActorLevel = 1.f;
	
private:	


};
