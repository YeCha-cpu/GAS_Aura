// Copyright GYQ

#include "GAS/BluePrintFunctionLibrary/AuraGASLibrary.h"
#include "Core/AuraHUD.h"
#include "Core/AuraPlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "UI/WidgetController/OverlayWidgetController.h"

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
