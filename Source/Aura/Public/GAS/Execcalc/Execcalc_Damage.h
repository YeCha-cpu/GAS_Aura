// Copyright GYQ

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "Execcalc_Damage.generated.h"

/**
 * 自定义【执行计算】（Execution Calculation）类，用于处理伤害结算。
 * 
 * 与 MMC（修改器幅度计算）不同，Execution Calculation 可以同时访问攻击者（Source）
 * 和受击者（Target）的属性，并可一次性输出多个属性修改（如扣血、扣蓝、附加效果）。
 * 
 * 本类专门用于计算一次攻击造成的最终伤害值，并直接扣减目标的“生命值（Health）”。
 * 
 * 使用方式：在 GameplayEffect 的 “Execution” 字段中指定本类，且该 GE 的 Duration Policy
 * 通常设为 Instant（瞬时），以确保 Execute 在应用时立即执行一次。
 */
UCLASS()
class AURA_API UExeccalc_Damage : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
	
public:
	UExeccalc_Damage();
	
	/** 
	 * 执行计算的核心入口函数。
	 * 当包含此 Execution 的 GameplayEffect 被应用时，GAS 会调用此函数。
	 * 
	 * @param ExecutionParams  包含当前 GE 的完整上下文（源/目标 ASC、Spec、标签等）
	 * @param OutExecutionOutput 用于输出修改结果（属性变化、条件 GE 等）
	 */
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};