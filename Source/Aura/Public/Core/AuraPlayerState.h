// Copyright GYQ

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"
#include "AuraPlayerState.generated.h"

class UAbilitySystemComponent;
class UAttributeSet;

UCLASS()
class AURA_API AAuraPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	AAuraPlayerState();
	
	// 获取能力系统组件的接口
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	// 获取属性集的函数
	FORCEINLINE UAttributeSet* GetAttributeSet() const { return AS; }
	
protected:
	// 能力系统组件 和 属性集 指针
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> ASC;
	UPROPERTY()
	TObjectPtr<UAttributeSet> AS;
	
};
