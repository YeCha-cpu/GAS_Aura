// Copyright GYQ

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "AuraWidgetController.generated.h"

class APlayerController;
class APlayerState;
class UAbilitySystemComponent;
class UAttributeSet;

// 封装了 WidgetController 所需要的参数
USTRUCT(BlueprintType)
struct FWidgetControllerParams
{
	GENERATED_BODY()
	
	FWidgetControllerParams() {}
	FWidgetControllerParams(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS) : 
	PlayerController(PC), PlayerState(PS), AbilitySystemComponent(ASC), AttributeSet(AS) {}
	
	/** 提供四个核心指针：PlayerController、PlayerState、AbilitySystemComponent、AttributeSet。*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<APlayerController> PlayerController = nullptr;// 玩家控制器
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<APlayerState> PlayerState = nullptr;// 玩家状态
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent = nullptr;// 能力系统组件
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAttributeSet> AttributeSet = nullptr;// 属性集
};

UCLASS()
class AURA_API UAuraWidgetController : public UObject
{
	GENERATED_BODY()
public:
	// 提供 SetWidgetControllerParams 统一设置参数
	UFUNCTION(BlueprintCallable)
	void SetWidgetControllerParams(const FWidgetControllerParams& InitParams);
	
	/** 声明虚函数 BroadcastInitialValues() 和 BindCallbackToDependencies()，供子类重写 */
	UFUNCTION(BlueprintCallable)
	virtual void BroadcastInitialValues();
	UFUNCTION(BlueprintCallable)
	virtual void BindCallbackToDependencies();

protected:
	// 这四个属性均为只读且可在蓝图中访问，用于 UI Widget 与 游戏核心系统 的交互。
	UPROPERTY(BlueprintReadOnly,Category = "WidgetPlayerController")
	TObjectPtr<APlayerController> PlayerController;
	
	UPROPERTY(BlueprintReadOnly,Category = "WidgetPlayerState")
	TObjectPtr<APlayerState> PlayerState;
	
	UPROPERTY(BlueprintReadOnly,Category = "WidgetASC")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	
	UPROPERTY(BlueprintReadOnly,Category = "WidgetAS")
	TObjectPtr<UAttributeSet> AttributeSet;
	
};
