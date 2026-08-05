// Copyright GYQ


#include "GAS/AuraAbilitySystemComponent.h"
#include "Engine/Engine.h"
#include "GAS/GA/AuraGameplayAbility.h"
#include "GAS/GT/AuraGameplayTags.h"

void UAuraAbilitySystemComponent::AbilityActorInfoSet()
{
	// 绑定 应用GE后 触发的委托回调
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UAuraAbilitySystemComponent::EffectApplied);
	
	// 单例使用示例(MaxHealth): 利用 FAuraGameplayTags 单例获取刚刚注册的原生标签，并在屏幕左上角打印出来。 
	UE_LOG(LogTemp, Warning, TEXT("Effect Applied: %s"), *FAuraGameplayTags::Get().Attributes_Secondary_MaxHealth.GetTagName().ToString())
}

void UAuraAbilitySystemComponent::AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities)
{
	for (const TSubclassOf<UGameplayAbility> AbilityClass : StartupAbilities)
	{
		/** “2步走”创建并激活初始技能 */
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		
		// 在给角色赋予初始技能时，将技能自身配置的“启动输入标签”（StartupInputTag）添加到该技能实例的动态标签列表中，从而实现技能与输入系统的绑定
		if (const UAuraGameplayAbility* AuraAbility = Cast<UAuraGameplayAbility>(AbilitySpec.Ability))
		{
			AbilitySpec.DynamicAbilityTags.AddTag(AuraAbility->StartupInputTag);
			GiveAbility(AbilitySpec);
		}
		
	}
}

void UAuraAbilitySystemComponent::AbilityInputTagHeld(const FGameplayTag& InputTag)
{
	// 如果传入的输入标签非法无效，直接终止本次逻辑，不执行后续技能匹配
	if (!InputTag.IsValid()) return;

	// 遍历当前ASC组件挂载的所有已赋予、具备激活资格的游戏能力实例
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		// 获取技能实例的动态标签列表中是否包含传入的输入标签
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			// 为当前技能实例标记「输入已按下」状态
			// 技能GA内部可通过该状态区分按下、长按、松开三种输入阶段，适配蓄力、持续施法等逻辑
			AbilitySpecInputPressed(AbilitySpec);

			// 校验该技能当前并未处于运行激活状态
			if (!AbilitySpec.IsActive())
			{
				// 通过技能句柄调用ASC原生接口，尝试正式激活该GameplayAbility技能
				TryActivateAbility(AbilitySpec.Handle);
			}
		}
	}
}

void UAuraAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;

	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
		 	// 为当前技能实例标记「输入已释放」状态
		 	AbilitySpecInputReleased(AbilitySpec);
		}
	}
	
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
