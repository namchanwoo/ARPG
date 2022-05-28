#include "CInventoryEquipmentLibrary.h"

#include "Blueprint/UserWidget.h"
#include "Systems/ItemSystem/CRPGItemAsset.h"

bool CInventoryEquipmentLibrary::OnItemValid(const FItemSlot& InItem)
{
	return IsValid(InItem.Item);
}

int32 CInventoryEquipmentLibrary::GetSlotItemMaxStackAmount(const FItemSlot& ItemSlot)
{
	if (IsValid(ItemSlot.Item))
		return ItemSlot.Item->MaxCount;
	return {};
}

EEquipSlots CInventoryEquipmentLibrary::GetItemEquipSlot(const FItemSlot& ItemSlot)
{
	if (IsValid(ItemSlot.Item))
		return ItemSlot.Item->EquipSlot;
	return {};
}

EItemType CInventoryEquipmentLibrary::GetSlotItemType(const FItemSlot& ItemSlot)
{
	if (IsValid(ItemSlot.Item))
		return ItemSlot.Item->ITemType;
	return {};
}

bool CInventoryEquipmentLibrary::IsSlotItemWeapon(const FItemSlot& ItemSlot)
{
	if (IsValid(ItemSlot.Item))
		return ItemSlot.Item->IsWeapon();
	return false;
}

bool CInventoryEquipmentLibrary::IsAmmoArchery(const FItemSlot& ItemSlot)
{
	if (IsValid(ItemSlot.Item))
	{
		bool L_ArcheryAmmoType = ItemSlot.Item->AmmoType == EAmmoType::Arrows ||
			ItemSlot.Item->AmmoType == EAmmoType::Bolts;

		//아이템 유형이 탄약유형이고 (그리고) 아이템탄약유형이 볼트나 화살이어야한다
		return L_ArcheryAmmoType && ItemSlot.Item->ITemType == EItemType::Ammo;
	}

	return false;
}

EItemHandType CInventoryEquipmentLibrary::GetSlotWeaponHandType(const FItemSlot& ItemSlot)
{
	if (IsValid(ItemSlot.Item))
		return ItemSlot.Item->WeaponHandType;
	return EItemHandType::None;
}

ECombatType CInventoryEquipmentLibrary::GetSlotItemWeaponType(const FItemSlot& ItemSlot)
{
	if (IsValid(ItemSlot.Item))
		return ItemSlot.Item->WeaponType;
	return ECombatType::None;
}

EItemEquipSpawnSetting CInventoryEquipmentLibrary::GetSlotItemEquipSpawnSetting(const FItemSlot& ItemSlot)
{
	if (IsValid(ItemSlot.Item))
		return ItemSlot.Item->ItemEquipSpawnSetting;
	return EItemEquipSpawnSetting::CanNotSpawn;
}

EAmmoType CInventoryEquipmentLibrary::GetItemAmmoType(const FItemSlot& ItemSlot)
{
	if (IsValid(ItemSlot.Item))
		return ItemSlot.Item->AmmoType;
	return {};
}

TArray<EAmmoType> CInventoryEquipmentLibrary::GetWeaponAmmoUseTypes(const FItemSlot& ItemSlot)
{
	if (IsValid(ItemSlot.Item))
		return ItemSlot.Item->AmmoUseTypes;
	return {};
}

TSubclassOf<ACBaseItem> CInventoryEquipmentLibrary::GetSlotItemMasterActorClass(const FItemSlot& ItemSlot)
{
	if (IsValid(ItemSlot.Item))
		return ItemSlot.Item->WorldItemActorClass;
	return nullptr;
}

UCRPGItemAsset* CInventoryEquipmentLibrary::GetItemAmmoQuiverItem(const FItemSlot& ItemSlot)
{
	if (IsValid(ItemSlot.Item))
		return ItemSlot.Item->QuiverItem;
	return nullptr;
}

bool CInventoryEquipmentLibrary::IsSlotItemTypeEqual(const FItemSlot& ItemSlot, EItemType CompareItemType)
{
	if (IsValid(ItemSlot.Item))
		return ItemSlot.Item->ITemType == CompareItemType;
	return false;
}


void CInventoryEquipmentLibrary::SetWidgetActive(UUserWidget* InTargetWidget, bool InActive)
{
	if (InActive)
	{
		InTargetWidget->SetRenderOpacity(1.0f);
		InTargetWidget->SetIsEnabled(true);
		InTargetWidget->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		InTargetWidget->SetRenderOpacity(0.5f);
		InTargetWidget->SetIsEnabled(false);
		InTargetWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
}

FItemMontage CInventoryEquipmentLibrary::GetItemMontages(const FItemSlot& ItemSlot)
{
	if (IsValid(ItemSlot.Item))
		return ItemSlot.Item->ItemMontages;
	return {};
}
