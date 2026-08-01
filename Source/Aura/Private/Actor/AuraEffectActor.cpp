// Copyright GYQ

#include "Actor/AuraEffectActor.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Components/SphereComponent.h"
#include "GAS/AS/AuraAttributeSet.h"

AAuraEffectActor::AAuraEffectActor()
{
	PrimaryActorTick.bCanEverTick = false;
	
	SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot")));
}

void AAuraEffectActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// TODO: 应用 GE 函数：
void AAuraEffectActor::ApplyEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> GameplayEffectClass)
{
	// 使用 能力系统蓝图库 获取目标Actor的 能力系统组件（内部已经判断是否继承自 IAbilitySystemInterface）
	UAbilitySystemComponent* TargetActorASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if(TargetActorASC ==  nullptr) return;
	check(GameplayEffectClass);
	
	// 1、创建游戏效果上下文 [用于记录这个 GE 的来源、触发者、命中位置、伤害类型等运行时数据，供后续数值计算、逻辑判定使用]
	FGameplayEffectContextHandle EffectContextHandle = TargetActorASC->MakeEffectContext();
	// 2、给游戏效果上下文添加源对象 [将当前这个AAuraEffectActor自身标记为效果的源对象。后续 GE 执行时，可以通过上下文追溯到效果是哪个 Actor 产生的]
	EffectContextHandle.AddSourceObject(this);
	// 3、创建游戏效果规格 [GameplayEffect 是配置资产（定义了 Modifier、持续时间、标签等规则），不能直接施加到角色身上；必须生成FGameplayEffectSpec（效果规格）这个运行时实例，才能实际应用]
	const FGameplayEffectSpecHandle EffectSpecHandle = TargetActorASC->MakeOutgoingSpec(GameplayEffectClass, ActorLevel, EffectContextHandle);
	// 4、应用游戏效果 [EffectSpecHandle.Data.Get() 是从句柄中取出实际的FGameplayEffectSpec的原始指针，再解引用后传入]
	const FActiveGameplayEffectHandle ActiveGameplayEffectHandle = TargetActorASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
	
	// 从句柄中取出实际的FGameplayEffectSpec的原始指针 再拿到此原始指针对应的原始配置资产 再取出 UGameplayEffect 的原始裸指针 再访问资产里的定义效果的时长类型，
	// 判断 定义效果的时长类型 是否是无限效果
	const bool bIsInfinite = EffectSpecHandle.Data.Get()->Def.Get()->DurationPolicy == EGameplayEffectDurationType::Infinite;
	
	// 如果是无限效果 ，并且无限效果的移除策略是移除在结束重叠时，我们需要手动将它移除。则将此 效果句柄 和 目标ActorASC 添加到 ActiveEffectHandles键值 中
	if (bIsInfinite && InfinitGameplayEffectRemovalPolicy == EEffectRemovalPolicy::RemoveOnEndOverlap)
	{
		ActiveEffectHandles.Add(ActiveGameplayEffectHandle, TargetActorASC);
	}

}

void AAuraEffectActor::OnOverlap(AActor* TargetActor)
{
	// 开始重叠时应用GE
	if (InstantGameplayEffectPolicy == EEffectApplcationPolicy::ApplyOnOverlap)
	{
		ApplyEffectToTarget(TargetActor, InstantGameplayEffectClass);
	}
	if (DurationGameplayEffectPolicy == EEffectApplcationPolicy::ApplyOnOverlap)
	{
		ApplyEffectToTarget(TargetActor, DurationGameplayEffectClass);
	}
	if (InfiniteGameplayEffectPolicy == EEffectApplcationPolicy::ApplyOnOverlap)
	{
		ApplyEffectToTarget(TargetActor, InfiniteGameplayEffectClass);
	}
}

void AAuraEffectActor::OnEndOverlap(AActor* TargetActor)
{
	// 离开重叠时应用GE
	if (InstantGameplayEffectPolicy == EEffectApplcationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, InstantGameplayEffectClass);
	}
	if (DurationGameplayEffectPolicy == EEffectApplcationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, DurationGameplayEffectClass);
	}
	if (InfiniteGameplayEffectPolicy == EEffectApplcationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, InfiniteGameplayEffectClass);
	}
	
	// 离开重叠时移除无限GE
	if (InfinitGameplayEffectRemovalPolicy == EEffectRemovalPolicy::RemoveOnEndOverlap)
	{
		// 获取目标角色的 技能系统组件，以匹配 ActiveEffectHandles 键值对中的值，用来移除效果
		UAbilitySystemComponent* TargetActorASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
		if (!IsValid(TargetActorASC)) return;
		
		// 将要移除的句柄保存在 HandlesToRemove 数组中，遍历完成后再删除
		TArray<FActiveGameplayEffectHandle> HandlesToRemove;
		
		// 遍历 ActiveEffectHandles 键值对，判断当前目标Actor的ASC 是否是 ActiveEffectHandles 键值对中的值【注意：不能在遍历的同时删除元素，可能导致崩溃！】
		for (auto HandlePair : ActiveEffectHandles)
		{
			if (TargetActorASC == HandlePair.Value)
			{
				// 移除句柄对应的效果,每次只移除一层效果堆叠
				TargetActorASC->RemoveActiveGameplayEffect(HandlePair.Key, 1);
				HandlesToRemove.Add(HandlePair.Key);
			}
		}
		// 遍历 HandlesToRemove 数组，将 HandlesToRemove 数组中的句柄从 ActiveEffectHandles 键值对中删除
		for (FActiveGameplayEffectHandle& RemoveHandle : HandlesToRemove)
		{
			ActiveEffectHandles.FindAndRemoveChecked(RemoveHandle);
		}
	}
}



