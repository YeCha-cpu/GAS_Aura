// Copyright GYQ

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "AuraInputConfig.generated.h"

class UInputAction;

// 输入动作结构体
USTRUCT(BlueprintType)
struct FAuraInputAction
{
	GENERATED_BODY()
	
	// 输入动作
	UPROPERTY(EditDefaultsOnly)
	const UInputAction* InputAction = nullptr;
	
	// 输入标签
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag InputTag = FGameplayTag();
};

/**
 * 
 */
UCLASS()
class AURA_API UAuraInputConfig : public UDataAsset
{
	GENERATED_BODY()
	
public:
	// 技能输入动作数组
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FAuraInputAction> AbilityInputActions;
	
	// 通过输入标签查找技能输入动作
	const UInputAction* FindAbilityInputActionByTag(const FGameplayTag& InputTag, bool bLogNotFound = false) const;
};
