// Copyright GYQ

#include "GAS/GT/AuraGameplayTags.h"
#include "GameplayTagsManager.h"
#include "Animation/AttributesRuntime.h"

// C++ 语法要求，定义一个属于 FAuraGameplayTags 类的、类型也是 FAuraGameplayTags 的静态变量，名字叫 GameplayTags
FAuraGameplayTags FAuraGameplayTags::GameplayTags;

// 静态初始化函数，用来为本地标签进行注册（由 UAuraAssetManager 调用）
void FAuraGameplayTags::InitNativeGameplayTags()
{
	/** ==================================== 向资源管理器注册本地标签，这样此标签就可以在c++或蓝图中使用 ==================================== */
	
	/*
	 * 注册主属性标签
	 */
	GameplayTags.Attributes_Primary_Strength = 
		UGameplayTagsManager::Get().AddNativeGameplayTag(/** 获取全局标签管理器 */
			FName("Attributes.Primary.Strength"),FString("Strength (increases attack power)"));
	
	GameplayTags.Attributes_Primary_Intelligence = 
		UGameplayTagsManager::Get().AddNativeGameplayTag(
			FName("Attributes.Primary.Intelligence"),FString("Intelligence (Boosts skill effects)"));
	
	GameplayTags.Attributes_Primary_Resilience = 
		UGameplayTagsManager::Get().AddNativeGameplayTag(
			FName("Attributes.Primary.Resilience"),FString("Resilience (boosts skill effects)"));
	
	GameplayTags.Attributes_Primary_Vigor = 
		UGameplayTagsManager::Get().AddNativeGameplayTag(
			FName("Attributes.Primary.Vigor"),FString("Vigor (increases HP)"));
	
	/*
	 * 注册次级属性标签
	 */
	GameplayTags.Attributes_Secondary_Armor = 
	UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.Armor"),FString("Armor (reduces damage taken and increases block chance)"));
	
	GameplayTags.Attributes_Secondary_ArmorPenrtration = 
		UGameplayTagsManager::Get().AddNativeGameplayTag(
			FName("Attributes.Secondary.ArmorPenrtration"),FString("ArmorPenetration (Increased Damage)"));
	
	GameplayTags.Attributes_Secondary_BlockChance = 
		UGameplayTagsManager::Get().AddNativeGameplayTag(
			FName("Attributes.Secondary.BlockChance"),FString("BlockChance (increases the success rate of blocking)"));
	
	GameplayTags.Attributes_Secondary_CriticalHitChance = 
		UGameplayTagsManager::Get().AddNativeGameplayTag(
			FName("Attributes.Secondary.CriticalHitChance"),FString("CriticalHitChance (increases critical strike rate)"));
	
	GameplayTags.Attributes_Secondary_CriticalHitDamage = 
		UGameplayTagsManager::Get().AddNativeGameplayTag(
			FName("Attributes.Secondary.CriticalHitDamage"),FString("CriticalHitDamage(Increase Critical Damage)"));
	
	GameplayTags.Attributes_Secondary_CriticalHitResistance = 
		UGameplayTagsManager::Get().AddNativeGameplayTag(
			FName("Attributes.Secondary.CriticalHitResistance"),FString("CriticalHitResistance(Reduce Critical Damage)"));
	
	GameplayTags.Attributes_Secondary_HealthRegeneration = 
		UGameplayTagsManager::Get().AddNativeGameplayTag(
			FName("Attributes.Secondary.HealthRegeneration"),FString("HealthRegeneration(Increase Health regeneration)"));
	
	GameplayTags.Attributes_Secondary_ManaRegeneration = 
		UGameplayTagsManager::Get().AddNativeGameplayTag(
			FName("Attributes.Secondary.ManaRegeneration"),FString("ManaRegeneration(Increase Mana regeneration)"));
	
	GameplayTags.Attributes_Secondary_MaxHealth = 
		UGameplayTagsManager::Get().AddNativeGameplayTag(
			FName("Attributes.Secondary.MaxHealth"),FString("MaxHealth (Increase Health Points)"));
	
	GameplayTags.Attributes_Secondary_MaxMana = 
		UGameplayTagsManager::Get().AddNativeGameplayTag(
			FName("Attributes.Secondary.MaxMana"),FString("MaxMana(Increase Mana points)"));
	
	/*
	 * 输入标签
	 */
	GameplayTags.InputTag_LMB = 
		UGameplayTagsManager::Get().AddNativeGameplayTag(
			FName("InputTag_LMB"),FString( "Left Mouse Button" ));
	
	GameplayTags.InputTag_RMB = 
		UGameplayTagsManager::Get().AddNativeGameplayTag(
			FName("InputTag_RMB"),FString( "Right Mouse Button" ));
	
	GameplayTags.InputTag_Key1 = 
		UGameplayTagsManager::Get().AddNativeGameplayTag(
			FName("InputTag_Key1"),FString( "Key 1" ));
	
	GameplayTags.InputTag_Key2 = 
		UGameplayTagsManager::Get().AddNativeGameplayTag(
			FName("InputTag_Key2"),FString( "Key 2" ));
	
	GameplayTags.InputTag_Key3 = 
		UGameplayTagsManager::Get().AddNativeGameplayTag(
			FName("InputTag_Key3"),FString( "Key 3" ));
	
	GameplayTags.InputTag_Key4 = 
		UGameplayTagsManager::Get().AddNativeGameplayTag(
			FName("InputTag_Key4"),FString( "Key 4" ));
	
	GameplayTags.Effect_Damage = 
		UGameplayTagsManager::Get().AddNativeGameplayTag(
			FName("Damage"),FString( "Damage" ));
	
	GameplayTags.Effect_HitReact = 
		UGameplayTagsManager::Get().AddNativeGameplayTag(
			FName("Effect_HitReact"),FString( "Hit React Effect" ));
}

