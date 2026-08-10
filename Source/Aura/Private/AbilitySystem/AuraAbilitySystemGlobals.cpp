// Copyright GYQ

#include "AbilitySystem/AuraAbilitySystemGlobals.h"

// 引入自定义上下文类型的定义。这个头文件中通常会声明 FAuraGameplayEffectContext 结构体，
// 并可能在内部额外存储 int、float、FVector 或 TArray 等自定义字段。
#include "AuraAbilityTypes.h"

/**
 * 实现分配函数
 * 
 * 实现细节：
 * 1. 直接使用 new 在堆上创建 FAuraGameplayEffectContext 对象。
 * 2. GAS 内部会通过智能指针或手动管理该内存，通常在效果执行完毕后自动调用析构。
 * 3. 由于返回的是基类指针 (FGameplayEffectContext*)，多态性确保后续
 *    调用 Get/Set 方法时，会执行我们重写过的派生类函数（如果存在）。
 */
FGameplayEffectContext* UAuraAbilitySystemGlobals::AllocGameplayEffectContext() const
{
	// 返回自定义类型的实例，替代了基类默认的 new FGameplayEffectContext()
	return new FAuraGameplayEffectContext();
}