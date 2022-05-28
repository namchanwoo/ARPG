#pragma once

#include "CoreMinimal.h"
#include "Datas/CombatData.h"
#include "Datas/ItemData.h"
#include "Datas/StatData.h"
#include "Kismet/KismetMathLibrary.h"


class ARPG_API CRPGLibrary
{
public:
	static FString GetStatTypeAsString(EStats EnumValue);
	static FString GetAttackTypesTypeAsString(EAttackTypes EnumValue);
	static FString GetCharacterActionTypeAsString(ECharacterAction EnumValue);
	static FString GetCombatTypeAsString(ECombatType EnumValue);
	static FString GetEquipSlotsTypeAsString(EEquipSlots EnumValue);
	static FString GetAttributesAsString(EAttributes EnumValue);


	template<typename T>
	static int32 GetNextArrayIndex(TArray<T> InArray,int32 Index,bool Forward)
	{
		int32 L_LastIndex = InArray.Num() - 1; //배열의 마지막 인덱스를 설정합니다
		
		int32 L_ForwardIndex = UKismetMathLibrary::SelectInt(Index + 1, 0, InArray.IsValidIndex(Index + 1));
		int32 L_BackIndex = UKismetMathLibrary::SelectInt(Index - 1, L_LastIndex, InArray.IsValidIndex(Index - 1));
		return UKismetMathLibrary::SelectInt(L_ForwardIndex, L_BackIndex, Forward);
	}
	
	static bool SelectBool(bool A, bool B, bool bSelectA)
	{
		return bSelectA ? A : B;
	}

	template<typename T>
	static T SelectStruct(T A, T B, bool bSelectA)
	{
		return bSelectA ? A : B;
	}

	
};


