// Copyright GYQ

#include "Core/AuraPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Interface/EnemyInterface.h"
#include "Character/AuraCharacter.h"

AAuraPlayerController::AAuraPlayerController()
{
	// 打开网络复制
	bReplicates = true;
	
}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	
	CursorTrace();
	
}

void AAuraPlayerController::CursorTrace()
{
	FHitResult CursorHit;
	GetHitResultUnderCursor(ECC_Visibility,false,CursorHit);
	if (!CursorHit.bBlockingHit) return;
	
	// 更新光标追踪的Actor引用，保存上一帧的Actor并设置当前光标下的Actor
	LastActor = ThisActor;
	ThisActor = Cast<IEnemyInterface>(CursorHit.GetActor());
	
	/**
	 * 从鼠标标处开始进行线条追踪，分为以下几种情况：
	 * A. 上一个演员对象为空，且当前演员对象也为空
	 *    - 不执行任何操作
	 * B. 上一个演员对象为空，且当前演员对象有效
	 *    - 高亮当前演员对象
	 * C. 上一个演员对象有效，且当前演员对象为空
	 *    - 取消高亮上一个演员对象
	 * D. 两个演员对象都有效，但二者不是同一个对象
	 *    - 取消高亮上一个演员对象，同时高亮当前演员对象
	 * E. 两个演员对象都有效，且二者是同一个对象
	 *    - 不执行任何操作
	 */
	
	if (!LastActor && !ThisActor)
	{
		// 情况A: 两者都为空，不执行任何操作
		return;
	}
	if (!LastActor && ThisActor)
	{
		// 情况B: 上一个为空，当前有效，高亮当前
		ThisActor->HighlightActor();
	}
	else if (LastActor && !ThisActor)
	{
		// 情况C: 上一个有效，当前为空，取消高亮上一个
		LastActor->UnHighlightActor();
	}
	else if (LastActor && ThisActor && LastActor != ThisActor)
	{
		// 情况D: 两者都有效但不同，切换高亮
		LastActor->UnHighlightActor();
		ThisActor->HighlightActor();
	}
	// 情况E: 两者相同，不执行任何操作
}

void AAuraPlayerController::BeginPlay()
{
	Super::BeginPlay();
	check(AuraContext);
	
	// 获取增强输入本地玩家子系统，用于管理增强输入映射和绑定 
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	
	check(Subsystem);
	// 添加映射上下文到本地玩家子系统
	Subsystem->AddMappingContext(AuraContext,0);
		
	bShowMouseCursor = true;// 显示鼠标光标,用于俯视角
	DefaultMouseCursor = EMouseCursor::Default;// 鼠标光标样式
	
	// 配置输入模式为游戏和UI混合模式，允许鼠标自由移动并在捕获时保持光标可见
	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputModeData.SetHideCursorDuringCapture(false);
	SetInputMode(InputModeData);
}

void AAuraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	// 将 InputComponent 强制转换为增强输入组件类型
	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);
	
	EnhancedInputComponent->BindAction(MoveAction,ETriggerEvent::Triggered,this,&AAuraPlayerController::Move);
	EnhancedInputComponent->BindAction(ZoomAction,ETriggerEvent::Triggered,this,&AAuraPlayerController::Zoom);
	
}

void AAuraPlayerController::Move(const FInputActionValue& InputActionValue){
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();

	// 获取控制旋转并提取偏航角，忽略俯仰和翻滚
	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0.f,Rotation.Yaw,0.f);
	
	// 根据偏航角计算前方向和右方向单位向量
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	// 将输入轴值转换为世界空间移动输入应用到被控制的Pawn
	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		ControlledPawn->AddMovementInput(ForwardDirection,InputAxisVector.Y);
		ControlledPawn->AddMovementInput(RightDirection,InputAxisVector.X);
	}
}

void AAuraPlayerController::Zoom(const FInputActionValue& InputActionValue)
{
	// 鼠标滚轮缩放弹簧臂长度
	const float ZoomValue = InputActionValue.Get<float>();
	
	if (AAuraCharacter* AuraCharacter = Cast<AAuraCharacter>(GetCharacter()))
	{
		if (USpringArmComponent* SpringArm = AuraCharacter->FindComponentByClass<USpringArmComponent>())
		{
			SpringArm->TargetArmLength += ZoomValue * 100.f;
			SpringArm->TargetArmLength = FMath::Clamp(SpringArm->TargetArmLength, 200.f, 1000.f);
		}
	}
}

