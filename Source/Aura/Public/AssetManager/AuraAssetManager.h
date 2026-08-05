// Copyright GYQ

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "AuraAssetManager.generated.h"

/**
 * 自定义的【全局资产管理器】，在 DefaultEngine.ini 里设置全局资产管理器
 * 继承自 UAssetManager，用于接管引擎全局的资产加载/卸载逻辑。
 * 通常用于处理 Primary Asset（主资产）的异步加载、分包（Chunk）管理以及资源引用追踪。
 */
UCLASS()
class AURA_API UAuraAssetManager : public UAssetManager
{
	GENERATED_BODY()
	
public:
	// 全局单例访问接口返回当前引擎正在使用的 AssetManager 实例，并将其强制转换为【自定义类型】。外部调用：UAuraAssetManager::Get().LoadPrimaryAsset(...)
	static UAuraAssetManager& Get();
	
	// 重写父类 UAssetManager 的虚函数，这个函数会在引擎启动时、资源管理系统准备开始加载主资产（Primary Assets）之前被自动调用
	virtual void StartInitialLoading() override;
	
};
