


#include "CInventoryComponent.h"


//
// #include "CInventoryComponent.h"
//
// UCInventoryComponent::UCInventoryComponent()
// {
// 	
// 	FInventorySlot InitializeInventory;
// 	InitializeInventory.InventoryPanel =EInventoryPanels::DefaultPanel;
// 	InitializeInventory.MaxSlots =104;
//
// 	Inventory.Add(InitializeInventory);
//
//
// 	PanelName.Add(EInventoryPanels::DefaultPanel,FText::FromString(TEXT("Miscellaneous")));
// 	PanelName.Add(EInventoryPanels::Panel1,FText::FromString(TEXT("Tools")));
// 	PanelName.Add(EInventoryPanels::Panel2,FText::FromString(TEXT("Weapons")));
// 	PanelName.Add(EInventoryPanels::Panel3,FText::FromString(TEXT("Shields")));
// 	PanelName.Add(EInventoryPanels::Panel4,FText::FromString(TEXT("Armor")));
// 	PanelName.Add(EInventoryPanels::Panel5,FText::FromString(TEXT("Ammo")));
// 	PanelName.Add(EInventoryPanels::Panel6,FText::FromString(TEXT("Rings")));
// 	
// 	//Initialize Setting
// 	{
// 		SlotsPerRow = 8;
// 		ObtainedItemVisibleDuration =2.0f;
// 		bCanShowObtainedMessage =true;
// 		ActivePanel = EInventoryPanels::Panel1;
// 		SortingMethod = ESortingMethod::ByType;
// 	}
// }

bool UCInventoryComponent::FindItemByData(UCRPGItemAsset* ItemDA, bool SearchByPanel, FItemSlot& OutItem)
{
	OutItem = FItemSlot();
	return false;
}

FItemSlot UCInventoryComponent::FindItemByGuid(EInventoryPanels InventoryPanel, FGuid Guid)
{
	return FItemSlot();
}

bool UCInventoryComponent::AddItem(UCRPGItemAsset* Item, int32 AmontToAdd, bool ShowObtainedMessage)
{
	return false;
}

void UCInventoryComponent::SetItemEquipped(bool InEquipped, EInventoryPanels InPanel, int32 InIndex)
{
	
}

// void UCInventoryComponent::InitializeInventory()
// {
// 	
// }
//
// void UCInventoryComponent::UseItem(const FItemSlot& ItemSlot, bool bCond)
// {
// 	
// }
//

