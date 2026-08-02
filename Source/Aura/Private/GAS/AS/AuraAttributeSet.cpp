// Copyright GYQ


#include "GAS/AS/AuraAttributeSet.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayEffectExtension.h"
#include "Engine/Engine.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"

UAuraAttributeSet::UAuraAttributeSet()
{
	// 初始化属性值

}

/*---------------------------------------------网络复制回调属性实现---------------------------------------------*/

void UAuraAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	// 此宏设置 属性 在网络上的复制行为：无条件复制 COND_None，每次值变化时都触发通知回调 REPNOTIFY_Always
	
	// vital Attributes:
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Mana, COND_None, REPNOTIFY_Always);
	// Primary Attributes:
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Strength, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Intelligence, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Resilience, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Vigor, COND_None, REPNOTIFY_Always);
	// Secondary Attributes:
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Armor, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, ArmorPenetration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, BlockChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, CriticalHitChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, CriticalHitDamage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, CriticalHitResistence, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, HealthRegeneration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, ManaRegeneration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);
}

/*---------------------------------------------网络复制回调函数实现---------------------------------------------*/

void UAuraAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth) const
{
	// 此宏用于在客户端接收到服务器复制的属性值后，通知 GameplayEffect 系统该属性已更新 
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Health, OldHealth);
}
void UAuraAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Mana, OldMana);
}
void UAuraAttributeSet::OnRep_Strength(const FGameplayAttributeData& OldStrength) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Strength, OldStrength);
}
void UAuraAttributeSet::OnRep_Intelligence(const FGameplayAttributeData& OldIntelligence) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Intelligence, OldIntelligence);
}
void UAuraAttributeSet::OnRep_Resilience(const FGameplayAttributeData& OldResilience) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Resilience, OldResilience);
}
void UAuraAttributeSet::OnRep_Vigor(const FGameplayAttributeData& OldVigor) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Vigor, OldVigor);
}
void UAuraAttributeSet::OnRep_Armor(const FGameplayAttributeData& OldArmor) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Armor, OldArmor);
}
void UAuraAttributeSet::OnRep_ArmorPenetration(const FGameplayAttributeData& OldArmorPenetration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, ArmorPenetration, OldArmorPenetration);
}
void UAuraAttributeSet::OnRep_BlockChance(const FGameplayAttributeData& OldBlockChance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, BlockChance, OldBlockChance);
}
void UAuraAttributeSet::OnRep_CriticalHitChance(const FGameplayAttributeData& OldCriticalHitChance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, CriticalHitChance, OldCriticalHitChance);
}
void UAuraAttributeSet::OnRep_CriticalHitDamage(const FGameplayAttributeData& OldCriticalHitDamage) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, CriticalHitDamage, OldCriticalHitDamage);
}
void UAuraAttributeSet::OnRep_CriticalHitResistence(const FGameplayAttributeData& OldCriticalHitResistence) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, CriticalHitResistence, OldCriticalHitResistence);
}
void UAuraAttributeSet::OnRep_HealthRegeneration(const FGameplayAttributeData& OldHealthRegeneration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, HealthRegeneration, OldHealthRegeneration);
}
void UAuraAttributeSet::OnRep_ManaRegeneration(const FGameplayAttributeData& OldManaRegeneration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, ManaRegeneration, OldManaRegeneration);
}
void UAuraAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, MaxHealth, OldMaxHealth);
}
void UAuraAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, MaxMana, OldMaxMana);
}

// 无论是对AS直接赋值，还是用GE修改AS，都会在属性变化前调用此函数(可用以限制【基础值】变化范围)
void UAuraAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	
	if (Attribute == GetHealthAttribute())
	{
		// 限制【基础值】范围
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
		
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				1,
				5.f,
				FColor::White,
				FString::Printf(TEXT("Health Changed: %f"), NewValue)
			);
		}
	}
	if (Attribute == GetMaxHealthAttribute())
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				2,
				5.f,
				FColor::White,
				FString::Printf(TEXT("MaxHealth Changed: %f"), NewValue)
			);
		}
	}
	if (Attribute == GetManaAttribute())
	{
		// 限制【基础值】范围
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxMana());
		
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				3,
				5.f,
				FColor::White,
				FString::Printf(TEXT("Mana Changed: %f"), NewValue)
			);
		}
	}
	if (Attribute == GetMaxManaAttribute())
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				4,
				5.f,
				FColor::White,
				FString::Printf(TEXT("MaxMana Changed: %f"), NewValue)
			);
		}
	}
}

