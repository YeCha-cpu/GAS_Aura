// Copyright GYQ

#include "GAS/Execcalc/Execcalc_Damage.h"
#include "AbilitySystemComponent.h"
#include "AuraAbilityTypes.h"
#include "DataAssets/AttributesInfo.h"
#include "DataAssets/CharacterClassInfo.h"
#include "GAS/AuraAbilitySystemComponent.h"
#include "GAS/AS/AuraAttributeSet.h"
#include "GAS/BluePrintFunctionLibrary/AuraGASLibrary.h"
#include "GAS/GT/AuraGameplayTags.h"
#include "Interface/CombatInterface.h"

// 声明 Armor 属性的捕获定义（宏展开后会生成 ArmorDef 成员）
struct AuraDamageStatics
{
	// 声明并定义 各种属性的捕获定义（宏展开后会生成如 ArmorDef 的成员）
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor); // 等效于 FGameplayEffectAttributeCaptureDefinition ArmorDef;
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArmorPenetration); 
	DECLARE_ATTRIBUTE_CAPTUREDEF(BlockChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitResistence);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitDamage);
	
	AuraDamageStatics()
	{
		// 向 UE 编辑器注册一条“属性取用规则，Target = 从目标身上取属性, Source = 从源身上取属性, false = 非快照（实时取值）
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Armor, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArmorPenetration, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, BlockChance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitChance, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitResistence, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitDamage, Source, false);
		
	}
};


static const AuraDamageStatics& DamageStatics()
{
	static AuraDamageStatics Dstatic;
	return Dstatic;
}

/**
 * 在构造函数中将我们在 DamageStatics 中定义的 Armor 捕获注册到本 Execution 的
 * RelevantAttributesToCapture 数组中。
 * 
 * 这是【必须】操作：GAS 底层会根据此数组在 Execute 执行前预先从
 * 目标 ASC 中抓取属性值并缓存，以便在 Execute 中通过
 * AttemptCalculateCapturedAttributeMagnitude 高效获取。 * 
 * 如果忘记添加，则执行时获取到的属性值将始终为 0。
 */
UExeccalc_Damage::UExeccalc_Damage()
{
	RelevantAttributesToCapture.Add(DamageStatics().ArmorDef);
	RelevantAttributesToCapture.Add(DamageStatics().ArmorPenetrationDef);
	RelevantAttributesToCapture.Add(DamageStatics().BlockChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitResistenceDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitDamageDef);
	
}

/**
 * 计算的核心实现
 */
