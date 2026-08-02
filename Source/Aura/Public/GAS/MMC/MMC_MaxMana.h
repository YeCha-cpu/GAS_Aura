// Copyright GYQ

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "MMC_MaxMana.generated.h"

/**
 * Custom Calculation Class:
 * 自定义计算修改量幅度（Modifier Magnitude Calculation，简称 MMC）
 * 它的作用是自定义动态计算角色的最大法力值（MaxMana）
 */
UCLASS()
class AURA_API UMMC_MaxMana : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()
	
public:
	UMMC_MaxMana();
		
	// 计算基准幅值
	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
		
private:
	// 用来定义游戏效果属性 捕获 的结构体（最大法力值 与 智慧 属性相关，所以要捕获 Intelligence）
	FGameplayEffectAttributeCaptureDefinition IntelligenceDef;
	
};
