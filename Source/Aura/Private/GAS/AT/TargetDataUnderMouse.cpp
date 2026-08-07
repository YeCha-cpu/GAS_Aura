// Copyright GYQ

#include "GAS/AT/TargetDataUnderMouse.h"

#include "AbilitySystemComponent.h"
#include "GameFramework/PlayerController.h"

UTargetDataUnderMouse* UTargetDataUnderMouse::CreatTargetDataUnderMouse(UGameplayAbility* OwningAbility)
{
	// 标准工厂方法：创建任务实例
	UTargetDataUnderMouse* MyObj = NewAbilityTask<UTargetDataUnderMouse>(OwningAbility);
	return MyObj;
}

void UTargetDataUnderMouse::Activate()
{
	// 判断是否属于本地控制端（玩家本人）
	const bool bIsLocallyControlled = Ability->GetCurrentActorInfo()->IsLocallyControlled();
	if (bIsLocallyControlled)
	{
		// 客户端：立即获取鼠标数据并发送
		SendMouseCursorData();
	}
	else
	{
		// 服务器端（或远程客户端）：监听来自客户端的复制数据
		FGameplayAbilitySpecHandle SpecHandle = GetAbilitySpecHandle();
		FPredictionKey ActivationPredictionKey = GetActivationPredictionKey();
		// 注册委托：当客户端数据到达时触发回调
		AbilitySystemComponent.Get()->AbilityTargetDataSetDelegate(SpecHandle, ActivationPredictionKey).AddUObject(this, &UTargetDataUnderMouse::OnTargetDataReplicatedCallback);
		// 检查数据是否已经到达（防止先到后注册的情况）
		const bool bCalledDelegate = AbilitySystemComponent.Get()->CallReplicatedTargetDataDelegatesIfSet(SpecHandle, ActivationPredictionKey);
		if(!bCalledDelegate)
		{
			// 若未到达，标记任务正在等待远程数据
			SetWaitingOnRemotePlayerData();
		}
	}
	
}

void UTargetDataUnderMouse::SendMouseCursorData()
{
	// 开启预测窗口（确保预测密钥正确处理）
	FScopedPredictionWindow ScopedPrediction(AbilitySystemComponent.Get());
	
	// 获取鼠标下的命中结果
	APlayerController* PC = Ability->GetCurrentActorInfo()->PlayerController.Get();
	FHitResult CursorHit; 
	PC->GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);
	
	// 将命中结果包装成 GAS 标准目标数据格式
	FGameplayAbilityTargetDataHandle DataHandle;
	FGameplayAbilityTargetData_SingleTargetHit* TargetData = new FGameplayAbilityTargetData_SingleTargetHit();
	TargetData->HitResult = CursorHit;
	DataHandle.Add(TargetData);
	
	// 通过内置 RPC 将数据发送到服务器
	AbilitySystemComponent->ServerSetReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey(), DataHandle, FGameplayTag(), AbilitySystemComponent->ScopedPredictionKey);

	// 本地立即广播（提升响应速度，例如显示瞄准效果）
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		ValidData.Broadcast(DataHandle);
	}
	
}

void UTargetDataUnderMouse::OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& DataHandle, FGameplayTag ActivationTag)
{
	// 服务器端消费数据（避免重复处理）
	AbilitySystemComponent->ConsumeClientReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey());
	// 广播目标数据，触发后续逻辑（如生成投射物）
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		ValidData.Broadcast(DataHandle);
	}
}