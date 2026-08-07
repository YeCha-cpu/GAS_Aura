// Copyright GYQ

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameplayTagContainer.h"
#include "Engine/HitResult.h"
#include "AuraPlayerController.generated.h"

class USplineComponent;
class USpringArmComponent;
struct FGameplayTag;
class UAuraInputConfig;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class IEnemyInterface;
class UAuraAbilitySystemComponent;

UCLASS()
class AURA_API AAuraPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AAuraPlayerController();
	virtual void PlayerTick(float DeltaTime) override;
	
protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	
private:
	/* ------------------------------IMC------------------------------ */
	UPROPERTY(EditAnywhere, Category = "Aura|Input")
	TObjectPtr<UInputMappingContext> AuraContext;
	
	/* ------------------------------IA------------------------------ */
	UPROPERTY(EditAnywhere, Category = "Aura|Input")
	TObjectPtr<UInputAction> MoveAction;
	
	UPROPERTY(EditAnywhere, Category = "Aura|Input")
	TObjectPtr<UInputAction> ZoomAction;
	
	UPROPERTY(EditAnywhere, Category = "Aura|Input")
	TObjectPtr<UInputAction> ShiftAction;
	
	// 输入标签配置 数据资产
	UPROPERTY(EditAnywhere, Category = "Aura|Input")
	TObjectPtr<UAuraInputConfig> InputConfig;
	
	/* ------------------------------输入回调函数------------------------------ */
	void Move(const FInputActionValue& InputActionValue);
	void Zoom(const FInputActionValue& InputActionValue);
	void AbilityInputTagPressed(FGameplayTag InputTag);
	void AbilityInputTagReleased(FGameplayTag InputTag);
	void AbilityInputTagHeld(FGameplayTag InputTag);
	void ShiftPressed() { bShiftKeyDown = true;}
	void ShiftReleased() { bShiftKeyDown = false;}
	
	bool bShiftKeyDown = false;
	
	// 鼠标追踪
	void CursorTrace();
	
	// TScriptInterface 是 UE 专门用来安全持有接口指针的智能指针类型，能正确处理垃圾回收和类型转换。
	TScriptInterface<IEnemyInterface> LastActor;
	TScriptInterface<IEnemyInterface> ThisActor;
	
	// 鼠标追踪击中点
	FHitResult CursorHit;
	
	UPROPERTY()
	TObjectPtr<UAuraAbilitySystemComponent> AuraASC;
	
	UFUNCTION(BlueprintCallable)
	UAuraAbilitySystemComponent* GetASC();
	
	/** 点击移动相关 */
	FVector CacheDestination = FVector::ZeroVector;		// 存放鼠标点击的世界位置 
	float FollowTime = 0;	// 按住鼠标使人物跟随鼠标位置的所需时间 
	float ShortPressThreshold = 0.5f;	// 短按阈值
	bool bAutoRunning = false;	// 是否自动移动
	bool bTargeting = false;	// 是否正在瞄准敌人
	
	UPROPERTY(EditDefaultsOnly)
	float AutoRunAcceptanceRadius = 50.f;	// 自动移动的容差
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USplineComponent> Spline;
	
	void AutoRun();
};
