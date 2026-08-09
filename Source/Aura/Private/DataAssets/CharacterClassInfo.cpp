// Copyright GYQ


#include "DataAssets/CharacterClassInfo.h"


FCharacterClassDefaultInfo UCharacterClassInfo::GetClassDefaultInfo(ECharacterClass CharacterClass)
{
	return CharacterClassDefaultInfo.FindChecked(CharacterClass);
}
