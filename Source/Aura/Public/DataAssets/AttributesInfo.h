// Copyright GYQ

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "AttributesInfo.generated.h"

/**
 * 【结构体】单个属性的 UI 展示信息：
 * 
 * 用于在编辑器中配置每个属性（如力量、护甲）的显示数据，
 * 包括对应的 GameplayTag、名称、描述和当前的数值。
 * 
 * 设计意图：
 * - 将“属性的游戏逻辑标识（Tag）”与“UI 展示信息（文本）”解耦。
 * - 方便策划/美术在数据资产中统一配置，无需修改代码。
 * - 支持运行时通过 Tag 查找对应的显示信息，用于 UI 绑定或调试日志。
 */
USTRUCT(BlueprintType)
struct FAuraAttributeInfo
{
	GENERATED_BODY()
	
	// -------------------- 编辑器配置字段（只读） --------------------
	
	/** 属性的唯一标识符（GameplayTag），必须与 GAS 中使用的标签一致 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag AttributeTag = FGameplayTag();
	
	/** 属性在 UI 中显示的名称（例如："护甲"、"力量"） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText AttributeName = FText();
	
	/** 属性在 UI 中的描述文本（例如："减少受到的伤害，提高格挡几率"） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText AttributeDescription = FText();
	
	// -------------------- 运行时字段（可读，不可在编辑器中配置） --------------------
	
	/** 属性的当前数值，由游戏逻辑动态更新，不在数据资产中存储初始值 */
	UPROPERTY(BlueprintReadOnly)
	float AttributeValue = 0.f;
};

/**
 * 【数据资产】属性信息集合：
 * 继承自 UDataAsset，作为一个可在内容浏览器中创建和编辑的资产文件。
 * 用于集中配置游戏中所有属性的显示信息。
 *
 * 优点：
 * 1. 策划无需打开 C++ 代码即可增删改属性显示数据。
 * 2. 数据资产可被多个系统引用（如 UI、战斗日志），保证数据一致性。
 * 3. 支持热更新（修改资产后无需重新编译代码）。
 *
 * 使用方式：
 * - 在内容浏览器中右键 -> 数据资产 -> 选择 UAttributesInfo 类创建资产。
 * - 在资产中配置 AttributeInfomation 数组，填入每个属性的 Tag、Name、Description。
 * - 在需要显示属性的 UI 或逻辑中，调用 FindAttributeInfoByTag 根据 Tag 获取对应的结构体。
 */
UCLASS()
class AURA_API UAttributesInfo : public UDataAsset
{
	GENERATED_BODY()
	
public:
	/** 所有属性的信息数组，在【蓝图中的数据资产编辑器】中配置 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes Info")
	TArray<FAuraAttributeInfo> AttributeInformation;
	
	/**
	 * 【查找函数】通过 GameplayTag 精确匹配对应的属性信息
	 * 
	 * @param AttributeTag 要查找的属性标签（来自 GAS 系统）
	 * @param bLogNotFound 若为 true，当未找到匹配项时在日志中输出错误信息，便于调试
	 * @return 匹配的 FAuraAttributeInfo 结构体；若未找到，返回一个默认构造的空结构体（所有字段为空/0）
	 */
	UFUNCTION(BlueprintCallable, Category = "AttributesInfo")
	FAuraAttributeInfo FindAttributeInfoByTag(const FGameplayTag& AttributeTag, bool bLogNotFound = false) const;
	

	
};
