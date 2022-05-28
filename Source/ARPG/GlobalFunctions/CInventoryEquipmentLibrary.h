#pragma once

#include "CoreMinimal.h"
#include "Datas/ItemData.h"
#include "Systems/ItemSystem/Actors/CBaseItem.h"


class ARPG_API CInventoryEquipmentLibrary
{
public:

	/**아이템이 유효한가?*/
	static bool OnItemValid(const FItemSlot& InItem);

	/**아이템의 최대 수량을 가져옵니다*/
	static int32 GetSlotItemMaxStackAmount(const FItemSlot& ItemSlot);
	
	/**아이템의 장비슬롯 유형을 가져옵니다*/
	static  EEquipSlots GetItemEquipSlot(const FItemSlot& ItemSlot);
	
	/**아이템 타입을 가져옵니다*/
	static EItemType GetSlotItemType(const FItemSlot& ItemSlot);

	/**아이템이 무기유형인가?*/
	static bool IsSlotItemWeapon(const FItemSlot& ItemSlot);

	/**아처탄약 유형인가?*/
	static bool IsAmmoArchery(const FItemSlot& ItemSlot);

	/**아이템의 손유형을 가져옵니다*/
	static EItemHandType GetSlotWeaponHandType(const FItemSlot& ItemSlot);

	/**아이템의 전투유형을 가져옵니다*/
	static ECombatType GetSlotItemWeaponType(const FItemSlot& ItemSlot);

	/**장착아이템의 스폰 유형 설정을 가져옵니다*/
	static EItemEquipSpawnSetting GetSlotItemEquipSpawnSetting(const FItemSlot& ItemSlot);

	/**아이템의 탄약유형을 가져옵니다*/
	static EAmmoType GetItemAmmoType(const FItemSlot& ItemSlot);
	
	/**아이템의 무기탄약사용 유형을 가져옵니다*/
	static TArray<EAmmoType> GetWeaponAmmoUseTypes(const FItemSlot& ItemSlot);
	
	/**아이템의 클래스를 가져옵니다*/
	static TSubclassOf<ACBaseItem> GetSlotItemMasterActorClass(const FItemSlot& ItemSlot);

	/**아이템의 탄약통을 가져옵니다*/
	static UCRPGItemAsset* GetItemAmmoQuiverItem(const FItemSlot& ItemSlot);
	


	
	

	/**입력받은 아이템의 아이템 유형과 지정한 아이템유형이 같은가?*/
	static bool IsSlotItemTypeEqual(const FItemSlot& ItemSlot, EItemType CompareItemType);
	
	
	static void SetWidgetActive(UUserWidget* InTargetWidget, bool InActive);



	/**아이템의 몽테이지를 가져옵니다*/
	static FItemMontage GetItemMontages(const FItemSlot& ItemSlot);
	
};






