#include "CRPGItemAsset.h"
#include "Global.h"


UCRPGItemAsset::UCRPGItemAsset()
{
	//World Item Info
	{
		ItemEquipSpawnSetting = EItemEquipSpawnSetting::Spawn_Visible;
	}

	//Item Info
	{
		ItemPanel = EInventoryPanels::DefaultPanel;
		ItemName = FText::FromString(TEXT("Item Name"));
		ItemDescription = FText::FromString(TEXT("Item Description"));
		ItemRarity = EItemRarity::Common;
		MaxCount = 99;
	}

	//Item Stats
	{
		ResellValueMultiplier = 0.25f;
	}

	//Weapon
	{
		BowDrawSpeedRate = 1.0f;
		ArcheryProjectileEquipSocket = FName(TEXT("Arrow"));
		PlayerHandStringSocketName = FName(TEXT("BowStringSocket"));
		WeaponStartSocket = FName(TEXT("WeaponStart"));
		WeaponEndSocket = FName(TEXT("WeaponEnd"));
	}

	//Tool
	{
		ModificationPercentage = 35;
		ItemStatusEffect.TimerTickInterval = 0.1f;
		ItemStatusEffect.EffectDuration = 5.0f;
	}
}

TSubclassOf<UCBaseImpactEffect> UCRPGItemAsset::GetImpactEffect_Implementation(EImpactDecision InImpactDecision)
{
	return GeneralImpactEffect;
}

int32 UCRPGItemAsset::GetRoomOnStack(int32 InAmount)
{
	return MaxCount - InAmount;
}

FName UCRPGItemAsset::GetEquipSocket()
{
	const auto EquipString = CRPGLibrary::GetEquipSlotsTypeAsString(EquipSlot);
	CHECK(EquipSocket.Contains(EquipSlot), *EquipString)
	return *EquipSocket.Find(EquipSlot);
}

FName UCRPGItemAsset::GetUnEquipSocket()
{
	return GetUnEquipSocketForEquipSlot(EquipSlot);
}

float UCRPGItemAsset::GetPhysicalDamage_Implementation()
{
	return 0;
}

FItemStatModifier UCRPGItemAsset::GetModifierForStat(EStats InStat)
{
	CHECK(ItemStats.Contains(InStat), FItemStatModifier());
	return *ItemStats.Find(InStat);
}

float UCRPGItemAsset::GetItemStatValue(EStats InStat)
{
	CHECK(ItemStats.Contains(InStat), NOTFIND_STATVALUE);
	return ItemStats.Find(InStat)->Value;
}

void UCRPGItemAsset::UseItem(AActor* InCaller)
{
}

bool UCRPGItemAsset::IsWeapon()
{
	return ITemType == EItemType::MeleeWeapon || ITemType == EItemType::RangedWeapon || ITemType == EItemType::Shield ||
		WeaponHandType != EItemHandType::None;
}

FName UCRPGItemAsset::FindEquipSocket(EEquipSlots InEquipSlot)
{
	const auto EquipString = CRPGLibrary::GetEquipSlotsTypeAsString(EquipSlot);
	CHECK(EquipSocket.Contains(InEquipSlot), *EquipString);
	return *EquipSocket.Find(InEquipSlot);
}

FName UCRPGItemAsset::FindUnEquipSocket(EEquipSlots InEquipSlot)
{
	const auto EquipString = CRPGLibrary::GetEquipSlotsTypeAsString(InEquipSlot);

	TMap<EEquipSlots, EUnEquipSlots> L_EquipMap;
	L_EquipMap.Add(EEquipSlots::WeaponSlot, EUnEquipSlots::WeaponSheath);
	L_EquipMap.Add(EEquipSlots::OffHand_WeaponSlot, EUnEquipSlots::ExtraWeaponSheath);
	L_EquipMap.Add(EEquipSlots::ShieldSlot, EUnEquipSlots::ShieldSheathSlot);

	CHECK(L_EquipMap.Contains(InEquipSlot), *EquipString);

	const auto L_UnEquipSlot = *L_EquipMap.Find(InEquipSlot);
	CHECK(UnEquipSocket.Contains(L_UnEquipSlot), *EquipString);
	return *UnEquipSocket.Find(L_UnEquipSlot);
}

bool UCRPGItemAsset::CanStack()
{
	return MaxCount > 1;
}

int32 UCRPGItemAsset::GetResellValue()
{
	return UKismetMathLibrary::FTrunc(Price * ResellValueMultiplier);
}

FName UCRPGItemAsset::GetUnEquipSocketForEquipSlot(EEquipSlots InEquipSlot)
{
	TMap<EEquipSlots, EUnEquipSlots> L_EquipMap;

	L_EquipMap.Add(EEquipSlots::WeaponSlot, EUnEquipSlots::WeaponSheath);
	L_EquipMap.Add(EEquipSlots::OffHand_WeaponSlot, EUnEquipSlots::ExtraWeaponSheath);
	L_EquipMap.Add(EEquipSlots::ShieldSlot, EUnEquipSlots::ShieldSheathSlot);

	if (L_EquipMap.Contains(InEquipSlot))
	{
		const auto UnEquipSlot = *L_EquipMap.Find(InEquipSlot);
		return *UnEquipSocket.Find(UnEquipSlot);
	}

	return FName(TEXT("None"));
}
