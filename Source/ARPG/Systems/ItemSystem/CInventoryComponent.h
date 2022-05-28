#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Datas/ItemData.h"
#include "CInventoryComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemRemoved, FItemSlot, InItem);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemAdded, FItemSlot, InItem);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInventoryItemSlotUpdated, FItemSlot, InItem);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActivePanelSwitched, EInventoryPanels, InActivePanel);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInventoryRefreshed, EInventoryPanels, InActivePanel);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemUsed, FItemSlot, ItemToUse, bool, UseImmediately);


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ARPG_API UCInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	//
	//
public:
	//우선순위
	bool FindItemByData(UCRPGItemAsset* ItemDA, bool SearchByPanel, FItemSlot& OutItem);

	/**주어진 아이템 ID로 인벤토리에서 인덱스 찾기*/
	FItemSlot FindItemByGuid(EInventoryPanels InventoryPanel, FGuid InGuid);


	bool AddItem(UCRPGItemAsset* Item, int32 AmontToAdd, bool ShowObtainedMessage);

	/**아이템 장착을 설정합니다*/
	void SetItemEquipped(bool InEquipped, EInventoryPanels InPanel, int32 InIndex);
	// 	
	// 	void InitializeInventory();
	// 	
	// 	void UseItem(const FItemSlot& ItemSlot, bool bCond);
	//
	//
	// private:
	//
	// 	/**슬롯 당 행*/
	// 	UPROPERTY(BlueprintReadOnly,EditAnywhere, Category = "Initialize", meta = (AllowPrivateAccess = "true"))
	// 	int32 SlotsPerRow;
	//
	// 	/**획득한 항목 (가시성) 지속시간*/
	// 	UPROPERTY(BlueprintReadOnly,EditAnywhere, Category = "Initialize", meta = (AllowPrivateAccess = "true"))
	// 	float ObtainedItemVisibleDuration;
	//
	// 	/**메세지 표시여부*/
	// 	UPROPERTY(BlueprintReadOnly,EditAnywhere, Category = "Initialize", meta = (AllowPrivateAccess = "true"))
	// 	bool bCanShowObtainedMessage;
	// 	
	// 	/**활성화된 패널*/
	// 	UPROPERTY(BlueprintReadOnly,EditAnywhere, Category = "Initialize", meta = (AllowPrivateAccess = "true"))
	// 	EInventoryPanels ActivePanel;
	//
	// 	/**사용중인 패널*/
	// 	UPROPERTY(BlueprintReadOnly, Category = "Initialize", meta = (AllowPrivateAccess = "true"))
	// 	TSet<EInventoryPanels> PanelsInUse;
	// 	
	// 	/**표시할 패널*/
	// 	UPROPERTY(BlueprintReadOnly,EditAnywhere, Category = "Initialize", meta = (AllowPrivateAccess = "true"))
	// 	TSet<EInventoryPanels> PanelsToDisplay;
	//
	// 	/**정렬할 방법*/
	// 	UPROPERTY(BlueprintReadOnly,EditAnywhere, Category = "Initialize", meta = (AllowPrivateAccess = "true"))
	// 	ESortingMethod SortingMethod;
	// 	
	// 	UPROPERTY(BlueprintReadOnly,EditAnywhere, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	// 	TArray<FInventorySlot> Inventory;
	//
	// 	UPROPERTY(BlueprintReadOnly,EditAnywhere, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	// 	TMap<EInventoryPanels,FText> PanelName;
	//
	// 	/**마지막 사용 아이템*/
	// 	FItemSlot LastUseItem;	

#pragma region               DECLARE_DYNAMIC_MULTICAST_DELEGATE
public:
	UPROPERTY(BlueprintAssignable)
	FOnItemRemoved OnItemRemoved;
	UPROPERTY(BlueprintAssignable)
	FOnItemAdded OnItemAdded;
	UPROPERTY(BlueprintAssignable)
	FOnInventoryItemSlotUpdated OnInventoryItemSlotUpdated;
	UPROPERTY(BlueprintAssignable)
	FOnActivePanelSwitched OnActivePanelSwitched;
	UPROPERTY(BlueprintAssignable)
	FOnInventoryRefreshed OnInventoryRefreshed;
	UPROPERTY(BlueprintAssignable)
	FOnItemUsed OnItemUsed;
#pragma endregion            DECLARE_DYNAMIC_MULTICAST_DELEGATE
};