// 属性值变化后调用
void UAuraAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	
	// Data 是 GAS 传入的完整回调上下文，包含了「哪个 GameplayEffect、修改了哪个属性、修改了多少数值、谁是施法者、谁是目标」等全部信息。
	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		// 确保应用 GE 后，Health属性值在有效范围内
		SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));
		
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(5, 5.f, FColor::Black,
				 FString::Printf(TEXT("Health from GetHealth(): %f"), GetHealth()));
			GEngine->AddOnScreenDebugMessage(6, 5.f, FColor::Black,
				 FString::Printf(TEXT("Health Magnitude: %f"), Data.EvaluatedData.Magnitude));
		}
	}
	if (Data.EvaluatedData.Attribute == GetManaAttribute())
	{
		// 确保应用 GE 后，Mana属性值在有效范围内
		SetMana(FMath::Clamp(GetMana(), 0.f, GetMaxMana()));
		
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(7, 5.f, FColor::Black,
				 FString::Printf(TEXT("Mana from GetMana(): %f"), GetMana()));
			GEngine->AddOnScreenDebugMessage(8,  5.f, FColor::Black,
				 FString::Printf(TEXT("Mana Magnitude: %f"), Data.EvaluatedData.Magnitude));
		}
	}

	// Props 用于存放提取完成的双方 Avatar、控制器、ASC、Character 等对象指针
	FEffectProperties Props;
	SetEffectProperties(Data, Props);
}

/**
 * 从 GameplayEffect 修改回调数据（Data）中，提取施法者（Source）与目标（Target）双方的完整上下文信息
 * 统一封装到 FEffectProperties 结构体中，避免在属性回调里重复编写大量重复的指针提取与空校验代码
 * 
 * @param Data   GAS 传入的 GE 修改回调 的完整上下文，包含施法者、目标、修改数值、GE 实例等全部信息
 * @param Props  输出参数，用于存放提取完成的双方 Avatar、控制器、ASC、Character 等对象指针
 * 
 * @note  本函数仅在服务端生效（PostGameplayEffectExecute 仅服务器触发）
 * @note  施法者控制器做了双重兜底获取：优先从 ASC 身份信息取，失败则从 Pawn 对象获取，兼容 ASC 挂载在 PlayerState/Character 等不同场景
 * @note  所有指针提取均做了多层空值校验，避免访问无效对象导致游戏崩溃
 */
void UAuraAttributeSet::SetEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& Props) const
{
	// 获取 GameplayEffect 的上下文
	Props.EffectContextHandle = Data.EffectSpec.GetContext();
	// 获取 施法者 上下文中的 ASC
	Props.SourceASC = Props.EffectContextHandle.GetOriginalInstigatorAbilitySystemComponent();
	// 判断 SourceASC 的 Actor 中的 Avatar 是否存在
	if (IsValid(Props.SourceASC) && Props.SourceASC->AbilityActorInfo.IsValid() && Props.SourceASC->AbilityActorInfo->AvatarActor.IsValid())
	{
		// 获取 施法者，AbilityActorInfo表示该ASC的归属者
		Props.SourceAvatarActor = Props.SourceASC->AbilityActorInfo->AvatarActor.Get();
		Props.SourceController = Props.SourceASC->AbilityActorInfo->PlayerController.Get();
		if (IsValid(Props.SourceAvatarActor) && IsValid(Props.SourceController))
		{ 
			// 获取 施法者 控制器
			if (const APawn* SourcePawn = Cast<APawn>(Props.SourceAvatarActor))
			{
				Props.SourceController = SourcePawn->GetController(); 
			}
		}
		// 获取 施法者 角色对象
		if (IsValid(Props.SourceController))
		{
			ACharacter* SourceCharacter = Cast<ACharacter>(Props.SourceController->GetPawn());
		}
			
	}
		
	//从回调上下文Data里层层校验有效性，安全取出目标方的核心对象（化身 Actor、控制器、角色对象、能力系统组件），
	//为后续业务逻辑（比如播放受击动画、触发 UI、施加 buff）做准备
	if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		Props.TargetAvatarActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
		Props.TargetController =  Data.Target.AbilityActorInfo->PlayerController.Get();
		Props.TargetCharacter = Cast<ACharacter>(Props.TargetAvatarActor);
		Props.TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Props.TargetAvatarActor);
	}
}