void UExeccalc_Damage::Execute_Implementation(
	const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	// 获取源和目标的能力系统组件
	const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

	// 获取源和目标对应的 Actor（用于后续可能需要的调试或上下文）
	AActor* SourceAvatar = SourceASC ? SourceASC->GetAvatarActor() : nullptr;
	AActor* TargetAvatar = TargetASC ? TargetASC->GetAvatarActor() : nullptr;
	
	// 获取源和目标角色的 ICombatInterface 接口
	ICombatInterface* SourceCombatInterface = Cast<ICombatInterface>(SourceAvatar);
	ICombatInterface* TargetCombatInterface = Cast<ICombatInterface>(TargetAvatar);

	// 获取当前的 GE 规格（Spec），包含所有参数和标签
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	// 获取源和目标的标签聚合容器（用于属性评估时匹配标签）
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	// 设置属性评估参数（将标签传入，以便属性计算时考虑标签依赖）
	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;
    
	// 从 GE Spec 的 SetByCaller 中读取原始伤害值（标签为 Effect_Damage）
	float Damage = Spec.GetSetByCallerMagnitude(FAuraGameplayTags::Get().Effect_Damage);
	
	// 1、捕获目标的 格挡几率 看是否格挡成功，若成功则伤害值减半
	float TargetBlockChance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BlockChanceDef, EvaluationParameters, TargetBlockChance);
	TargetBlockChance = FMath::Max<float>(TargetBlockChance, 0);
	// 随机数生成格挡结果
	const bool bBlocked = FMath::RandRange(1,100) < TargetBlockChance;
	
	FGameplayEffectContextHandle EffectContextHandle = Spec.GetContext(); // 获取 GE 的执行上下文句柄
	UAuraGASLibrary::SetIsBlockedHit(EffectContextHandle, bBlocked);// 设置格挡结果
	
	Damage = bBlocked ? Damage / 2.f : Damage; // 如格挡成功，伤害值减半
	
	// 2、捕获目标的护甲属性 与 源的护甲穿透属性，分别用于计算伤害减免 和 忽略目标护甲的一定比率，直接造成真伤的几率
	float TargetArmor = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorDef, EvaluationParameters, TargetArmor);
	TargetArmor = FMath::Max<float>(TargetArmor, 0.f);
	float SourceArmorPenetration = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorPenetrationDef, EvaluationParameters, SourceArmorPenetration);
	SourceArmorPenetration = FMath::Max<float>(SourceArmorPenetration, 0.f);
	// 通过 GAS蓝图函数库(UAuraGASLibrary) 查找到对应的 CharacterClassInfo 并获取护甲穿透曲线，
	const UCharacterClassInfo* CharacterClassInfo = UAuraGASLibrary::GetCharacterClassInfo(SourceAvatar);
	const FRealCurve* ArmorPenetrationCurve = CharacterClassInfo->DamageCalculationCoefficients->FindCurve(FName("ArmorPenetration"),FString());
	// 再通过 源角色的战斗接口 获取源角色的等级，根据等级在曲线上查找对应的护甲穿透值
	const float SourceArmorPenetrationCoefficient = ArmorPenetrationCurve->Eval(SourceCombatInterface->GetPlayerLevel());
	// 计算最终的有效护甲（4个护甲穿透点才能抵消目标1%的护甲）
	const float EffectiveArmor = TargetArmor * (100 - SourceArmorPenetration * SourceArmorPenetrationCoefficient) / 100.f;
	// 获取有效护甲曲线
	const FRealCurve* EffectiveArmorCurve = CharacterClassInfo->DamageCalculationCoefficients->FindCurve(FName("EffectiveArmor"),FString());
	// 通过 目标角色的战斗接口 获取目标角色的等级，根据等级在曲线上查找对应的有效护甲值
	const float EffectiveArmorCoefficient = EffectiveArmorCurve->Eval(TargetCombatInterface->GetPlayerLevel());
	// 根据有效护甲来对伤害进行进一步削减
	Damage *= (100 - EffectiveArmor * EffectiveArmorCoefficient) / 100.f;
	
	/** 5、暴击相关（同上） */
	// 依旧捕获源角色的暴击几率，用于判断是否暴击
	float SourceCriticalHitChance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitChanceDef, EvaluationParameters, SourceCriticalHitChance);
	SourceCriticalHitChance = FMath::Max<float>(SourceCriticalHitChance, 0.f);
	// 依旧捕获目标角色的暴击抗性，用于削减暴击伤害
	float TargetCriticalHitResistence = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitResistenceDef, EvaluationParameters, TargetCriticalHitResistence);
	TargetCriticalHitResistence = FMath::Max<float>(TargetCriticalHitResistence, 0.f);
	// 依旧捕获源角色的暴击伤害，用于增加暴击伤害
	float SourceCriticalHitDamage = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitDamageDef, EvaluationParameters, SourceCriticalHitDamage);
	SourceCriticalHitDamage = FMath::Max<float>(SourceCriticalHitDamage, 0.f);
	// 获取暴击抗性曲线
	const FRealCurve* CriticalHitResistenceCurve = CharacterClassInfo->DamageCalculationCoefficients->FindCurve(FName("CriticalHitResistence"),FString());
	// 通过 目标角色的战斗接口 获取目标角色的等级，根据等级在曲线上查找对应的有效暴击几率值
	const float CriticalHitResistenceCoefficient = CriticalHitResistenceCurve->Eval(TargetCombatInterface->GetPlayerLevel());
	// 计算最终的有效暴击几率（源暴击几率 减去 目标暴击抗性乘以0.15）
	const float EffectiveCriticalHitChance = SourceCriticalHitChance - TargetCriticalHitResistence * CriticalHitResistenceCoefficient;
	
	const bool bCriticalHit = FMath::RandRange(1,100) < EffectiveCriticalHitChance;// 随机数生成暴击结果
	UAuraGASLibrary::SetIsCriticalHit(EffectContextHandle, bCriticalHit);// 设置暴击结果
	
	// 如果暴击成功，则将伤害值乘以2倍伤害伤害加上源暴击伤害
	Damage = bCriticalHit ? 2.f * Damage + SourceCriticalHitDamage : Damage;
	
	// 构造一个修改数据，将伤害值以加法形式应用到目标的 IncomingDamage 元属性
	const FGameplayModifierEvaluatedData EvaluatedData(UAuraAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, Damage);
	// 输出该修改，GAS 会将其应用到目标属性
	OutExecutionOutput.AddOutputModifier(EvaluatedData);
}