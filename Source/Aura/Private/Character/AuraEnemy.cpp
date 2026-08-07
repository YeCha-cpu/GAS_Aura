// Copyright GYQ

#include "Character/AuraEnemy.h"
#include "GAS/AuraAbilitySystemComponent.h"
#include "GAS/AS/AuraAttributeSet.h"
#include "Aura/Aura.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "UI/Widget/AuraUserWidget.h"


AAuraEnemy::AAuraEnemy()
{
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);

	// 创建技能系统组件
	AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);// 技能系统组件是多客户端同步的
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);// 设置 敌人ASC 的复制模式为 Minimal
	
	// 创建属性集
	AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>("Attributes");
	
	// 创建生命值条
	HealthBar = CreateDefaultSubobject<UWidgetComponent>("HealthBar");
	HealthBar->SetupAttachment(GetRootComponent());
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
	
	// 初始化GAS能力系统所需的Actor基础信息（OwnerActor、AvatarActor绑定）
	InitAbilityActorInfo();
	
	// 为生命值条设置Widget控制器
	if (UAuraUserWidget* AuraUserWidget = Cast<UAuraUserWidget>(HealthBar->GetUserWidgetObject()))
	{
		AuraUserWidget->SetWidgetController(this);
	}
	
	if (const UAuraAttributeSet* AuraAS = Cast<UAuraAttributeSet>(AttributeSet))
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAS->GetHealthAttribute()).
		AddLambda([this](const FOnAttributeChangeData& Data)
			{
				// 向外广播自定义血量更新事件，把最新血量值抛出去
				// 外部C++/蓝图可绑定OnHealthChanged接收血量变动
				OnHealthChanged.Broadcast(Data.NewValue);
			}
		);
		
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAS->GetMaxHealthAttribute()).
		AddLambda([this](const FOnAttributeChangeData& Data)
			{
				// 广播最大血量变更事件，推送最新最大生命值
				OnMaxHealthChanged.Broadcast(Data.NewValue);
			}
		);
		
		// 初始化血量条显示
		OnHealthChanged.Broadcast(AuraAS->GetHealth());
		OnMaxHealthChanged.Broadcast(AuraAS->GetMaxHealth());
		
	}
}

void AAuraEnemy::InitAbilityActorInfo()
{
	// 初始化技能系统组件
	AbilitySystemComponent->InitAbilityActorInfo(this,this);
	
	// 绑定 应用GE后 触动的委托回调
	Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();
	
	InitializeDefaultAttributes();
}
