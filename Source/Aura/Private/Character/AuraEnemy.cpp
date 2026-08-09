// Copyright GYQ

#include "Character/AuraEnemy.h"
#include "GAS/AuraAbilitySystemComponent.h"
#include "GAS/AS/AuraAttributeSet.h"
#include "Aura/Aura.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GAS/BluePrintFunctionLibrary/AuraGASLibrary.h"
#include "GAS/GA/AuraGameplayAbility.h"
#include "GAS/GT/AuraGameplayTags.h"
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

void AAuraEnemy::Die()
{
	SetLifeSpan(LifeSpan);
	Super::Die();
}

void AAuraEnemy::BeginPlay()
{
	Super::BeginPlay();
	GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
	InitAbilityActorInfo();	// 初始化GAS能力系统所需的Actor基础信息（OwnerActor、AvatarActor绑定）
	UAuraGASLibrary::GiveStartAbilities(this, AbilitySystemComponent);	// 赋予初始技能
	
	// 为生命值条设置Widget控制器
	if (UAuraUserWidget* AuraUserWidget = Cast<UAuraUserWidget>(HealthBar->GetUserWidgetObject()))
	{
		AuraUserWidget->SetWidgetController(this);
	}
	
	if (const UAuraAttributeSet* AuraAS = Cast<UAuraAttributeSet>(AttributeSet))
	{
		// 监听血量属性变化
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAS->GetHealthAttribute()).
		AddLambda([this](const FOnAttributeChangeData& Data)
			{
				// 向外广播自定义血量更新事件，把最新血量值抛出去
				// 外部C++/蓝图可绑定OnHealthChanged接收血量变动
				OnHealthChanged.Broadcast(Data.NewValue);
			}
		);
		// 监听最大血量属性变化
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAS->GetMaxHealthAttribute()).
		AddLambda([this](const FOnAttributeChangeData& Data)
			{
				// 广播最大血量变更事件，推送最新最大生命值
				OnMaxHealthChanged.Broadcast(Data.NewValue);
			}
		);
		
		// 注册受击标签监听（受击者的ASC组件 新增或完全移除受击标签（Effect_HitReact）时才会调用回调函数）
		AbilitySystemComponent->RegisterGameplayTagEvent(FAuraGameplayTags::Get().Effect_HitReact, EGameplayTagEventType::NewOrRemoved).
		AddUObject(this, &AAuraEnemy::HitReactTagChanged);
		
		// 初始化血量条显示
		OnHealthChanged.Broadcast(AuraAS->GetHealth());
		OnMaxHealthChanged.Broadcast(AuraAS->GetMaxHealth());
		
	}
}

// 受击标签数量发生变化时，设置bHitReacting，并阻止其移动 
void AAuraEnemy::HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	bHitReacting = NewCount > 0;
		
	GetCharacterMovement()->MaxWalkSpeed = bHitReacting ? 0.f : BaseWalkSpeed;
}

void AAuraEnemy::InitAbilityActorInfo()
{
	// 初始化技能系统组件
	AbilitySystemComponent->InitAbilityActorInfo(this,this);
	
	// 绑定 应用GE后 触动的委托回调
	Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();
	
	InitializeDefaultAttributes();
}

void AAuraEnemy::InitializeDefaultAttributes() const
{
	UAuraGASLibrary::InitializeDefaultAttributes(this, CharacterClass, Level, AbilitySystemComponent); 
}
