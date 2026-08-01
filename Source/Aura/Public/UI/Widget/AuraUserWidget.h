// Copyright GYQ

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AuraUserWidget.generated.h"


UCLASS()
class AURA_API UAuraUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	// 外部调用此函数来设置 WidgetController 以及 蓝图相关逻辑
	UFUNCTION(BlueprintCallable)
	void SetWidgetController(UObject* InWidgetController);
	
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UObject> WidgetController;
	
protected:
	// 蓝图实现事件
	UFUNCTION(BlueprintImplementableEvent)
	void WidgetControllerSet();
	
};
