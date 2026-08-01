// Copyright GYQ

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AuraAbilitySystemComponent.generated.h"

// 用于 监听 GE 应用时，获取 GE 资产标签
DECLARE_MULTICAST_DELEGATE_OneParam(FEffectAssetTags, const FGameplayTagContainer& /* AssetTags */)

UCLASS()
class AURA_API UAuraAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
public:
	// 外部可调用这个函数来绑定 应用GE后 触动的委托回调
	void AbilityActorInfoSet();
	
	// 监听 GE 应用时，获取 GE 资产标签
	FEffectAssetTags EffectAssetTags;
	
protected:
	// 回调函数
	void EffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle);
	
};
