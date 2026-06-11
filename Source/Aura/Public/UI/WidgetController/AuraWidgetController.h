// Copyright GYQ

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "AuraWidgetController.generated.h"

class APlayerController;
class APlayerState;
class UAbilitySystemComponent;
class UAttributeSet;

UCLASS()
class AURA_API UAuraWidgetController : public UObject
{
	GENERATED_BODY()
	
protected:
	// 这四个属性均为只读且可在蓝图中访问，用于 UI Widget 与 游戏核心系统 的交互。
	UPROPERTY(BlueprintReadOnly,Category = "WidgetPlayerController")
	TObjectPtr<APlayerController> PlayerController;
	
	UPROPERTY(BlueprintReadOnly,Category = "WidgetPlayerState")
	TObjectPtr<APlayerState> PlayerState;
	
	UPROPERTY(BlueprintReadOnly,Category = "WidgetASC")
	TObjectPtr<UAbilitySystemComponent> ASC;
	
	UPROPERTY(BlueprintReadOnly,Category = "WidgetAS")
	TObjectPtr<UAttributeSet> AS;
	
};
