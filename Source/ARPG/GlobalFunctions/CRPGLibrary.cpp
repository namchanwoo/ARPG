#include "GlobalFunctions/CRPGLibrary.h"
#include "Kismet/KismetMathLibrary.h"


FString CRPGLibrary::GetStatTypeAsString(EStats EnumValue)
{
	const UEnum* enumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("EStats"), true);
	if (!enumPtr)
		return FString(TEXT("Invalid NoEnum"));

	FString L_Name = enumPtr->GetNameStringByIndex(static_cast<int32>(EnumValue));
	return L_Name;
}

FString CRPGLibrary::GetAttackTypesTypeAsString(EAttackTypes EnumValue)
{
	const UEnum* enumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("EAttackTypes"), true);
	if (!enumPtr)
		return FString(TEXT("Invalid NoEnum"));

	FString L_Name = enumPtr->GetNameStringByIndex(static_cast<int32>(EnumValue));
	return L_Name;
}

FString CRPGLibrary::GetCharacterActionTypeAsString(ECharacterAction EnumValue)
{
	const UEnum* enumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("ECharacterAction"), true);
	if (!enumPtr)
		return FString(TEXT("Invalid NoEnum"));

	FString L_Name = enumPtr->GetNameStringByIndex(static_cast<int32>(EnumValue));
	return L_Name;
}

FString CRPGLibrary::GetCombatTypeAsString(ECombatType EnumValue)
{
	const UEnum* enumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("ECombatType"), true);
	if (!enumPtr)
		return FString(TEXT("Invalid NoEnum"));

	FString L_Name = enumPtr->GetNameStringByIndex(static_cast<int32>(EnumValue));
	return L_Name;
}

FString CRPGLibrary::GetEquipSlotsTypeAsString(EEquipSlots EnumValue)
{
	const UEnum* enumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("EEquipSlots"), true);
	if (!enumPtr)
		return FString(TEXT("Invalid NoEnum"));

	FString L_Name = enumPtr->GetNameStringByIndex(static_cast<int32>(EnumValue));
	return L_Name;
}

FString CRPGLibrary::GetAttributesAsString(EAttributes EnumValue)
{
	const UEnum* enumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("EAttributes"), true);
	if (!enumPtr)
		return FString(TEXT("Invalid NoEnum"));

	FString L_Name = enumPtr->GetNameStringByIndex(static_cast<int32>(EnumValue));
	return L_Name;
}

