// GYQ

#include "AuraAbilityTypes.h"
#include "Engine/HitResult.h"


// 重写 GameplayEffectContext 的网络序列化函数
// 目的：手动控制结构体如何通过网络传输，只打包必要的数据，节省网络带宽，并支持 TSharedPtr 等特殊类型
bool FAuraGameplayEffectContext::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	// 1. 定义位掩码（RepBits）：用于标记哪些字段被有效数据占据
	// 使用 uint32 的每一个 bit 代表一个字段，最多支持 32 个字段
	uint32 RepBits = 0;
	
	// -------------------- 序列化阶段（Saving / 写入） --------------------
	// 当 Ar.IsSaving() 为 true 时，表示正在将数据打包发送（通常发生在服务器端）
	if (Ar.IsSaving())
	{
		// Bit 0: 检查 Instigator（发起者）是否有效且需要复制
		if (bReplicateInstigator && Instigator.IsValid())
		{
			RepBits |= 1 << 0; // 将该位置 1
		}
		// Bit 1: 检查 EffectCauser（效果原因者）是否有效且需要复制
		if (bReplicateEffectCauser && EffectCauser.IsValid() )
		{
			RepBits |= 1 << 1;
		}
		// Bit 2: 检查 AbilityCDO（能力类默认对象）是否有效
		if (AbilityCDO.IsValid())
		{
			RepBits |= 1 << 2;
		}
		// Bit 3: 检查 SourceObject（源对象）是否有效且需要复制
		if (bReplicateSourceObject && SourceObject.IsValid())
		{
			RepBits |= 1 << 3;
		}
		// Bit 4: 检查 Actors 数组是否包含元素
		if (Actors.Num() > 0)
		{
			RepBits |= 1 << 4;
		}
		// Bit 5: 检查 HitResult（命中结果）共享指针是否有效
		if (HitResult.IsValid())
		{
			RepBits |= 1 << 5;
		}
		// Bit 6: 检查是否拥有世界原点坐标
		if (bHasWorldOrigin)
		{
			RepBits |= 1 << 6;
		}
		// Bit 7: 检查是否为格挡命中（自定义布尔变量）
		if (bIsBlockedHit)
		{
			RepBits |= 1 << 7;
		}
		// Bit 8: 检查是否为暴击命中（自定义布尔变量）
		if (bIsCriticalHit)
		{
			RepBits |= 1 << 8;
		}
	}
	
	// 2. 将位掩码本身序列化（写入/读取）
	// 第二个参数 9 表示我们使用了 9 个 bit（位 0 到 8），告知序列化器需要读写多少位
	Ar.SerializeBits(&RepBits, 9);
	
	// -------------------- 根据位掩码进行条件序列化 --------------------
	// 注意：以下代码同时适用于 发送（Saving） 和 接收（Loading） 两端
	// 发送端根据掩码写入数据，接收端根据掩码读取数据，保证了同步

	// 如果 Bit 0 被标记，读写 Instigator
	if (RepBits & (1 << 0))
	{
		Ar << Instigator;
	}
	// 如果 Bit 1 被标记，读写 EffectCauser
	if (RepBits & (1 << 1))
	{
		Ar << EffectCauser;
	}
	// 如果 Bit 2 被标记，读写 AbilityCDO
	if (RepBits & (1 << 2))
	{
		Ar << AbilityCDO;
	}
	// 如果 Bit 3 被标记，读写 SourceObject
	if (RepBits & (1 << 3))
	{
		Ar << SourceObject;
	}
	// 如果 Bit 4 被标记，安全序列化 Actors 数组（限制最大数量为 31，防止溢出攻击）
	if (RepBits & (1 << 4))
	{
		SafeNetSerializeTArray_Default<31>(Ar, Actors);
	}
	// 如果 Bit 5 被标记，处理 HitResult（共享指针的特殊处理）
	if (RepBits & (1 << 5))
	{
		// 接收端特殊处理：因为 HitResult 是 TSharedPtr，默认是空指针
		// 如果确定要接收数据，必须预先 new 出一个 FHitResult 实例，否则无法存储反序列化出的数据
		if (Ar.IsLoading())
		{
			if (!HitResult.IsValid())
			{
				HitResult = TSharedPtr<FHitResult>(new FHitResult());
			}
		}
		// 调用 FHitResult 自带网络序列化函数
		HitResult->NetSerialize(Ar, Map, bOutSuccess);
	}
	// 如果 Bit 6 被标记，读写世界原点，并标记 bHasWorldOrigin 为 true
	if (RepBits & (1 << 6))
	{
		Ar << WorldOrigin;
		bHasWorldOrigin = true;
	}
	else
	{
		// 如果 Bit 6 未被标记，确保接收端（Loading）的 bHasWorldOrigin 为 false
		// 注意：这行代码在 Saving 阶段也会执行，但由于 Saving 时 bHasWorldOrigin 已由业务逻辑决定，这里主要是为了 Loading 时重置默认状态
		bHasWorldOrigin = false;
	}
	// 如果 Bit 7 被标记，读写 bIsBlockedHit
	if (RepBits & (1 << 7))
	{
		Ar << bIsBlockedHit;
	}
	// 如果 Bit 8 被标记，读写 bIsCriticalHit
	if (RepBits & (1 << 8))
	{
		Ar << bIsCriticalHit;
	}

	// -------------------- 反序列化后处理（Post-Loading） --------------------
	// 当数据接收完成（Loading）后，调用 AddInstigator 来初始化内部关联关系
	// 例如：根据 Instigator 和 EffectCauser 重新构建 InstigatorAbilitySystemComponent（ASC）
	// 因为网络传输只传了 Actor 指针，需要这个函数来刷新缓存的 ASC 引用
	if (Ar.IsLoading())
	{
		AddInstigator(Instigator.Get(), EffectCauser.Get()); 
	}	
	
	// 标记序列化过程成功
	bOutSuccess = true;
	return true;
}
