// Copyright GYQ

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "UI/WidgetController/AttributeMenuWidgetController.h"
#include "AuraHUD.generated.h"

class UAttributeSet;
class UAbilitySystemComponent;
class UOverlayWidgetController;
class UAuraUserWidget;
struct FWidgetControllerParams;

UCLASS()
class AURA_API AAuraHUD : public AHUD
{
	GENERATED_BODY()
public:
	// 获取 OverlayWidget 的 Controller。若没有则根据 WCParams 参数来创建一个 OverlayWidgetController 并返回
	UOverlayWidgetController* GetOverlayWidgetController(const FWidgetControllerParams& WCParams);
	
	// 获取 AttributeMenuWidget 的 Controller。若没有则根据 WCParams 参数来创建一个 AttributeMenuWidgetController 并返回
	UAttributeMenuWidgetController* GetAttributeMenuWidgetController(const FWidgetControllerParams& WCParams);
	
	// 外部通过此函数来初始化 OverlayWidget（包括 OverlayWidget 实例本身以及 控制器参数）
	void InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS);
	
private:
	/** ======================================================== OverlayWidget 相关 ========================================================*/
	
	UPROPERTY()
	TObjectPtr<UAuraUserWidget> OverlayWidget; // 用于存储创建后的 Widget
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<UAuraUserWidget> OverlayWidgetClass; // 用于创建 Widget 时指定要创建的 Widget 类

	UPROPERTY()
	TObjectPtr<UOverlayWidgetController> OverlayWidgetController; // 用于存储创建后的控制器指针

	UPROPERTY(EditAnywhere)
	TSubclassOf<UOverlayWidgetController> OverlayWidgetControllerclass;	// 用于创建控制器时指定要创建的控制器类 
	
	/** ======================================================== AttributeMenuWidget 相关 ========================================================*/
	
	UPROPERTY()
	TObjectPtr<UAttributeMenuWidgetController> AttributeMenuWidgetController; // 用于存储创建后的控制器指针
	UPROPERTY(EditAnywhere)
	TSubclassOf<UAttributeMenuWidgetController> AttributeMenuWidgetControllerClass; // 用于创建控制器时指定要创建的控制器类 
};
