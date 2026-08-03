// Copyright GYQ

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AuraGASLibrary.generated.h"

class UOverlayWidgetController;

/**
 * 【蓝图函数库】GAS 相关的实用工具函数
 * 
 * 继承自 UBlueprintFunctionLibrary，提供可在蓝图中直接调用的静态函数。
 * 设计意图：将 GAS 和 UI 框架中常用的“获取/创建 WidgetController”的逻辑
 * 封装成工具函数，供蓝图和 C++ 复用，避免重复代码。
 * 
 * 注意：函数库中的函数必须是静态（static）且标记 UFUNCTION。
 */
UCLASS()
class AURA_API UAuraGASLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	/**
	 * 【蓝图可调用】获取 OverlayWidgetController（覆盖层 UI 控制器）
	 * 
	 * 功能：根据当前世界上下文，自动提取 PlayerController、PlayerState、
	 * AbilitySystemComponent 和 AttributeSet，然后通过 AuraHUD 获取或创建
	 * 对应的 OverlayWidgetController 实例。
	 * 
	 * 使用场景：
	 * - 在蓝图中需要直接获取 OverlayWidgetController 时调用。
	 * - 作为 UI 框架的“快捷入口”，避免蓝图手动查找 HUD 和解析参数。
	 * 
	 * @param WorldContextObject 世界上下文对象（通常传入 self 或 GetWorld()）
	 * @return 有效的 UOverlayWidgetController 指针；若获取失败（如没有 HUD），返回 nullptr
	 * 
	 * 注意：此函数是 BlueprintPure（纯函数），不会改变状态，可在蓝图事件图表中直接连线使用。
	 */
	UFUNCTION(BlueprintPure, Category = "Library|WidgetController", meta = (WorldContext = "WorldContextObject"))
	static UOverlayWidgetController* GetOverlayWidgetController(const UObject* WorldContextObject);
	
};