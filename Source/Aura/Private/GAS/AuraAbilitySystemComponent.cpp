// Copyright GYQ


#include "GAS/AuraAbilitySystemComponent.h"
#include "Engine/Engine.h"
#include "GAS/GT/AuraGameplayTags.h"

void UAuraAbilitySystemComponent::AbilityActorInfoSet()
{
	// 绑定 应用GE后 触发的委托回调
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UAuraAbilitySystemComponent::EffectApplied);
	
	// 单例使用示例(MaxHealth): 利用 FAuraGameplayTags 单例获取刚刚注册的原生标签，并在屏幕左上角打印出来。 
	UE_LOG( LogTemp, Warning, TEXT("Effect Applied: %s"), *FAuraGameplayTags::Get().Attributes_Secondary_MaxHealth.GetTagName().ToString())
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
