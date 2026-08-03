// Copyright GYQ

#include "DataAssets/AttributesInfo.h"

FAuraAttributeInfo UAttributesInfo::FindAttributeInfoByTag(const FGameplayTag& AttributeTag, bool bLogNotFound) const
{
	// 遍历数组中所有配置的属性信息
	for (const FAuraAttributeInfo& Info : AttributeInformation)
	{
		// 使用 MatchesTagExact 进行精确匹配（标签必须完全一致）
		// 注意：如果需要父子层级匹配，可改为 MatchesTag，但这里用精确匹配更安全，避免歧义
		if (Info.AttributeTag.MatchesTagExact(AttributeTag))
		{
			// 找到匹配项，直接返回（拷贝一份结构体）
			return Info;
		}
	}
	
	// 未找到匹配项，根据 bLogNotFound 决定是否输出错误日志
	if (bLogNotFound)
	{
		UE_LOG(LogTemp, Error, TEXT("Can't find AttributeInfo with tag [%s] on AttributesInfo [%s]"), 
			*AttributeTag.ToString(), 
			*GetNameSafe(this)  // 获取当前数据资产的名称，便于定位是哪个资产配置漏了
		);
	}
	
	// 返回一个默认构造的空结构体，调用方应检查返回值是否有效
	return FAuraAttributeInfo();
}