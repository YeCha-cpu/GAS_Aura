// Copyright GYQ

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "DataAssets/AttributesInfo.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "AttributeMenuWidgetController.generated.h"

class UAttributesInfo;
struct FAuraAttributeInfo;

// 创建属性信息委托实例
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAttributeInfo_Signature, const FAuraAttributeInfo&, Info);

/**
 * 一、核心依赖关系:
 *		基类：UAuraWidgetController —— 提供 PC、PS、ASC、AS 四个核心指针，封装参数传递。
 *		数据资产：UAttributesInfo —— 存储 TArray<FAuraAttributeInfo>，每个元素含 FGameplayTag、显示名称、描述。
 *		属性集：UAuraAttributeSet —— 拥有 TMap<FGameplayTag, TAttributeFuncPtr<FGameplayAttribute()>> TagsToAttributes;，将标签映射到属性的 获取委托（静态函数指针）。
 *		HUD：AAuraHUD —— 负责创建并缓存 UAttributeMenuWidgetController 实例。
 *		蓝图函数库：UAuraGASLibrary —— 提供静态函数，供蓝图快速获取该控制器。
 * 
 * 二、总结：完整的运转逻辑链（时序图）:
 *		启动时：AuraAssetManager 初始化，注册原生 GameplayTag。
 *		角色 Possess 时：AAuraPlayerState 创建 ASC 和 AS。AAuraAttributeSet 在构造函数中填充 TagsToAttributes 映射表。
 *		
 *		点击按钮打开属性菜单 UI 时：
 *		UI 蓝图调用 UAuraGASLibrary::GetAttributeMenuWidgetController。
 *		HUD 检查缓存 -> 为空 -> NewObject 创建 UAttributeMenuWidgetController -> 调用 BindCallbackToDependencies（此时为所有属性挂载了 Lambda 监听） -> 返回。
 *		UI 蓝图绑定 AttributeInfoDelegate。
 *		UI 蓝图调用 BroadcastInitialValues -> 触发 BroadcastAttributeInfo -> 界面显示当前属性值。
 *		
 *		游戏中属性变化时（如升级加点、装备移除）：
 *		GE 修改了 AS 中的数值。
 *		ASC 触发 FOnGameplayAttributeValueChange 多播委托。 
 *		Lambda 回调被执行 -> 调用 BroadcastAttributeInfo -> 新的 FAuraAttributeInfo 通过 AttributeInfoDelegate 广播 -> UI 界面实时刷新。
 */
UCLASS(BlueprintType, Blueprintable)
class AURA_API UAttributeMenuWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()
	
public:
	//  1、—— 初始数据推送：重写 BroadcastInitialValues() —— 遍历所有属性，广播初始值
	virtual void BroadcastInitialValues() override;
	//  2、—— 变化监听（蓝图调用）：重写 BindCallbackToDependencies() —— 为每个属性注册变化回调，当属性变化时调用 BroadcastAttributeInfo 广播更新
	virtual void BindCallbackToDependencies() override;
	
	// 通过动态多播委托 AttributeInfoDelegate 广播给 UI 蓝图
	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FAttributeInfo_Signature AttributeInfoDelegate;
	
protected:
	// 将 属性集（UAuraAttributeSet）中所有主/次级属性的当前数值，连同显示名称、描述等 UI 信息，一起封装成结构体 FAuraAttributeInfo
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAttributesInfo> AttributesInfo;
	
private:
	// 3、—— 组装并发送：私有辅助函数 BroadcastAttributeInfo 负责从数据资产查询信息、获取当前数值、组合并广播
	void BroadcastAttributeInfo(const FGameplayTag& AttributeTag, const FGameplayAttribute& Attribute); 
};
