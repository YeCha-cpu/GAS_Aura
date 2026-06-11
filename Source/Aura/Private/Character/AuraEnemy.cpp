// Copyright GYQ

#include "Character/AuraEnemy.h"
#include "GAS/AuraAbilitySystemComponent.h"
#include "GAS/AS/AuraAttributeSet.h"
#include "Aura/Aura.h"


AAuraEnemy::AAuraEnemy()
{
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);

	// 创建技能系统组件
	ASC = CreateDefaultSubobject<UAuraAbilitySystemComponent>("AbilitySystemComponent");
	ASC->SetIsReplicated(true);// 技能系统组件是多客户端同步的
	ASC->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);// 设置 敌人ASC 的复制模式为 Minimal
	
	// 创建属性集
	AS = CreateDefaultSubobject<UAuraAttributeSet>("Attributes");
}

void AAuraEnemy::HighlightActor()
{
	// 开启渲染自定义深度
	GetMesh()->SetRenderCustomDepth(true);
	GetMesh()->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
	Weapon->SetRenderCustomDepth(true);
	Weapon->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
}

void AAuraEnemy::UnHighlightActor()
{
	// 关闭渲染自定义深度
	GetMesh()->SetRenderCustomDepth(false);
	Weapon->SetRenderCustomDepth(false);
}

void AAuraEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	// 初始化技能系统组件
	ASC->InitAbilityActorInfo(this,this);
}
