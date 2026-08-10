// Copyright GYQ

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemGlobals.h" // 引入GAS全局管理器基类
#include "AuraAbilitySystemGlobals.generated.h"

/**
 * 【自定义能力系统全局管理器】
 * 
 * 在 DefaultGame.ini 配置文件中添加以下内容：
 * [/Script/GameplayAbilities.AbilitySystemGlobals]
 * +AbilitySystemGlobalsClassName="/Script/Aura.AuraAbilitySystemGlobals"
 * 
 * 作用：重写基类的工厂方法，让整个GAS框架在创建“游戏效果上下文”时，
 *       统一使用我们自定义的 FAuraGameplayEffectContext，而不是默认的版本。
 *       这使得每个技能/效果都能携带项目特有的数据（例如：暴击标记、元素类型、命中骨骼等）。
 */
UCLASS() // 标记为UObject类，支持UE的反射和垃圾回收
class AURA_API UAuraAbilitySystemGlobals : public UAbilitySystemGlobals
{
	GENERATED_BODY() // UE必须的宏，用于生成构造函数等样板代码
	
public:
	/**
	 * 重写分配游戏效果上下文的虚函数
	 * 该函数在 GAS 内部被频繁调用（如应用伤害、添加Buff时），用于创建一个上下文对象来传递本次效果相关的临时数据。
	 * 
	 * @return 返回新分配的自定义效果上下文指针，调用方负责管理其生命周期（通常由GAS内部自动释放）
	 */
	virtual FGameplayEffectContext* AllocGameplayEffectContext() const override;
};