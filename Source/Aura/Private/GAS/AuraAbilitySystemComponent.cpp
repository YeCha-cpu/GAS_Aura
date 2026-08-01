// Copyright GYQ


#include "GAS/AuraAbilitySystemComponent.h"
#include "Engine/Engine.h"

void UAuraAbilitySystemComponent::AbilityActorInfoSet()
{
	// 绑定 应用GE后 触发的委托回调
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UAuraAbilitySystemComponent::EffectApplied);
}

// GE应用时调用
void UAuraAbilitySystemComponent::EffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle)
{
	GEngine->AddOnScreenDebugMessage(
		9,
		5.f,
		FColor::Yellow,
		FString::Printf(TEXT("Effect Applied: %s"), *EffectSpec.Def->GetFName().ToString())
	);
	
	// 获取所有【资产标签】以广播
	FGameplayTagContainer TagContainer;
	EffectSpec.GetAllAssetTags(TagContainer);
	
	// 广播 资产标签
	EffectAssetTags.Broadcast(TagContainer);
	
	
}
