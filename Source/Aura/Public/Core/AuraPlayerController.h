// Copyright GYQ

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "AuraPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class IEnemyInterface;

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

	
	/* ------------------------------输入回调函数------------------------------ */
	void Move(const FInputActionValue& InputActionValue);
	void Zoom(const FInputActionValue& InputActionValue);
	
	void CursorTrace();
	IEnemyInterface* LastActor;
	IEnemyInterface* ThisActor;
};
