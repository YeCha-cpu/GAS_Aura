// Copyright GYQ

#include "GAS/Execcalc/Execcalc_Damage.h"
#include "AbilitySystemComponent.h"
#include "GAS/AS/AuraAttributeSet.h"

/**
 * 静态结构体，用于集中管理本 Execution 需要捕获的所有属性定义。
 * 
 * 使用 DECLARE_ATTRIBUTE_CAPTUREDEF / DEFINE_ATTRIBUTE_CAPTUREDEF 宏
 * 可以自动生成形如 “ArmorDef” 的 FGameplayEffectAttributeCaptureDefinition 成员，
 * 并方便地在构造中初始化捕获来源（Source/Target）和快照模式。
 * 
 * 此结构体采用“单例”模式，通过 DamageStatics() 函数获取静态实例，
 * 确保整个模块内只存在一份捕获定义，避免重复初始化。
 */
struct AuraDamageStatics
{
	// 声明并定义 Armor 属性的捕获定义（宏展开后会生成 ArmorDef 成员）
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);
	
	AuraDamageStatics()
	{
		/**
		 * 定义 Armor 属性的捕获方式：
		 * - 属性：UAuraAttributeSet::Armor
		 * - 来源：Target（从受击者身上获取）
		 * - 快照：false（实时读取当前值，不冻结）
		 * 
		 * 注意：若需捕获攻击者属性（如 AttackPower），可在此处增加另一条定义，
		 * 并将来源设为 Source。
		 */
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Armor, Target, false);
	}
};

/**
 * 获取静态的 AuraDamageStatics 实例。
 * 在 C++ 中，使用函数内静态局部变量是实现线程安全单例的常见手法。
 */
static const AuraDamageStatics& DamageStatics()
{
	static AuraDamageStatics Dstatic;
	return Dstatic;
}

// 构造函数
UExeccalc_Damage::UExeccalc_Damage()
{
	/**
	 * 将我们在 DamageStatics 中定义的 Armor 捕获注册到本 Execution 的
	 * RelevantAttributesToCapture 数组中。
	 * 
	 * 这是【必须】操作：GAS 底层会根据此数组在 Execute 执行前预先从
	 * 目标 ASC 中抓取属性值并缓存，以便在 Execute 中通过
	 * AttemptCalculateCapturedAttributeMagnitude 高效获取。
	 * 
	 * 如果忘记添加，则执行时获取到的属性值将始终为 0。
	 */
	RelevantAttributesToCapture.Add(DamageStatics().ArmorDef);
}

/**
 * GAS 自定义GE执行计算：伤害结算自定义计算逻辑
 * 继承 UGameplayEffectCustomExecutionBase 的执行实现函数
 * 作用：在GE运行阶段动态读取目标护甲属性，组装护甲修正Modifier下发到属性计算
 */
void UExeccalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
    FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
    // 1. 从执行参数中获取施法方、受击方的AbilitySystemComponent
    const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
    const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

    // 2. 通过ASC拿到对应的绑定Actor（实体角色对象），做空指针安全判断
    const AActor* SourceAvatar = SourceASC ? SourceASC->GetAvatarActor() : nullptr;
    const AActor* TargetAvatar = TargetASC ? TargetASC->GetAvatarActor() : nullptr;

    // 3. 获取当前正在执行的GameplayEffect实例配置数据
    const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

    // 4. 提取GE捕获的施法方、受击方已聚合的GameplayTag标签集合
    const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
    const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

    // 5. 构建属性求值所需的环境参数结构体
    FAggregatorEvaluateParameters EvaluationParameters;
    // 把上下文中的双方Tag传入，属性计算时可基于Tag做条件判断
    EvaluationParameters.SourceTags = SourceTags;
    EvaluationParameters.TargetTags = TargetTags;

    // 初始化护甲数值为0
    float Armor = 0.f;

    // 6. 尝试读取GE预先捕获的【护甲防御】属性当前数值，结果存入Armor变量
    // DamageStatics() 是全局静态属性配置单例，ArmorDef为护甲属性标识
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorDef, EvaluationParameters, Armor);
    // 护甲数值保底0，防止护甲为负数出现异常减伤/增伤bug
    Armor = FMath::Max<float>(0.f, Armor);

    // 演示用：护甲数值+1（实际项目里一般删掉，这里是临时测试修正）
    ++Armor;

    // 7. 构造属性修改数据：对护甲属性执行【加法运算】，数值为当前读取到的护甲值
    // ArmorProperty：目标护甲属性；EGameplayModOp::Additive 加法Modifier
    const FGameplayModifierEvaluatedData EvaluatedData(DamageStatics().ArmorProperty, EGameplayModOp::Additive, Armor);

    // 8. 将构造好的属性修改器输出，交由GAS后续流程应用到目标角色的属性聚合器中
    OutExecutionOutput.AddOutputModifier(EvaluatedData);
}