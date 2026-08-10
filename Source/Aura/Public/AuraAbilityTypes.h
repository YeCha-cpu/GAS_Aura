// GYQ

#pragma once

#include "GameplayEffectTypes.h"
#include "AuraAbilityTypes.generated.h"

/*
 * FAuraGameplayEffectContext::NetSerialize 是一个自定义网络序列化函数，
 * 用于在多人游戏中精确控制 GameplayEffectContext 结构体如何通过网络发送。
 * 之所以需要手动实现，是因为该结构体包含了一些非标准类型（如 TSharedPtr<FHitResult>、TArray<AActor*> 等），且我们希望按需序列化——只发送当前需要同步的字段，以节省带宽。
 */
USTRUCT(BlueprintType)
struct FAuraGameplayEffectContext : public FGameplayEffectContext
{
	GENERATED_BODY()

public:
	bool IsBlockedHit() const { return bIsBlockedHit; }
	bool IsCriticalHit() const { return bIsCriticalHit; }
	
	void SetIsCriticalHit(bool bInIsCriticalHit) { bIsCriticalHit = bInIsCriticalHit; }
	void SetIsBlockedHit(bool bInIsBlockedHit) { bIsBlockedHit = bInIsBlockedHit; }
	
	// 注意，这里应该直接return StaticStruct（）；作者这里会返回基类的EffectContext，会导致持续性GE的网络同步出现问题
	/** Returns the actual struct used for serialization, subclasses must override this! */
	virtual UScriptStruct* GetScriptStruct() const override
	{
		return StaticStruct();
	}
	
	/** Creates a copy of this context, used to duplicate for later modifications */
	virtual FAuraGameplayEffectContext* Duplicate() const
	{
		FAuraGameplayEffectContext* NewContext = new FAuraGameplayEffectContext();
		*NewContext = *this;
		if (GetHitResult())
		{
			// Does a deep copy of the hit result
			NewContext->AddHitResult(*GetHitResult(), true);
		}
		return NewContext;
	}

	/** Custom serialization, subclasses must override this */
	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess) override;
	
protected:
	UPROPERTY()
	bool bIsBlockedHit = false; // 是否是阻挡击中
	
	UPROPERTY()
	bool bIsCriticalHit = false; // 是否是暴击

};

template<>
struct TStructOpsTypeTraits<FAuraGameplayEffectContext> : public TStructOpsTypeTraitsBase2<FAuraGameplayEffectContext>
{
	enum
	{
		WithNetSerializer = true,	// 启用该结构体的自定义网络序列化器，让FAuraGameplayEffectContext可以通过网络 RPC 复制到客户端，必须配套实现FNetSerializer
		WithCopy = true		// 开启结构体的自定义拷贝函数，GAS 复制 Effect 上下文时会调用Copy()方法
	};
};
