// Copyright GYQ

#pragma once

#include "CoreMinimal.h"
#include "PersonaAssetEditorToolkit.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "AbilitySystemComponent.h"
#include "OverlayWidgetController.generated.h"

class UAuraUserWidget;

USTRUCT(BlueprintType)
struct FUIWidgetRow : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag MessageTag = FGameplayTag();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText MessageText = FText();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UAuraUserWidget> MessageWidget;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UTexture2D* Image =  nullptr;
};

// 用于广播技能系统属性变化
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttributeChanged_Signature, float, NewValue);
// 用于广播数据表行
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMessageWidgetRow_Signature, FUIWidgetRow, Row);

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class AURA_API UOverlayWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()
	
public:
	// 用于广播属性初始值
	virtual void BroadcastInitialValues() override;
	// 绑定 AS属性 变化委托
	virtual void BindCallbackToDependencies() override;
	
	// 创建技能系统属性变化委托实例：
	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes") //蓝图可赋值
	FOnAttributeChanged_Signature OnHealthChanged;
	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnAttributeChanged_Signature OnMaxHealthChanged;
	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnAttributeChanged_Signature OnManaChanged;
	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnAttributeChanged_Signature OnMaxManaChanged;
	
	// 创建数据表行委托实例：
	UPROPERTY(BlueprintAssignable, Category = "GAS|Messages")
	FMessageWidgetRow_Signature MessageWidgetRowDelegate;
	
protected:
	// 数据表
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget Data")
	TObjectPtr<UDataTable> MessageWidgetDataTable;
	
	// 获取数据表行
	template<typename T>
	T* GetDataTableRowByTag(UDataTable* DataTable, const FGameplayTag& Tag);

};

// 模板函数的定义一般放在头文件中 
template <typename T>
T* UOverlayWidgetController::GetDataTableRowByTag(UDataTable* DataTable, const FGameplayTag& Tag)
{
	// 因为 标签名 与 数据行名 保持一致，所以可以用标签名来查找行
	T* Row = DataTable->FindRow<T>(Tag.GetTagName(), TEXT(""));
	return Row;
}
