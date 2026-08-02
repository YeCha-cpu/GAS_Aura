// Copyright GYQ


#include "Character/AuraCharacter.h"
#include "AbilitySystemComponent.h"
#include "Core/AuraHUD.h"
#include "Core/AuraPlayerController.h"
#include "Core/AuraPlayerState.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GAS/AuraAbilitySystemComponent.h"

AAuraCharacter::AAuraCharacter()
{
	GetCharacterMovement()->bOrientRotationToMovement = true;	// 允许角色自动旋转
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);// 旋转速度
	
	// 启用平面约束并将角色初始位置对齐到平面
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;
	
	// 禁用控制器旋转
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	
}

// PossessedBy：Pawn 被 PlayerController 接管时，仅在【服务器】调用的函数
void AAuraCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	InitAbilityActorInfo();
	
}

// OnRep_PlayerState：PlayerState是服务器复制给客户端的，当【客户端】收到服务器同步的 PlayerState 时，会触发 OnRep_PlayerState 这个「复制通知函数」。
void AAuraCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	
	InitAbilityActorInfo();
}

int32 AAuraCharacter::GetPlayerLevel()
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	return AuraPlayerState->GetPlayerLevel();
}

void AAuraCharacter::InitAbilityActorInfo()
{
	// 初始化技能系统组件
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	AuraPlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(AuraPlayerState, this);
	
	// 绑定 应用GE后 触动的委托回调
	Cast<UAuraAbilitySystemComponent>(AuraPlayerState->GetAbilitySystemComponent())->AbilityActorInfoSet();
	
	// 从 玩家状态 中获取 能力系统组件 和 属性集
	AbilitySystemComponent = AuraPlayerState->GetAbilitySystemComponent();
	AttributeSet = AuraPlayerState->GetAttributeSet();
	
	// 获取 HUD
	if (AAuraPlayerController* AuraPlayerController = Cast<AAuraPlayerController>(GetController()))
	{
		if (AAuraHUD* AuraHUD = Cast<AAuraHUD>(AuraPlayerController->GetHUD()))
		{
			// 初始化 OverlayWidget
			AuraHUD->InitOverlay(AuraPlayerController, AuraPlayerState, AbilitySystemComponent, AttributeSet);
		}
	}
	// 用两个GE初始化 主要属性 与 次要属性
	InitializeDefaultAttributes();
}
