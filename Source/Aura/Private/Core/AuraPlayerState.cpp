// Copyright GYQ


#include "Core/AuraPlayerState.h"
#include "GAS/AuraAbilitySystemComponent.h"
#include "GAS/AS/AuraAttributeSet.h"

AAuraPlayerState::AAuraPlayerState()
{
	NetUpdateFrequency = 100.f;
	
	// 创建技能系统组件
	ASC = CreateDefaultSubobject<UAuraAbilitySystemComponent>("AbilitySystemComponent");
	ASC->SetIsReplicated(true);// 确保与网络同步
	ASC->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);// 设置 玩家ASC 的复制模式为 Mixed
	
	// 创建属性集
	AS = CreateDefaultSubobject<UAuraAttributeSet>("Attributes");
}

UAbilitySystemComponent* AAuraPlayerState::GetAbilitySystemComponent() const
{
	return ASC;
}
