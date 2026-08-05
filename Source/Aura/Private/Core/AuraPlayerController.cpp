// Copyright GYQ

#include "Core/AuraPlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "DrawDebugHelpers.h"
#include "EnhancedInputSubsystems.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "GameFramework/SpringArmComponent.h"
#include "Interface/EnemyInterface.h"
#include "Character/AuraCharacter.h"
#include "Components/SplineComponent.h"
#include "Engine/Engine.h"
#include "Engine/LocalPlayer.h"
#include "GAS/AuraAbilitySystemComponent.h"
#include "GAS/GT/AuraGameplayTags.h"
#include "Input/AuraInputComponent.h"

AAuraPlayerController::AAuraPlayerController()
{
	// 打开网络复制
	bReplicates = true;
	
	Spline = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
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
	ThisActor = TScriptInterface<IEnemyInterface>(CursorHit.GetActor());
	
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
	if (Subsystem)
	{
		// 添加映射上下文到本地玩家子系统
        Subsystem->AddMappingContext(AuraContext,0);
	}
	
	bShowMouseCursor = true;// 显示鼠标光标,用于俯视角
	DefaultMouseCursor = EMouseCursor::Default;// 鼠标光标样式
	
	// 配置输入模式为游戏和UI混合模式，允许鼠标自由移动并在捕获时保持光标可见
	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputModeData.SetHideCursorDuringCapture(false);
	// 设置输入模式
	SetInputMode(InputModeData);
	
}

void AAuraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();	
	
	// 将 InputComponent 强制转换为增强输入组件类型
	UAuraInputComponent* AuraInputComponent = CastChecked<UAuraInputComponent>(InputComponent);
	
	AuraInputComponent->BindAbilityActions(InputConfig, this, &ThisClass::AbilityInputTagPressed, &ThisClass::AbilityInputTagReleased, &ThisClass::AbilityInputTagHeld);
	AuraInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered,this, &AAuraPlayerController::Move);
	AuraInputComponent->BindAction(ZoomAction, ETriggerEvent::Triggered,this, &AAuraPlayerController::Zoom);
	
}

void AAuraPlayerController::Move(const FInputActionValue& InputActionValue)
{
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

UAuraAbilitySystemComponent* AAuraPlayerController::GetASC()
{
	if (AuraASC ==  nullptr)
	{
		AuraASC = Cast<UAuraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn<APawn>()));
	}
	return AuraASC;
}

void AAuraPlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))	// 如果输入标签为左键
	{
		bTargeting = ThisActor ? true : false;	// 获取当前演员对象，如果存在则表示正在瞄准敌人
		bAutoRunning = false;	// 停止自动移动
	}
	
}

// 根据是否处于“瞄准/锁定目标”状态（bTargeting），决定是移动角色还是施放技能。
void AAuraPlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{
	// 如果按住的不是左键（LMB），直接交给 ASC 去尝试激活技能
	if (!InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
	{
		if (GetASC())
		{
			GetASC()->AbilityInputTagHeld(InputTag);
		}
		return;
	}
    
	/** 以下是左键按住时的逻辑 */
	if (bTargeting)   // 如果处于 瞄准/目标选中 状态（鼠标落在敌人上 ）
	{
		// 同样交给 ASC 去激活技能（此时左键视为技能触发键）
		if (GetASC())
		{
			GetASC()->AbilityInputTagHeld(InputTag);
		}
	}
	else   // 非瞄准状态，左键作为移动键
	{
		// 累计按住时长（可用于区分“点击”和“长按”）
		FollowTime += GetWorld()->GetDeltaSeconds();
        
		// 每帧获取鼠标下的地面位置（射线检测）
		FHitResult CursorHit;
		if (GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, false, CursorHit))
		{
			CacheDestination = CursorHit.ImpactPoint;
		}
        
		// 让角色朝鼠标位置移动
		if (APawn* ControlledPawn = GetPawn())
		{
			const FVector WorldLocation = (CacheDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
			ControlledPawn->AddMovementInput(WorldLocation);
		}
	}
}

// 根据 FollowTime（按住时长）决定是否触发自动寻路（短按视为点击移动），同时处理技能释放的结束
void AAuraPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	// 非左键的按键松开 → 直接通知 ASC 执行「释放」逻辑（用于中断持续技能）
	if (!InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
	{
		if (GetASC())
		{
			GetASC()->AbilityInputTagReleased(InputTag);
		}
		return;
	}
	
	if (bTargeting)
	{
		// 如果处于瞄准状态, 则尝试释放技能
		if (GetASC())
		{
			GetASC()->AbilityInputTagReleased(InputTag);
		}
	}
	else
	{
		// 非瞄准状态（移动模式）
		APawn* ControlledPawn = GetPawn();
		if (!ControlledPawn)
		{
			UE_LOG(LogTemp, Error, TEXT("AbilityInputTagHeld: No controlled pawn!"));
			return;
		}
		
		// 短按（小于阈值）且角色存在 → 发起【自动寻路】
		if (FollowTime <= ShortPressThreshold && ControlledPawn)
		{
			// 同步寻路到 CacheDestination（鼠标点击位置）
			if (UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(this, ControlledPawn->GetActorLocation(), CacheDestination))
			{
				// 将路径点添加到样条线（Spline）用于显示或跟随
				Spline->ClearSplinePoints();	// 先清空样条线
				
				// 遍历每个同步寻路的路径点，添加到样条线中
				for (const FVector& PointLoc : NavPath->PathPoints)
				{
					Spline->AddSplinePoint(PointLoc, ESplineCoordinateSpace::World);
					DrawDebugSphere(GetWorld(), PointLoc, 10.f, 12, FColor::Green, false, 10.f);
				}
				bAutoRunning = true;   // 开始自动沿着路径移动
			}
		}
		FollowTime = 0.f;	// 重置按住时长
		bTargeting = false;	// 停止瞄准
	}
}


