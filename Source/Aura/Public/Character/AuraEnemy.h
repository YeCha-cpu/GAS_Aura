// Copyright GYQ

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "Interface/EnemyInterface.h"
#include "AuraEnemy.generated.h"


UCLASS()
class AURA_API AAuraEnemy : public AAuraCharacterBase, public IEnemyInterface// 继承 IEnemyInterface 接口
{
	GENERATED_BODY()
	
public:
	AAuraEnemy();
	
	virtual void HighlightActor() override;
	virtual void UnHighlightActor() override;
	virtual int32 GetPlayerLevel() override;
	
protected:
	virtual void BeginPlay() override;
	virtual void InitAbilityActorInfo() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Class Defaults")
	int32 Level = 1;
	
};
