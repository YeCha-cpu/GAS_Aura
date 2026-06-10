// Copyright GYQ


#include "Character/AuraCharacter.h"

#include "GameFramework/CharacterMovementComponent.h"

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
