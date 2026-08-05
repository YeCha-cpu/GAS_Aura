// Copyright GYQ

#include "UI/WidgetController/AttributeMenuWidgetController.h"
#include "GAS/AS/AuraAttributeSet.h"
#include "GAS/GT/AuraGameplayTags.h"

// 时序：当 UI 界面（如属性面板）打开时，【蓝图】会调用此函数。它不依赖任何网络复制，直接从本地 AttributeSet 内存中读取当前数值。
void UAttributeMenuWidgetController::BroadcastInitialValues()
{
	// 获取属性集实例
	UAuraAttributeSet* AS = CastChecked<UAuraAttributeSet>(AttributeSet);
	// 确保数据资产（AttributesInfo）已被正确设置
	check(AttributesInfo);
	// 遍历属性集中建立的【标签 → 属性委托】映射表
	for (auto& Pair : AS->TagsToAttributes)
	{
		// 更新属性值
		BroadcastAttributeInfo(Pair.Key, Pair.Value());
	}
}

void UAttributeMenuWidgetController::BindCallbackToDependencies()
{
	UAuraAttributeSet* AS = CastChecked<UAuraAttributeSet>(AttributeSet);
	check(AttributesInfo);
	for (auto& Pair:AS->TagsToAttributes)
	{
		// 通过获取 Pair.Value() 来获取该属性的 FGameplayAttribute，
		// GetGameplayAttributeValueChangeDelegate 获取该 FGameplayAttribute 返回一个 FOnGameplayAttributeValueChange 多播委托
		// 为 FOnGameplayAttributeValueChange 绑定 lambda delegate
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Pair.Value()).
		AddLambda([this, Pair](const FOnAttributeChangeData Data)// 在 for 循环中，Pair 的内存地址每次迭代都在变。如果使用 [this, &Pair]（按引用捕获），当 Lambda 在未来的某一帧被触发时，Pair 早已析构，导致野指针崩溃。
		{
			// 将 属性标签 和 属性的 FGameplayAttribute 一起广播给蓝图，做初始化
			BroadcastAttributeInfo( Pair.Key, Pair.Value());
		});
	}
}

void UAttributeMenuWidgetController::BroadcastAttributeInfo(const FGameplayTag& AttributeTag,
	const FGameplayAttribute& Attribute)
{
	// 1. 查数据资产（拿 UI 名称、描述）
	FAuraAttributeInfo Info = AttributesInfo->FindAttributeInfoByTag(AttributeTag);
        
	// 2 获取属性的实际数值
	// GetNumericValue 通过该结构体内存储的 Property 指针，从 AttributeSet 实例中读取 该属性的数值
	Info.AttributeValue = Attribute.GetNumericValue(AttributeSet);
        
	// 3. 发射给蓝图 UI（WBP_TextValueRow在蓝图只需获取控制器后绑定此委托，再赋值给 UI 即可 ）
	AttributeInfoDelegate.Broadcast(Info);
}


