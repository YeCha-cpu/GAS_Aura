// Copyright GYQ


#include "GAS/MMC/MMC_MaxHealth.h"
#include "GAS/AS/AuraAttributeSet.h"
#include "Interface/CombatInterface.h"

UMMC_MaxHealth::UMMC_MaxHealth()
{
	// 1. 指定要捕获的属性：绑定到 Aura属性集 中的 Vigor (活力)
	// 只有在这里定义后，才能在 CalculateBaseMagnitude_Implementation 中通过 
	// GetCapturedAttributeMagnitude 成功获取到该属性的值
	VigorDef.AttributeToCapture = UAuraAttributeSet::GetVigorAttribute();
	
	// 2. 指定捕获来源：
	// 含义：从该 GameplayEffect 的 "目标"（即受术者）身上获取 Vigor。
	VigorDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	
	// 3. 设置快照 (Snapshot) 模式：
	// - true（快照）：在 GE 被应用（Apply）的那一帧，抓取一次 Vigor 值并冻结。之后即使角色通过升级或 Buff 改变了 Vigor，最大生命值 也不会更新。
	// - false（实时）：每次计算最大生命值时，都重新去获取当前的 Vigor 值。
	VigorDef.bSnapshot = false;
	
	// 4. 将配置好的捕获定义注册到 MMC 的捕获列表中
	// 注意：这里 Add 之后，GAS 底层会在执行 CalculateBaseMagnitude 之前，
	// 自动根据这份定义从 Target 身上提取 Vigor 值并缓存，供后续 Get 函数调用。
	// 若遗漏此步骤，GetCapturedAttributeMagnitude 将永远返回 0。
	RelevantAttributesToCapture.Add(VigorDef);
}

float UMMC_MaxHealth::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	// 1. 获取上下文的 源/目标 标签（用于属性修改器筛选能否执行此修改器）
	// 这些标签用于在计算属性时，决定哪些 GameplayEffect 的 Modifier 应该被包含或忽略
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	
	// 2. 配置属性修改评估参数
	// 将源/目标标签传入评估参数，确保后续获取 Vigor 时能正确计算当前生效的 Buff/Debuff
	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;
	
	// 3. 捕获并获取 "Vigor (活力)" 属性的当前最终数值，
	// GetCapturedAttributeMagnitude 函数会自动从 Target（或Source）身上获取该属性的当前最终数值
	float Vigor = 0.f;
	GetCapturedAttributeMagnitude(VigorDef, Spec, EvaluationParameters, Vigor);
	
	// 对活力值进行钳制处理，防止负数导致生命值异常降低（防御性编程）
	Vigor = FMath::Max<float>(Vigor, 0.f);
	
	// 4. 获取施法者/来源对象的等级
	// 此处约定：施加该 GE 的 SourceObject 必须实现了 ICombatInterface 接口
	ICombatInterface* CombatInterface = Cast<ICombatInterface>(Spec.GetContext().GetSourceObject());
	
	// 若接口转换失败（例如某些非角色单位施加），降级使用默认等级 1
	const int32 PlayerLevel = CombatInterface ? CombatInterface->GetPlayerLevel() : 1;
	
	// 5. 返回最终计算公式
	// 最大生命值 = 80（基础值）+ 2.5 × 活力属性 + 10 × 角色等级
	return 80.f + 2.5f * Vigor + 10.f * PlayerLevel;
}
