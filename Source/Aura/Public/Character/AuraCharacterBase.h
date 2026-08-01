// Copyright GYQ

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "AuraCharacterBase.generated.h"

class UAbilitySystemComponent;
class UAttributeSet;

UCLASS()
class AURA_API AAuraCharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AAuraCharacterBase();
	
	// 获取能力系统组件的接口
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	// 获取属性集的函数
	FORCEINLINE UAttributeSet* GetAttributeSet() const { return AttributeSet; }

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere,Category = "Combat")
	TObjectPtr<USkeletalMeshComponent> Weapon;
	
	// 能力系统组件 和 属性集 指针
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

	// 初始化技能系统组件，指定 OwnerActor 和 AvatarActor
	virtual void InitAbilityActorInfo();
	
private:

};
