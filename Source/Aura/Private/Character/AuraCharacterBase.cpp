// Copyright GYQ

#include "Character/AuraCharacterBase.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"

AAuraCharacterBase::AAuraCharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;

	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");
	Weapon->SetupAttachment(GetMesh(),FName("WeaponHandSocket"));
	Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

UAbilitySystemComponent* AAuraCharacterBase::GetAbilitySystemComponent() const
{
	return ASC;
}

void AAuraCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

