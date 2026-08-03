// Copyright GYQ

#include "UI/WidgetController/OverlayWidgetController.h"
#include "GAS/AuraAbilitySystemComponent.h"
#include "GAS/AS/AuraAttributeSet.h"

// 会先把当前的属性初始值发送给 UI（比如进入游戏时先设置好血条的最大值和当前值）
void UOverlayWidgetController::BroadcastInitialValues()
{
	const UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);
	
	OnHealthChanged.Broadcast(AuraAttributeSet->GetHealth());
	OnMaxHealthChanged.Broadcast(AuraAttributeSet->GetMaxHealth());
	OnManaChanged.Broadcast(AuraAttributeSet->GetMana());
	OnMaxManaChanged.Broadcast(AuraAttributeSet->GetMaxMana());
	
}

// 负责注册后续的变化监听
void UOverlayWidgetController::BindCallbackToDependencies()
{
	const UAuraAttributeSet* AuraAttributeSet = Cast<UAuraAttributeSet>(AttributeSet);
	
	/**
	 * 借助 AddLambda 和 ASC 的委托，当属性值被修改（通常通过 Gameplay Effect）时，对应的 Lambda 自动执行，并把新值广播给 UI。
	 * 1、拿到监听该属性变化的委托实例：
	 * 从 ASC 中获取“该属性变化时”的委托，这个委托是 FOnGameplayAttributeValueChange。
	 * 拿到委托之后就可以进行绑定了
	 * 2、用 AddLambda 进行绑定。可以省去 创建回调函数 与 绑定回调函数 的操作（AddUObject）： 
	 * 接收参数 const FOnAttributeChangeData Data，这个结构体里包含了变化前后的值（OldValue、NewValue）。
	 * 调用 OnHealthChanged.Broadcast(Data.NewValue)，把变化后的新值广播给绑定了 OnHealthChanged 的 UI 部件（比如血条）。
	 */
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetHealthAttribute()).
		AddLambda([this](const FOnAttributeChangeData Data)
		{
			OnHealthChanged.Broadcast(Data.NewValue);
		});
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetMaxHealthAttribute()).
		AddLambda([this](const FOnAttributeChangeData Data)
		{
			OnMaxHealthChanged.Broadcast(Data.NewValue);
		});
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetManaAttribute()).
		AddLambda([this](const FOnAttributeChangeData Data)
		{
			OnManaChanged.Broadcast(Data.NewValue);
		});
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetMaxManaAttribute()).
		AddLambda([this](const FOnAttributeChangeData Data)
		{
			OnMaxManaChanged.Broadcast(Data.NewValue);
		});
	
	// 接受GE应用时的广播，监听效果资产标签，回调为 Lambda 函数
	Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent)->EffectAssetTags.AddLambda(
	[this](const FGameplayTagContainer& AssetTags)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Lambda CALLED"));
		
		// 遍历所有资产标签
		for (const FGameplayTag& Tag : AssetTags)
		{
			/** 如果一个标签仅仅在项目设置（DefaultGameplayTags.ini）或蓝图编辑器中手动输入定义，
			 * 而没有在 C++ 代码中显式注册为“原生标签”，那么 C++ 编译器在编译时根本不知道这个字符串是否合法，此时要用 RequestGameplayTag获取
			 */
			FGameplayTag MessageTag = FGameplayTag::RequestGameplayTag("Message");
			
			// // 给MessageWidgetDataTable做判空处理，nullptr则continue
			if (MessageWidgetDataTable == nullptr) continue;
			
			// 如果 某个资产标签 成功匹配 Message下的标签：（"A.1".MatchesTag("A") will return True, "A".MatchesTag("A.1") will return False ）
			if (Tag.MatchesTag(MessageTag))
			{
				// 获取该标签所在的 数据表行
				const FUIWidgetRow* Row = GetDataTableRowByTag<FUIWidgetRow>(MessageWidgetDataTable, Tag);
				// 触发广播 该标签所在的数据表行 的委托，并将数据表行作为参数传出，接受由蓝图逻辑处理
				MessageWidgetRowDelegate.Broadcast(*Row);
			}
			
			//向 Widget Controller 广播 Tag
			GEngine->AddOnScreenDebugMessage(10,5.f,FColor::Orange,FString::Printf(TEXT("Effect Applied Tag: %s"), *Tag.ToString()));
			
		}
		
	});
	
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Lambda registered"));
}

