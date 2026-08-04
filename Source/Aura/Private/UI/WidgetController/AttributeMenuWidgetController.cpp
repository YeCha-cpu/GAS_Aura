// Copyright GYQ

#include "UI/WidgetController/AttributeMenuWidgetController.h"
#include "GAS/AS/AuraAttributeSet.h"
#include "GAS/GT/AuraGameplayTags.h"

void UAttributeMenuWidgetController::BindCallbackToDependencies()
{
	UAuraAttributeSet* AS = CastChecked<UAuraAttributeSet>(AttributeSet);
	check(AttributesInfo);
	for (auto& Pair:AS->TagsToAttributes)
	{
		// 获取该属性在 ASC 中的变化委托，该委托会在属性变化后触发 BroadcastAttributeInfo
		/*
		 * GetGameplayAttributeValueChangeDelegate 返回什么？
		 * 它返回一个 FOnGameplayAttributeValueChange 多播委托。
		 * 这个委托存储在 UAbilitySystemComponent 内部的 FActiveGameplayEffect 管理器中。
		 * 当任何 GameplayEffect（GE）修改该属性时，ASC 会主动调用该委托的 Broadcast。
		 */
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Pair.Value()).
		
		/*
		 * Lambda 的捕获方式 [this, Pair]（按值捕获）：
		 * 这里非常重要！Pair 是一个局部引用变量。在 for 循环中，Pair 的内存地址每次迭代都在变。如果使用 [this, &Pair]（按引用捕获），
		 * 当 Lambda 在未来的某一帧被触发时，Pair 早已析构，导致野指针崩溃。
		 * 代码中使用了按值捕获，将 Pair 的副本（即 FGameplayTag 和函数指针）保存到了 Lambda 闭包中，确保了回调触发时的绝对安全。
		 */
		AddLambda([this, Pair](const FOnAttributeChangeData Data)
		{
			// 属性变化时触发
			BroadcastAttributeInfo( Pair.Key, Pair.Value());
		});
	}
}

// 时序：当 UI 界面（如属性面板）打开时，蓝图会调用此函数。它不依赖任何网络复制，直接从本地 AttributeSet 内存中读取当前数值。
void UAttributeMenuWidgetController::BroadcastInitialValues()
{
	// 获取属性集实例
	UAuraAttributeSet* AS = CastChecked<UAuraAttributeSet>(AttributeSet);
	// 确保数据资产（AttributesInfo）已被正确设置
	check(AttributesInfo);
	// 遍历属性集中建立的【标签 → 属性委托】映射表（TMap<FGameplayTag, FAttribute_Signature> TagsToAttributes;）
	// TagsToAttributes 是在 UAuraAttributeSet 构造函数中填充的 TMap，它将每个 GameplayTag（如 "Attributes.Primary.Strength"）映射到一个可返回 FGameplayAttribute 的委托。
	for (auto& Pair : AS->TagsToAttributes)
	{
		BroadcastAttributeInfo(Pair.Key, Pair.Value());
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
        
	// 3. 发射给蓝图 UI
	AttributeInfoDelegate.Broadcast(Info);
}


