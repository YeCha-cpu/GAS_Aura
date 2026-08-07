// Copyright GYQ

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "TargetDataUnderMouse.generated.h"

/** 目标数据委托：广播鼠标命中结果（用于蓝图中绑定） */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMouseTargetDataSignature, const FGameplayAbilityTargetDataHandle&, DataHandle);

/**
 * 鼠标目标数据任务
 * 作用：获取鼠标下的目标位置，并处理客户端→服务器的数据复制
 * 流程：客户端捕获鼠标位置 → 发送到服务器 → 两端均广播 ValidData
 */
UCLASS()
class AURA_API UTargetDataUnderMouse : public UAbilityTask
{
	GENERATED_BODY()
	
public:
	/** 创建任务实例（蓝图节点） */
	UFUNCTION(BlueprintCallable, Category = "AbilitySystem|Tasks", meta = (
		DisplayName = "TargetDataUnderMouse",	
		HidePin = "OwningAbility", 
		DefaultToSelf = "OwningAbility", 
		BlueprintInternalUseOnly = "true"))
	static UTargetDataUnderMouse* CreatTargetDataUnderMouse(UGameplayAbility* OwningAbility);
	
	/** 当有效目标数据可用时广播（包含命中结果） */
	UPROPERTY(BlueprintAssignable)
	FMouseTargetDataSignature ValidData;
	
private:
	/** 任务激活时执行（自动调用） */
	virtual void Activate() override;
	
	/** 在本地控制端发送鼠标数据到服务器 */
	void SendMouseCursorData();
	
	/** 服务器接收复制数据的回调 */
	void OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& DataHandle, FGameplayTag ActivationTag);
};