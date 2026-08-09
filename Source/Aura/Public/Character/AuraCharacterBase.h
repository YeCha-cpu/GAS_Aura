// Copyright GYQ

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "Abilities/GameplayAbility.h"
#include "GameFramework/Character.h"
#include "Interface/CombatInterface.h"
#include "AuraCharacterBase.generated.h"

class UMaterialInstance;
class UGameplayEffect;
class UAbilitySystemComponent;
class UAttributeSet;
class UGameplayAbility;
class UAnimMontage;

UCLASS()
class AURA_API AAuraCharacterBase : public ACharacter, public IAbilitySystemInterface, public ICombatInterface
{
	GENERATED_BODY()

public:
	AAuraCharacterBase();
	
	// 获取能力系统组件的接口
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	// 获取属性集的函数
	FORCEINLINE UAttributeSet* GetAttributeSet() const { return AttributeSet; }
	
	// 重写战斗接口中的获取受击反应蒙太奇的函数	
	virtual UAnimMontage* GetHitReactMontage_Implementation() override;
	
	virtual void Die() override;

	UFUNCTION(NetMulticast, Reliable)
	virtual void MulticastHandleDeath();
	
protected:
	virtual void BeginPlay() override;
	
	// 角色武器
	UPROPERTY(EditAnywhere,Category = "Combat")
	TObjectPtr<USkeletalMeshComponent> Weapon;
	
	// 武器的Socket名称
	UPROPERTY(EditAnywhere,Category = "Combat")
	FName WeaponTipSocketName;
	
	// 获取武器的Socket位置
	virtual FVector GetCombatSocketLocation() override;
	
	// 能力系统组件 和 属性集 指针
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

	// 初始化技能系统组件，指定 OwnerActor 和 AvatarActor
	virtual void InitAbilityActorInfo();
	
	// 负责默认初始化重要属性的GE
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attributes")
	TSubclassOf<UGameplayEffect> DefaultVitalAttributes;
	
	// 负责默认初始化主要属性的GE
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attributes")
	TSubclassOf<UGameplayEffect> DefaultPrimaryAttributes;
	
	// 负责默认初始化次要属性的GE
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attributes")
	TSubclassOf<UGameplayEffect> DefaultSecondaryAttributes;
	
	// 初始化默认属性函数（主要属性 与 次要属性）
	virtual void InitializeDefaultAttributes() const;
	
	// 创建并应用目标GE
	void ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass, float Level) const;
	
	/** GA初始化函数 */
	void AddCharacterAbilities();
	
	/** 死亡溶解效果 */
	
	void Dissolve();
	
	UFUNCTION(BlueprintImplementableEvent)
	void StartCharacterDissolveTimeline(UMaterialInstanceDynamic* DynamicMaterialInstance);
	
	UFUNCTION(BlueprintImplementableEvent)
	void StartWeaponDissolveTimeline(UMaterialInstanceDynamic* DynamicMaterialInstance);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dissolution")
	TObjectPtr<UMaterialInstance> CharacterDissolveMaterialInstance;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dissolution")
	TObjectPtr<UMaterialInstance> WeaponDissolveMaterialInstance	;
	
private:
	// 要初始化的能力（蓝图中设置）
	UPROPERTY(EditAnywhere, Category = "Abilities")
	TArray<TSubclassOf<UGameplayAbility>> StartUpAbilities;

	// 角色受击反应蒙太奇 
	UPROPERTY(EditAnywhere, Category = "Combat")
	TObjectPtr<UAnimMontage> HitReactMontage;
};
