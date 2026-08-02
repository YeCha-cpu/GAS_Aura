// Copyright GYQ

#include "Character/AuraEnemy.h"
#include "GAS/AuraAbilitySystemComponent.h"
#include "GAS/AS/AuraAttributeSet.h"
#include "Aura/Aura.h"
#include "Components/SkeletalMeshComponent.h"


AAuraEnemy::AAuraEnemy()
{
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);

	// 创建技能系统组件
	AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);// 技能系统组件是多客户端同步的
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);// 设置 敌人ASC 的复制模式为 Minimal
	
	// 创建属性集
	AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>("Attributes");
}

void AAuraEnemy::HighlightActor()
{
	// 开启 角色 和 武器 的渲染自定义深度，用于高亮
	GetMesh()->SetRenderCustomDepth(true);
	GetMesh()->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
	Weapon->SetRenderCustomDepth(true);
	Weapon->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
}

void AAuraEnemy::UnHighlightActor()
{
	// 关闭 角色 和 武器 的渲染自定义深度
	GetMesh()->SetRenderCustomDepth(false);
	Weapon->SetRenderCustomDepth(false);
}

int32 AAuraEnemy::GetPlayerLevel()
{
	return Level;
}

void AAuraEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	InitAbilityActorInfo();
}

void AAuraEnemy::InitAbilityActorInfo()
{
	// 初始化技能系统组件
	AbilitySystemComponent->InitAbilityActorInfo(this,this);
	
	// 绑定 应用GE后 触动的委托回调
	Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();
}
