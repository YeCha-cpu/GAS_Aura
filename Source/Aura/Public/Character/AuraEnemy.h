// Copyright GYQ

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "DataAssets/CharacterClassInfo.h"
#include "Interface/EnemyInterface.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "AuraEnemy.generated.h"

class UWidgetComponent;

UCLASS()
class AURA_API AAuraEnemy : public AAuraCharacterBase, public IEnemyInterface// 继承 IEnemyInterface 接口
{
	GENERATED_BODY()
	
public:
	AAuraEnemy();
	
	virtual void HighlightActor() override;
	virtual void UnHighlightActor() override;
	virtual int32 GetPlayerLevel() override;
	virtual void Die() override;
	
	// 属性变化委托
	UPROPERTY(BlueprintAssignable)
	FOnAttributeChanged_Signature OnHealthChanged;
	
	UPROPERTY(BlueprintAssignable)
	FOnAttributeChanged_Signature OnMaxHealthChanged;
	
	// 受击标签数量发生变化时的回调 
	void HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount);
	
protected:
	virtual void BeginPlay() override;
	virtual void InitAbilityActorInfo() override;
	virtual void InitializeDefaultAttributes() const override;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Class Defaults")
	int32 Level = 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Class Defaults")
	ECharacterClass CharacterClass = ECharacterClass::Warrior;
	
	// 头顶血条 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UWidgetComponent> HealthBar;
	
	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	bool bHitReacting = false;
	
	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	float BaseWalkSpeed = 250.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float LifeSpan = 5.f;
};
