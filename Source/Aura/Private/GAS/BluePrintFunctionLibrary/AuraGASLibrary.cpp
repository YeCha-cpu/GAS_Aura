// Copyright GYQ

#include "GAS/BluePrintFunctionLibrary/AuraGASLibrary.h"

#include "Core/AuraGameModeBase.h"
#include "Core/AuraHUD.h"
#include "Core/AuraPlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "UI/WidgetController/AttributeMenuWidgetController.h"

class AAuraGameModeBase;

UOverlayWidgetController* UAuraGASLibrary::GetOverlayWidgetController(const UObject* WorldContextObject)
{
	/** 1、获取 FWidgetControllerParams 的 4个参数*/
	
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))	// 获取 PC
	{
		if (AAuraHUD* AuraHUD = Cast<AAuraHUD>(PC->GetHUD())) // 获取 HUD
		{
			AAuraPlayerState* PS = PC->GetPlayerState<AAuraPlayerState>(); // 获取 PS
			
			// 获取 ASC 与 AS
			// 标准 GAS 架构的约定：PlayerState 持有 ASC 和 AS。
			UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
			UAttributeSet* AS = PS->GetAttributeSet();
			
			/** 创建 FWidgetControllerParams，用于统一传递 UI 控制器所需的 4 个核心依赖对象。 */
			const FWidgetControllerParams WidgetControllerParams(PC, PS, ASC, AS);
			
			// 调用 HUD 的方法获取 OverlayWidgetController，如果没有，则会创建一个
			return AuraHUD->GetOverlayWidgetController(WidgetControllerParams);
		}
	}
	return nullptr;
}

UAttributeMenuWidgetController* UAuraGASLibrary::GetAttributeMenuWidgetController(const UObject* WorldContextObject)
{
	// 1. 获取 PC
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		// 2. 获取 HUD
		if (AAuraHUD* AuraHUD = Cast<AAuraHUD>(PC->GetHUD()))
		{
			// 3. 获取 PS, ASC, AS（标准 GAS 架构：由 PS 持有）
			AAuraPlayerState* PS = PC->GetPlayerState<AAuraPlayerState>();
			UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
			UAttributeSet* AS = PS->GetAttributeSet();
            
			// 4. 组装参数传给 HUD
			const FWidgetControllerParams WidgetControllerParams(PC, PS, ASC, AS);
			return AuraHUD->GetAttributeMenuWidgetController(WidgetControllerParams);
		}
	}
	return nullptr;
}

// Aura项目GAS工具类：为ASC初始化角色对应职业的三类基础属性（主属性、副属性、生命核心属性）
void UAuraGASLibrary::InitializeDefaultAttributes(const UObject* WorldContextObject, ECharacterClass CharacterClass, float Level, UAbilitySystemComponent* ASC)
{
	// 通过世界上下文获取当前GameMode，并强转为项目自定义的AuraGameMode基类
	AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	// 校验GameMode获取失败则直接退出函数，防止空指针访问崩溃
	if (AuraGameMode == nullptr) return;
	
	// 通过ASC获取角色角色
	AActor* AvatarActor = ASC->GetAvatarActor();

	// 从GameMode中拿到全局配置的角色职业信息数据资源
	UCharacterClassInfo* CharacterClassInfo = AuraGameMode->CharacterClassInfo;
	// 根据传入的职业枚举，取出该职业预设的基础属性配置结构体
	FCharacterClassDefaultInfo ClassDefaultInfo = CharacterClassInfo->GetClassDefaultInfo(CharacterClass);

	// ========== 第一步：4步走应用主属性（力量、智力、敏捷这类核心基础属性） ==========
	// 构建GAS的GE上下文句柄，创建一个空白的GameplayEffect上下文对象
	FGameplayEffectContextHandle PrimaryAttributesContextHandle = ASC->MakeEffectContext();
	PrimaryAttributesContextHandle.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle PrimaryAttributesSpecHandle = ASC->MakeOutgoingSpec(ClassDefaultInfo.PrimaryAttributes, Level, PrimaryAttributesContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*PrimaryAttributesSpecHandle.Data.Get());

	// ========== 第二步：4步走应用副属性（暴击率、暴击伤害、护甲穿透等衍生属性） ==========
	FGameplayEffectContextHandle SecondaryAttributesContextHandle = ASC->MakeEffectContext();
	SecondaryAttributesContextHandle.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle SecondaryAttributesSpecHandle = ASC->MakeOutgoingSpec(CharacterClassInfo->SecondaryAttributes, Level, SecondaryAttributesContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*SecondaryAttributesSpecHandle.Data.Get());

	// ========== 第三步：4步走应用生命类关键属性（生命值、法力值、体力这类生存核心属性） ==========
	FGameplayEffectContextHandle VitalAttributesContextHandle = ASC->MakeEffectContext();
	VitalAttributesContextHandle.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle VitalAttributesSpecHandle = ASC->MakeOutgoingSpec(CharacterClassInfo->VitalAttributes, Level, VitalAttributesContextHandle);ASC->ApplyGameplayEffectSpecToSelf(*VitalAttributesSpecHandle.Data.Get());
}

void UAuraGASLibrary::GiveStartAbilities(const UObject* WorldContextObject, UAbilitySystemComponent* ASC)
{
	// 从上下文获取当前GameMode并强转为项目自定义Aura游戏模式
	AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if (AuraGameMode == nullptr) return;

	// 获取GameMode中存储的角色职业配置数据资产
	UCharacterClassInfo* CharacterClassInfo = AuraGameMode->CharacterClassInfo;

	// 遍历通用基础技能数组
	for (TSubclassOf<UGameplayAbility> AbilityClass : CharacterClassInfo->CommonAbilities)
	{
		// 构建技能实例规格，初始等级设置为1级
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		// 将技能赋予传入的目标ASC组件
		ASC->GiveAbility(AbilitySpec);
	}
}

