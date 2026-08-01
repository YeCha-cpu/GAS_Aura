// Copyright GYQ

#include "Core/AuraHUD.h"
#include "UI/Widget/AuraUserWidget.h"
#include "UI/WidgetController/OverlayWidgetController.h"

UOverlayWidgetController* AAuraHUD::GetOverlayWidgetController(const FWidgetControllerParams& WCParams)
{
	if (OverlayWidgetController == nullptr)
	{
		// 创建 OverlayWidgetController并设置参数
		OverlayWidgetController = NewObject<UOverlayWidgetController>(this, OverlayWidgetControllerclass);
		OverlayWidgetController->SetWidgetControllerParams(WCParams);
		
		// 为 OverlayWidgetController 绑定 AS属性 变化委托
		OverlayWidgetController->BindCallbackToDependencies();
		return OverlayWidgetController;
	}
	return OverlayWidgetController;
}

void AAuraHUD::InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS)
{
	checkf(OverlayWidgetClass, TEXT("OverlayWidgetClass 没初始化，请在 BP_AuraHUD 里设置"));
	checkf(OverlayWidgetControllerclass, TEXT("OverlayWidgetControllerclass 没初始化，请在 BP_AuraHUD 里设置"));
	
	// 创建 一个 UAuraUserWidget 类的实例给 OverlayWidget 
	UUserWidget* Widget = CreateWidget<UUserWidget>(GetWorld(), OverlayWidgetClass);
	OverlayWidget = Cast<UAuraUserWidget>(Widget);
	
	// 为 WidgetController 传递控制器参数
	const FWidgetControllerParams WidgetControllerParams(PC, PS, ASC, AS);
	UOverlayWidgetController* WidgetController = GetOverlayWidgetController(WidgetControllerParams);
	
	// 设置 OverlayWidget 的 WidgetController
	OverlayWidget->SetWidgetController(WidgetController);
	
	// 广播 AS属性 初始值
	WidgetController->BroadcastInitialValues();
	
	// 添加 OverlayWidget 到 Viewport
	OverlayWidget->AddToViewport();
	
}

