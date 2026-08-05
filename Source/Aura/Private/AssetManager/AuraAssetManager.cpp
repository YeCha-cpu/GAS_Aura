// Copyright GYQ


#include "AssetManager/AuraAssetManager.h"
#include "Engine/Engine.h"
#include "GAS/GT/AuraGameplayTags.h"

UAuraAssetManager& UAuraAssetManager::Get()
{
	check(GEngine)
	
	// 从全局引擎中获取当前的 AssetManager 基类指针。GEngine->AssetManager 返回的是 UAssetManager*（父类指针），所以这里 Cast为 UAuraAssetManager*
	UAuraAssetManager* AuraAssetManager = Cast<UAuraAssetManager>(GEngine->AssetManager);
	
	// 返回解引用后的引用
	return *AuraAssetManager;
	
}

void UAuraAssetManager::StartInitialLoading()
{
	// UAssetManager（父类）在里面做了大量关键工作：比如扫描项目设置里的 Primary Asset Types（主资产类型）、初始化扫描规则等。
	Super::StartInitialLoading();
	
	// 在引擎资源扫描完但还没开始加载具体资源时，调用自定义标签 AuraGameplayTags 的初始化函数 （InitNativeGameplayTags()）
	// 这么做保证在游戏的任何蓝图或 C++ 代码尝试使用 GameplayTag 之前，这些标签已经被注册进引擎的全局标签管理器了。
	FAuraGameplayTags::InitNativeGameplayTags();
}
