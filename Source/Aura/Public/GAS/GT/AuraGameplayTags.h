// Copyright GYQ

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

/**
 *【核心架构类】原生游戏标签单例管理器
 * 功能：
 * 1. 将所有需要在 C++ 和蓝图中同时使用的 GameplayTag 定义为成员变量。
 * 2. 确保这些标签在游戏启动时（AssetManager 初始化阶段）就被注册进引擎全局标签管理器。
 * 3. 外部调用此单例直接获取标签，无需使用 RequestGameplayTag（避免运行时字符串查找开销）。
 * 
 * 设计模式：单例（Singleton），在 C++ 静态存储区存放唯一实例。
 * 注意：用 struct 是为了让所有成员默认 public，减少重复写 public: 的代码量。
 */
struct FAuraGameplayTags
{
private:
	/** 唯一的静态单例，存储在程序的全局静态区。外部无法直接访问或修改。*/
	static FAuraGameplayTags GameplayTags;
	
public:
	// 单例的访问入口。外部调用类方法 FAuraGameplayTags::Get() 就能拿到唯一的实例对象
	static const FAuraGameplayTags& Get() { return GameplayTags;}
	
	// 静态初始化函数，用来为本地标签进行注册
	static void InitNativeGameplayTags(); 
	
	// ==================== 创建本地标签成员变量（对外暴露的全局唯一句柄） ====================
	
	FGameplayTag Attributes_Primary_Strength;/** 主属性标签：力量（增加攻击力） */
	FGameplayTag Attributes_Primary_Intelligence;/** 主属性标签：智力（增加技能效果） */
	FGameplayTag Attributes_Primary_Resilience;/** 主属性标签：韧性（增加技能效果） */
	FGameplayTag Attributes_Primary_Vigor;/** 主属性标签：耐力（增加生命值） */
	
	FGameplayTag Attributes_Secondary_Armor;/** 次级属性标签：护甲（减少伤害，增加格挡几率） */
	FGameplayTag Attributes_Secondary_ArmorPenrtration;/** 次级属性标签：护甲穿透（增加伤害） */
	FGameplayTag Attributes_Secondary_BlockChance;/** 次级属性标签：格挡几率（增加格挡成功率） */
	FGameplayTag Attributes_Secondary_CriticalHitChance;/** 次级属性标签：暴击几率（增加暴击率） */
	FGameplayTag Attributes_Secondary_CriticalHitDamage;/** 次级属性标签：暴击伤害（增加暴击伤害） */
	FGameplayTag Attributes_Secondary_CriticalHitResistance;/** 次级属性标签：暴击抗性（减少暴击伤害） */
	FGameplayTag Attributes_Secondary_HealthRegeneration;/** 次级属性标签：生命值回复（增加生命值回复） */
	FGameplayTag Attributes_Secondary_ManaRegeneration;/** 次级属性标签：法力值回复（增加法力值回复） */
	FGameplayTag Attributes_Secondary_MaxHealth;/** 次级属性标签：最大生命值（增加生命值） */
	FGameplayTag Attributes_Secondary_MaxMana;/** 次级属性标签：最大法力值（增加法力值） */
	
};
