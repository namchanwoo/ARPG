#include "CEquipmentManagerComponent.h"

#include "Global.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetGuidLibrary.h"
#include "Systems/ItemSystem/CInventoryComponent.h"
#include "Systems/ItemSystem/CRPGItemAsset.h"
#include "Systems/ItemSystem/Actors/CBaseArcheryWeapon.h"
#include "Systems/ItemSystem/Actors/CBaseItem.h"
#include "Systems/ItemSystem/Actors/CBaseShield.h"


UCEquipmentManagerComponent::UCEquipmentManagerComponent()
{
	//기본 장비 분류 설정
	for (int32 i = 0; i < static_cast<int32>(EEquipSlots::MAX); i++)
	{
		EEquipSlots L_EquipSlot = static_cast<EEquipSlots>(i);
		if (L_EquipSlot == EEquipSlots::None) continue;
		FEquipmentSlot L_EquipmentSlotStruct = FEquipmentSlot(L_EquipSlot);
		EquipmentSlots.Add(L_EquipmentSlotStruct);
	}

	CurrentDodgeType = EDodgeType::RollDodge;
	GeneralCombatType = EGeneralCombatType::MeleeCombat;
}

void UCEquipmentManagerComponent::BeginPlay()
{
	Super::BeginPlay();
	OwnerCharacterRef = Cast<ACharacter>(GetOwner());
	GetOwner()->OnDestroyed.AddDynamic(this, &UCEquipmentManagerComponent::EventOn_OwnerDestroyed);
}

void UCEquipmentManagerComponent::InitializeEquipment(TArray<FEquipmentSlot> InEquipArray)
{
	//임시 장비슬롯들 설정
	TArray<FEquipmentSlot> L_EquipmentSlots = InEquipArray;

	//1.액티브 슬롯의 액터들을 초기화합니다
	{
		for (const auto ActiveEquippedSlotActor : ActiveEquippedSlotActors)
		{
			//액티브슬롯의 액터가 존재한다면 소멸시킨다
			if (IsValid(ActiveEquippedSlotActor.Value))
				ActiveEquippedSlotActor.Value->Destroy();
		}
		ActiveEquippedSlotActors.Empty(); //활성화된 슬롯의 액터를 초기화한다		
	}


	//2. Equipment Slots 초기화
	for (auto F_EquipmentSlot : EquipmentSlots)
	{
		const EEquipSlots L_EquipSlot = F_EquipmentSlot.EquipSlot; //로컬 슬롯타입 설정
		for (int32 i = 0; i < F_EquipmentSlot.Items.Num(); i++)
		{
			SetItemInSlot(L_EquipSlot, FItemSlot(), 0);
		}
	}


	//3.장착을 설정합니다
	{
		//인벤토리 컴포넌트가 유효하다면
		if (IsValid(InventoryComponent))
		{
			for (auto L_EquipmentSlot : L_EquipmentSlots)
			{
				// EEquipSlots L_EquipSlot = L_EquipmentSlot.EquipSlot;
				for (int32 i = 0; i < L_EquipmentSlot.Items.Num() - 1; i++)
				{
					if (CInventoryEquipmentLibrary::OnItemValid(L_EquipmentSlot.Items[i]))
					{
						FItemSlot L_Item = L_EquipmentSlot.Items[i];
						int32 L_EquipIndex = i;

						FItemSlot L_FindItem_Data;
						InventoryComponent->FindItemByData(L_Item.Item, true, L_FindItem_Data);
						FItemSlot L_FindItem_Guid = InventoryComponent->FindItemByGuid(L_Item.Item->ItemPanel, L_Item.Guid);

						FItemSlot L_InventoryItem = CRPGLibrary::SelectStruct(L_FindItem_Guid, L_FindItem_Data,
						                                                      UKismetGuidLibrary::IsValid_Guid(L_Item.Guid));

						if (CInventoryEquipmentLibrary::OnItemValid(L_InventoryItem))
						{
							EquipItem(L_InventoryItem, L_EquipIndex, EEquipSameItemDecision::Replace);
						}
						else
						{
							if (InventoryComponent->AddItem(L_Item.Item, L_Item.Amount, false))
							{
								InventoryComponent->FindItemByData(L_Item.Item, true, L_FindItem_Data);
								L_FindItem_Guid = InventoryComponent->FindItemByGuid(L_Item.Item->ItemPanel, L_Item.Guid);

								L_InventoryItem = CRPGLibrary::SelectStruct(L_FindItem_Guid, L_FindItem_Data,
								                                            UKismetGuidLibrary::IsValid_Guid(L_Item.Guid));
								if (CInventoryEquipmentLibrary::OnItemValid(L_InventoryItem))
								{
									EquipItem(L_InventoryItem, L_EquipIndex, EEquipSameItemDecision::Replace);
								}
							}
						}
					}
				}
			}
		}
		else //인벤토리 컴포넌트가 유효하지 않다면
		{
			for (auto F_EquipmentSlot : L_EquipmentSlots)
			{
				// EEquipSlots L_EquipSlot = F_EquipmentSlot.EquipSlot;
				for (int32 i = 0; i < F_EquipmentSlot.Items.Num(); i++)
				{
					if (CInventoryEquipmentLibrary::OnItemValid(F_EquipmentSlot.Items[i]))
					{
						const FItemSlot L_Item = F_EquipmentSlot.Items[i];
						const int32 L_EquipIndex = i;
						EquipItem(L_Item, L_EquipIndex, EEquipSameItemDecision::Replace);
					}
				}
			}
		}
	}
}

void UCEquipmentManagerComponent::InitializeCharacterPreview()
{
}

void UCEquipmentManagerComponent::InitializeEquipmentComponent()
{
	InitializeCharacterPreview();

	InventoryComponent = Cast<UCInventoryComponent>(GetOwner()->GetComponentByClass(UCInventoryComponent::StaticClass()));

	//장비를 초기화 한다
	InitializeEquipment(EquipmentSlots);

	//인벤토리 컴포넌트의 델리게이트를 바인딩
	if (InventoryComponent != nullptr)
	{
		InventoryComponent->OnItemAdded.AddDynamic(this, &UCEquipmentManagerComponent::OnInventorySlotUpdated);
		InventoryComponent->OnItemRemoved.AddDynamic(this, &UCEquipmentManagerComponent::OnInventorySlotUpdated);
	}
}

void UCEquipmentManagerComponent::SetCombatType(ECombatType NewCombatType)
{
	if (CurrentCombatType != NewCombatType)
	{
		PrevCombatType = CurrentCombatType; //이전 전투 타입을 설정한다

		//이전 전투 타입을 끝낸다.
		if (OnCombatTypeEnd.IsBound())
			OnCombatTypeEnd.Broadcast(PrevCombatType);

		CurrentCombatType = NewCombatType; //현재 전투 타입을 새로운 전투타입으로 설정한다

		//새로운 전투 타입으로 체인지 한다
		if (OnCombatTypeChanged.IsBound())
			OnCombatTypeChanged.Broadcast(CurrentCombatType);
	}
}

void UCEquipmentManagerComponent::UpdateCombatType(ECombatType InWeaponType, EItemType InItemType)
{
	//웨폰슬롯의 활성화된 아이템슬롯을 가져옵니다
	const FItemSlot L_WeaponItem = GetActiveEquippedSlotItem(EEquipSlots::WeaponSlot);

	//활성화된 무기슬롯의 아이템이 유효하다면
	if (CInventoryEquipmentLibrary::OnItemValid(L_WeaponItem))
	{
		//1. 아이템의 타입에 따른 일반전투 유형을 결정한다
		{
			switch (InItemType)
			{
			case EItemType::MeleeWeapon:
				SetGeneralCombatType(EGeneralCombatType::MeleeCombat);
				break;

			case EItemType::RangedWeapon:
				SetGeneralCombatType(EGeneralCombatType::RangedCombat);
				break;

			default: break;
			}
		}

		//2. 전투 타입을 설정합니다
		{
			//쉴드슬롯이 활성화 상태라면 
			const bool EnableShieldSlot = !IsEquipSlotDisabled(EEquipSlots::ShieldSlot);
			//아이템타입의 손유형이 한손이라면
			const bool IsOneHandType = CInventoryEquipmentLibrary::GetSlotWeaponHandType(L_WeaponItem) == EItemHandType::OneHand;
			//활성화된 아이템 유형중에 쉴드유형이 존재한다면
			const bool ShieldActiveSlot = IsItemTypeEquippedToActiveSlot(EItemType::Shield);
			//아이템이 전투형이 쉴드유형과 함께 할수 있는지
			const bool IsWeaponWithShield = L_WeaponItem.Item->WeaponTypeWithShield != ECombatType::None;

			//한손엔 다른 무기 다른손엔 방패를 쥘 수 있는 자세가 가능하다면
			if (EnableShieldSlot && IsOneHandType && ShieldActiveSlot && IsWeaponWithShield)
				SetCombatType(L_WeaponItem.Item->WeaponTypeWithShield); //무기 유형에 따라 방패 전투 유형 선택
			else //위의 4가지 조건중 하나라도 부합되지 않는다면
				SetCombatType(InWeaponType);
		}
	}
	else //활성화된 무기슬롯의 아이템이 유효하지 않다면
	{
		//쉴드 슬롯이 활성화된 아이템유형에 존재하고 (그리고) 쉴드슬롯이 활성화 되어 있다면
		if (IsItemTypeEquippedToActiveSlot(EItemType::Shield) && !IsEquipSlotDisabled(EEquipSlots::ShieldSlot))
		{
			SetCombatType(ECombatType::UnarmedCombatAndShield);
			SetGeneralCombatType(EGeneralCombatType::MeleeCombat);
		}
		else //쉴드 슬롯이 활성화 되어 있지 않거나 (또는) 방패슬롯이 비활성화 되어 있다면
		{
			SetCombatType(ECombatType::Unarmed);
			SetGeneralCombatType(EGeneralCombatType::MeleeCombat);
		}
	}
}

void UCEquipmentManagerComponent::SetGeneralCombatType(EGeneralCombatType NewGeneralCombatType)
{
	if (GeneralCombatType != NewGeneralCombatType)
	{
		//이전 전투 타입을 끝낸다.
		if (OnGeneralCombatTypeEnd.IsBound())
			OnGeneralCombatTypeEnd.Broadcast(GeneralCombatType);

		//현재 전투 타입을 새로운 전투타입으로 설정한다
		GeneralCombatType = NewGeneralCombatType;

		//새로운 전투 타입으로 체인지 한다
		if (OnGeneralCombatTypeChanged.IsBound())
			OnGeneralCombatTypeChanged.Broadcast(NewGeneralCombatType);
	}
}

void UCEquipmentManagerComponent::SetCombatEnabled(bool InCondition)
{
	if (bIsCombatEnabled != InCondition)
	{
		bIsCombatEnabled = InCondition;
		if (OnCombatChanged.IsBound())
			OnCombatChanged.Broadcast(bIsCombatEnabled);
	}
}

void UCEquipmentManagerComponent::ToggleCombat()
{
	const bool L_EnableCombat = !bIsCombatEnabled; //토글을 위한 반대키

	//웨폰 슬롯
	{
		SetSlotDisabled(EEquipSlots::WeaponSlot, !L_EnableCombat);
		SetCombatEnabled(L_EnableCombat); //컴뱃상태의 활성화를 토글

		//웨폰슬롯이 활성화되어 있다면 Equip 비활성화되어 있다면 Sheath
		EAttachType L_AttachType;
		!IsEquipSlotDisabled(EEquipSlots::WeaponSlot) ? L_AttachType = EAttachType::EquipSocket : L_AttachType = EAttachType::SheathSocket;

		AttachEquipSlotActor(EEquipSlots::WeaponSlot, false, FName(TEXT("None")), L_AttachType);
	}

	//쉴드슬롯
	{
		//활성화된 쉴드슬롯의 아이템이 유효하다면
		if (CInventoryEquipmentLibrary::OnItemValid(GetActiveEquippedSlotItem(EEquipSlots::ShieldSlot)))
		{
			//현재 웨폰타입이 원핸드가 아니라면
			const bool NotOneHand = CInventoryEquipmentLibrary::GetSlotWeaponHandType(GetActiveEquippedSlotItem(EEquipSlots::WeaponSlot)) !=
				EItemHandType::OneHand;
			//보조 무기 타입이 유효하다면
			const bool ExtraWeaponValid = CInventoryEquipmentLibrary::OnItemValid(GetActiveEquippedSlotItem(EEquipSlots::OffHand_WeaponSlot));
			//조건에 따라 활성화상태를 설정
			const bool L_ShieldDisabe = !L_EnableCombat || NotOneHand || ExtraWeaponValid;
			SetSlotDisabled(EEquipSlots::ShieldSlot, L_ShieldDisabe); //비활성화 설정

			//웨폰슬롯이 활성화되어 있다면 Equip 비활성화되어 있다면 Sheath
			EAttachType L_AttachType;
			!IsEquipSlotDisabled(EEquipSlots::ShieldSlot) ? L_AttachType = EAttachType::EquipSocket : L_AttachType = EAttachType::SheathSocket;

			AttachEquipSlotActor(EEquipSlots::ShieldSlot, false, FName(TEXT("None")), L_AttachType);
			UpdateCombatType(CurrentCombatType, GetEquippedHandItemType()); //컴뱃 상태를 업데이트한다
		}
	}

	//레인지 슬롯
	{
		const FItemSlot L_RangeWeaponSlot = GetActiveEquippedSlotItem(EEquipSlots::WeaponSlot);
		const FItemSlot L_AmmoItemSlot = GetActiveEquippedSlotItem(EEquipSlots::AmmoSlot);

		//무기슬롯의 아이템 타입이 원거리무기가 아니라면
		const bool NotRangedWeapon = !CInventoryEquipmentLibrary::IsSlotItemTypeEqual(L_RangeWeaponSlot, EItemType::RangedWeapon);
		//원거리무기탄약에 탄약유형의 타입이 포함되어 있지 않다면
		const bool NotContainsAmmo = !CInventoryEquipmentLibrary::GetWeaponAmmoUseTypes(L_RangeWeaponSlot).Contains(
			CInventoryEquipmentLibrary::GetItemAmmoType(L_AmmoItemSlot));
		//조건에 따라 활성화상태를 설정
		const bool L_Disable = !bIsCombatEnabled || (NotRangedWeapon || NotContainsAmmo);
		DisableEquipSlot(EEquipSlots::AmmoSlot, L_Disable);

		//아처무기가 유효하다면
		if (IsValid(ArcheryWeaponEquipped))
			ArcheryWeaponEquipped->UpdateArrowVisibility(!IsEquipSlotDisabled(EEquipSlots::AmmoSlot));
	}
}

EItemType UCEquipmentManagerComponent::GetEquippedHandItemType()
{
	const FItemSlot L_WeaponSlot = GetActiveEquippedSlotItem(EEquipSlots::WeaponSlot);

	//무기유형의 아이템이 유효하다면
	if (CInventoryEquipmentLibrary::OnItemValid(L_WeaponSlot))
		return CInventoryEquipmentLibrary::GetSlotItemType(L_WeaponSlot);
	else //아이템이 유효하지 않다면
	{
		//추가무기슬롯의 아이템이 유효하다면
		const FItemSlot L_ExtraWeapon = GetActiveEquippedSlotItem(EEquipSlots::OffHand_WeaponSlot);
		if (CInventoryEquipmentLibrary::OnItemValid(L_ExtraWeapon))
			return CInventoryEquipmentLibrary::GetSlotItemType(L_ExtraWeapon);
		else
			return EItemType::None;
	}
}

bool UCEquipmentManagerComponent::IsWeaponEquipped()
{
	return IsItemTypeEquippedToActiveSlot(EItemType::MeleeWeapon) || IsItemTypeEquippedToActiveSlot(EItemType::RangedWeapon);
}

void UCEquipmentManagerComponent::SetEquipSlot(EEquipSlots InEquipSlot, const FItemSlot& InItem, int32 ItemIndex, EUpdateQuickSlotType UpdateType)
{
	SetItemInSlot(InEquipSlot, InItem, ItemIndex); //장착슬롯 아이템 설정

	const int32 L_EquipSlotIndex = GetEquipSlotIndex(InEquipSlot); //슬롯유형 인덱스
	FEquipmentSlot L_EquipSlot; //장비슬롯

	if (EquipmentSlots.IsValidIndex(L_EquipSlotIndex))
		L_EquipSlot = EquipmentSlots[L_EquipSlotIndex];

	if (OnEquipSlotUpdated.IsBound())
		OnEquipSlotUpdated.Broadcast(InEquipSlot, L_EquipSlot, UpdateType, ItemIndex);
}

FItemSlot UCEquipmentManagerComponent::GetEquippedSlotItem(EEquipSlots InEquipSlot, int32 ItemIndex)
{
	TArray<FItemSlot> L_ItemSlots = GetEquippedSlotsItems(InEquipSlot);

	//인덱스가 유효하다면
	if (L_ItemSlots.IsValidIndex(ItemIndex))
		return L_ItemSlots[ItemIndex];

	return FItemSlot();
}

int32 UCEquipmentManagerComponent::GetHeldAmountForEquipSlot(EEquipSlots InEquipSlot, int32 InItemIndex)
{
	return GetEquippedSlotItem(InEquipSlot, InItemIndex).Amount;
}

int32 UCEquipmentManagerComponent::GetEquipSlotIndex(EEquipSlots InEquipSlot)
{
	for (int32 i = 0; i < EquipmentSlots.Num(); i++)
	{
		if (EquipmentSlots[i].EquipSlot == InEquipSlot)
			return i;
	}
	return -1;
}

bool UCEquipmentManagerComponent::IsEquipSlotDisabled(EEquipSlots InEquipSlot)
{
	if (IsSlotDisabled.Contains(InEquipSlot))
		return *IsSlotDisabled.Find(InEquipSlot);
	return false;
}

int32 UCEquipmentManagerComponent::GetHeldAmountForActiveSlot(EEquipSlots InEquipSlot)
{
	return GetActiveEquippedSlotItem(InEquipSlot).Amount;
}

TSubclassOf<UCBaseImpactEffect> UCEquipmentManagerComponent::GetEquipmentImpactEffect(EImpactDecision InImpactDecision,
                                                                                      FVector& OutImpactEffectLocation)
{
	//임팩트 유형에 따라
	switch (InImpactDecision)
	{
	case EImpactDecision::None:
	case EImpactDecision::Blocked:
	case EImpactDecision::PerfectBlocked:
	case EImpactDecision::Parried:
	case EImpactDecision::DefenseBroken:

		//활성화된 쉴드슬롯의 액터가 유효하고 (그리고) 쉴드슬롯이 활성화되어있다면
		if (IsValid(GetActiveEquippedSlotActor(EEquipSlots::ShieldSlot)) && !IsEquipSlotDisabled(EEquipSlots::ShieldSlot))
		{
			//활성화된 쉴드슬롯의 액터를 가져옵니다
			ACBaseItem* L_ShieldActor = GetActiveEquippedSlotActor(EEquipSlots::ShieldSlot);
			//쉴드슬롯의 액터의 데이터에셋이 유효하다면
			if (IsValid(L_ShieldActor->DataAsset))
			{
				OutImpactEffectLocation = ShieldWeapon->GetActorLocation();
				return L_ShieldActor->DataAsset->GetImpactEffect(InImpactDecision);
			}
		}
		else //활성화된 쉴드슬롯의 액터가 유효하지 않거나 (또는) 쉴드슬롯이 비활성화되어있다면
		{
			//활성화된 웨폰슬롯의 액터를 가져옵니다
			const ACBaseItem* WeaponActor = GetActiveEquippedSlotActor(EEquipSlots::WeaponSlot);

			//웨폰슬롯의 액터가 유효하다면
			if (IsValid(WeaponActor))
			{
				//무기의 타입이 장착되어 있고 (그리고) 무기의 데이터에셋이 유효하다면
				if (IsWeaponEquipped() && IsValid(WeaponActor->DataAsset))
				{
					OutImpactEffectLocation = WeaponActor->GetActorLocation();
					return WeaponActor->DataAsset->GetImpactEffect(InImpactDecision);
				}
			}
		}

	case EImpactDecision::NotBlocked:
		//체스트슬롯의 아이템을 가져온다
		const auto L_ChestSlot = GetActiveEquippedSlotActor(EEquipSlots::ChestSlot);
	//상체슬롯 아이템이 유효하다면
		if (IsValid(L_ChestSlot))
		{
			//체스트 아이템의 데이터에셋이 유효하다면
			if (IsValid(L_ChestSlot->DataAsset))
			{
				OutImpactEffectLocation = FVector::ZeroVector;
				return L_ChestSlot->DataAsset->GetImpactEffect(InImpactDecision);
			}
		}
	}

	OutImpactEffectLocation = FVector::ZeroVector;
	return nullptr;
}

void UCEquipmentManagerComponent::UseActiveSlotItem(EEquipSlots InEquipSlot)
{
	//활성화된 슬롯의 아이템 인덱스를 가져온다
	const int32 L_ActiveIndex = GetEquipSlotActiveItemIndex(InEquipSlot);

	//인덱스가 유효하다면
	if (OnItemIndexValid(InEquipSlot, L_ActiveIndex))
	{
		ACBaseItem* L_ActiveEquipActor = GetActiveEquippedSlotActor(InEquipSlot);

		if (IsValid(L_ActiveEquipActor))
			L_ActiveEquipActor->OnUse();

		//EquipSlot 아이템이 유효하다면
		const FItemSlot L_ToolToUse = GetActiveEquippedSlotItem(InEquipSlot);
		if (CInventoryEquipmentLibrary::OnItemValid(L_ToolToUse))
		{
			L_ToolToUse.Item->UseItem(GetOwner());
			RemoveItemQuantity(InEquipSlot, 1, L_ActiveIndex); //수량을 1개 제거합니다
			if (OnItemUsed.IsBound())
				OnItemUsed.Broadcast(L_ToolToUse);
		}
	}
}

bool UCEquipmentManagerComponent::OnItemIndexValid(EEquipSlots InEquipSlot, int32 ItemIndex)
{
	const int32 L_EquipSlotIndex = GetEquipSlotIndex(InEquipSlot);

	//인덱스가 유효하다면
	if (EquipmentSlots.IsValidIndex(L_EquipSlotIndex))
		return EquipmentSlots[L_EquipSlotIndex].Items.IsValidIndex(ItemIndex);

	return false;
}

void UCEquipmentManagerComponent::SetEquipSlotDisabled(EEquipSlots InEquipSlots, bool IsDisabled)
{
	const int32 L_EquipSlotsIndex = GetEquipSlotIndex(InEquipSlots);
	EquipmentSlots[L_EquipSlotsIndex].bDisabled = IsDisabled;
	IsSlotDisabled.Add(InEquipSlots, IsDisabled);
}

void UCEquipmentManagerComponent::UpdateSlotItemAmount(EEquipSlots InEquipSlot, int32 InItemIndex, int32 InAmount)
{
	const int32 SlotIndex = GetEquipSlotIndex(InEquipSlot);
	EquipmentSlots[SlotIndex].Items[InItemIndex].Amount = InAmount;
}

void UCEquipmentManagerComponent::SetActiveItemIndex(EEquipSlots InEquipSlot, int32 ActiveItemIndex)
{
	const int32 SlotIndex = GetEquipSlotIndex(InEquipSlot);
	EquipmentSlots[SlotIndex].ActiveItemIndex = ActiveItemIndex;
}

void UCEquipmentManagerComponent::SetItemInSlot(EEquipSlots InEquipSlot, const FItemSlot& InItem, int32 Index)
{
	const int32 L_SlotIndex = GetEquipSlotIndex(InEquipSlot);
	EquipmentSlots[L_SlotIndex].Items[Index] = InItem;
}

// void UCEquipmentManagerComponent::SetItemInSlot(EEquipSlots InEquipSlot, UCRPGItemAsset* InItemAsset, int32 ItemAmount, int32 ItemIndex,
//                                                 bool ItemEquipped, FGuid ItemIGuid, int32 Index)
// {
// 	const int32 SlotIndex = GetEquipSlotIndex(InEquipSlot);
// 	// const FItemSlot L_SetItemSlot = FItemSlot(InItem,ItemAmount,ItemIndex,ItemEquipeed,ItemIGuid);
// 	// EquipmentSlots[SlotIndex].Items[Index] = L_SetItemSlot;
//
// 	if (EquipmentSlots[SlotIndex].Items.IsValidIndex(ItemIndex))
// 	{
// 		EquipmentSlots[SlotIndex].Items[Index].Item = InItemAsset;
// 		EquipmentSlots[SlotIndex].Items[Index].Amount = ItemAmount;
// 		EquipmentSlots[SlotIndex].Items[Index].Index = ItemIndex;
// 		EquipmentSlots[SlotIndex].Items[Index].bIsEquipped = ItemEquipped;
// 		EquipmentSlots[SlotIndex].Items[Index].Guid = ItemIGuid;
// 	}
// 	else //유효하지 않으면
// 	{
// 		const FItemSlot L_SetItemSlot = FItemSlot(InItemAsset, ItemAmount, ItemIndex, ItemEquipped, ItemIGuid);
// 		EquipmentSlots[SlotIndex].Items.Add(L_SetItemSlot);
// 	}
// }

TArray<FItemSlot> UCEquipmentManagerComponent::GetEquippedSlotsItems(EEquipSlots InEquipSlot)
{
	const int32 L_EquipSlotIndex = GetEquipSlotIndex(InEquipSlot);
	return EquipmentSlots[L_EquipSlotIndex].Items;
}

void UCEquipmentManagerComponent::SwitchEquipSlotActiveIndex(EEquipSlots InEquipSlot)
{
	int32 L_PrevItemIndex = GetEquipSlotActiveItemIndex(InEquipSlot); //이전 인덱스

	TArray<FItemSlot> L_EquippedItems = GetEquippedSlotsItems(InEquipSlot);
	const FItemSlot L_PrevItem = L_EquippedItems[L_PrevItemIndex]; //이전 아이템

	int32 L_Index;
	const FItemSlot L_NextItem = FindNextValidEquippedItem(InEquipSlot, L_PrevItemIndex, L_Index);

	//새로운 아이템이 유효하다면
	if (CInventoryEquipmentLibrary::OnItemValid(L_NextItem))
	{
		SetActiveItemIndex(InEquipSlot, L_Index); //활성화 된 인덱스를 설정

		if (OnEquipSlotUpdated.IsBound())
		{
			OnEquipSlotUpdated.Broadcast(InEquipSlot, EquipmentSlots[GetEquipSlotIndex(InEquipSlot)],
			                             EUpdateQuickSlotType::SwitchItems, L_PrevItemIndex);
			OnEquipSlotUpdated.Broadcast(InEquipSlot, EquipmentSlots[GetEquipSlotIndex(InEquipSlot)],
			                             EUpdateQuickSlotType::SwitchItems, L_Index);
		}

		ActiveSlotUnEquipped(InEquipSlot, L_PrevItem); //액티브슬롯을 장착해재한다
		ActiveSlotEquipped(L_PrevItem, L_NextItem, InEquipSlot); //액티브 슬롯을 장착한다
	}
}

void UCEquipmentManagerComponent::EquipItem(const FItemSlot& NewItem, int32 InItemIndex, EEquipSameItemDecision EquipSameItemDecision)
{
	//새로운 아이템이 유효한 경우에만
	if (CInventoryEquipmentLibrary::OnItemValid(NewItem))
	{
		const EEquipSlots L_EquipSlot = CInventoryEquipmentLibrary::GetItemEquipSlot(NewItem); //아이템 슬롯 타입을 가져온다

		const FItemSlot L_PrevItem = GetEquippedSlotItem(L_EquipSlot, InItemIndex); //이전 아이템을 설정

		//이전 아이템과 새로운 아이템이 같다면
		if (L_PrevItem.Item == NewItem.Item)
		{
			//아이템 업데이트 유형이 교체라면
			if (EquipSameItemDecision == EEquipSameItemDecision::Replace)
				UnEquipItemAtIndex(L_EquipSlot, InItemIndex);
		}
		else //이전 아이템과 새로운 아이템이 같지 않다면
			UnEquipItemAtIndex(L_EquipSlot, InItemIndex);


		// FItemSlot SetItemSlot;
		// SetItemSlot.Item = NewItem.Item;
		// //아이템 수량설정
		// int32 TotalAmount = NewItem.Amount + GetHeldAmountForEquipSlot(L_EquipSlot, ItemIndex);
		// int32 L_MaxStackAmount = InventoryEquipmentLibrary::GetSlotItemMaxStackAmount(NewItem);
		// SetItemSlot.Amount = UKismetMathLibrary::SelectInt(L_MaxStackAmount, TotalAmount, TotalAmount > L_MaxStackAmount);
		// SetItemSlot.Index = ItemIndex;
		// SetItemSlot.IsEquipped = true;
		// SetItemSlot.Guid = NewItem.Guid;

		const int32 TotalAmount = GetHeldAmountForEquipSlot(L_EquipSlot, InItemIndex) + NewItem.Amount;
		const int32 L_MaxStackAmount = CInventoryEquipmentLibrary::GetSlotItemMaxStackAmount(NewItem);
		const int32 L_Amount = UKismetMathLibrary::SelectInt(L_MaxStackAmount, TotalAmount, TotalAmount > L_MaxStackAmount);

		//장비슬롯의 아이템을 설정
		SetItemInSlot(L_EquipSlot, FItemSlot(NewItem.Item, L_Amount, InItemIndex, true, NewItem.Guid), InItemIndex);

		//장착아이템을 업데이트한다
		OnEquipItemUpdated(GetEquippedSlotItem(L_EquipSlot, InItemIndex));

		//장착슬롯이 업데이트 되었을 때 호출됩니다
		if (OnEquipSlotUpdated.IsBound())
			OnEquipSlotUpdated.Broadcast(L_EquipSlot, EquipmentSlots[GetEquipSlotIndex(L_EquipSlot)],
			                             EUpdateQuickSlotType::InternalUpdate, InItemIndex);

		if (InItemIndex == GetEquipSlotActiveItemIndex(L_EquipSlot))
			ActiveSlotEquipped(L_PrevItem, NewItem, L_EquipSlot);
	}
}

void UCEquipmentManagerComponent::UnEquipItemAtIndex(EEquipSlots InEquipSlot, int32 InIndex)
{
	const FItemSlot L_ItemToUnEquip = GetEquippedSlotItem(InEquipSlot, InIndex);

	//아이템이 유효한 경우에만
	if (CInventoryEquipmentLibrary::OnItemValid(L_ItemToUnEquip))
	{
		//아이템 슬롯 설정
		SetItemInSlot(InEquipSlot, FItemSlot(0, InIndex, false), InIndex);
		OnEquipItemUpdated(FItemSlot(L_ItemToUnEquip.Item, 0, L_ItemToUnEquip.Index, false, L_ItemToUnEquip.Guid));

		if (OnEquipSlotUpdated.IsBound())
			OnEquipSlotUpdated.Broadcast(InEquipSlot, FEquipmentSlot(), EUpdateQuickSlotType::InternalUpdate, InIndex);

		//장착이 활성화 된 슬롯의 아이템과 지정한인덱스가 같다면 활성화된슬롯의 장착을 해재합니다
		if (InIndex == GetEquipSlotActiveItemIndex(InEquipSlot))
			ActiveSlotUnEquipped(InEquipSlot, L_ItemToUnEquip);
	}
}

void UCEquipmentManagerComponent::ActiveSlotEquipped(FItemSlot PrevItem, FItemSlot NewItem, EEquipSlots SlotToEquipItem)
{
	//장착할 장비의 슬롯을 설장합니다
	const EEquipSlots L_SlotToEquipItem = SlotToEquipItem;

	//장비슬롯을 활성화 합니다
	SetEquipSlotDisabled(L_SlotToEquipItem, false);
	//새로운 아이템 유형을 활성화로 설정합니다
	ActiveEquippedItemTypes.Add(CInventoryEquipmentLibrary::GetSlotItemType(NewItem), true);

	//1. 액터를 스폰합니다
	{
		//액터를 월드에 업데이트 성공 했다면
		if (UpdateWorldActor(L_SlotToEquipItem, NewItem))
		{
			//해당 아이템이 무기라면
			if (CInventoryEquipmentLibrary::IsSlotItemWeapon(NewItem))
			{
				EAttachType AttachType;
				bIsCombatEnabled ? AttachType = EAttachType::EquipSocket : AttachType = EAttachType::SheathSocket;
				AttachEquipSlotActor(L_SlotToEquipItem, false, FName(TEXT("None")), AttachType);
			}
			else //해당 아이템이 무기가 아니라면
				AttachEquipSlotActor(L_SlotToEquipItem, false, FName(TEXT("None")), EAttachType::EquipSocket);
		}
	}


	//2.
	{
		//2.1아이템 유형에 따른 활성 항목 숨기기/숨기기 해제
		ActiveSlotVisibilitySetting(NewItem);

		//2.2아이템 유형에 따른 전투유형 설정

		FItemSlot L_NewEquipItem;
		//아이템 타입에 따른 로직실행 
		switch (CInventoryEquipmentLibrary::GetSlotItemType(NewItem))
		{
		case EItemType::MeleeWeapon:
			UpdateCombatType(CInventoryEquipmentLibrary::GetSlotItemWeaponType(NewItem), CInventoryEquipmentLibrary::GetSlotItemType(NewItem));
			break;

		case EItemType::RangedWeapon:
			UpdateCombatType(CInventoryEquipmentLibrary::GetSlotItemWeaponType(NewItem), CInventoryEquipmentLibrary::GetSlotItemType(NewItem));
		//탄약유형이 아처유형이라면
			if (CInventoryEquipmentLibrary::IsAmmoArchery(GetActiveEquippedSlotItem(EEquipSlots::AmmoSlot)))
			{
				//아처무기가 유효하다면
				if (IsValid(ArcheryWeaponEquipped))
					ArcheryWeaponEquipped->UpdateArrowMesh();
			}
			break;

		case EItemType::Ammo:
			L_NewEquipItem =
				FItemSlot(CInventoryEquipmentLibrary::GetItemAmmoQuiverItem(NewItem), GetEquipSlotActiveItemIndex(EEquipSlots::QuiverSlot), 0, true);

			EquipItem(L_NewEquipItem, 0, EEquipSameItemDecision::Replace);

		//무기슬롯의 아이템이 유형이 원거리무기 유형이고 (그리고) 탄약이 필요한 활이라면
			if (CInventoryEquipmentLibrary::IsSlotItemTypeEqual(GetActiveEquippedSlotItem(EEquipSlots::WeaponSlot), EItemType::RangedWeapon) &&
				CInventoryEquipmentLibrary::IsAmmoArchery(NewItem))
			{
				if (IsValid(ArcheryWeaponEquipped))
					ArcheryWeaponEquipped->UpdateArrowMesh();
			}
			break;


		case EItemType::Shield:
			UpdateCombatType(GetCurrentCombatType(), CInventoryEquipmentLibrary::GetSlotItemType(NewItem));
			break;


		default: break; /////////////Default Case//////////////////////////
		}
	}

	//3.활성화슬롯이 장착되었을 때 호출됩니다
	if (OnActiveSlotEquipped.IsBound())
		OnActiveSlotEquipped.Broadcast(NewItem, L_SlotToEquipItem, GetEquipSlotActiveItemIndex(L_SlotToEquipItem));
}

void UCEquipmentManagerComponent::ActiveSlotVisibilitySetting(const FItemSlot& NewItem)
{
	switch (CInventoryEquipmentLibrary::GetSlotItemType(NewItem))
	{
		bool L_IsShieldDisable;
		bool L_NotOneHand;
		bool L_AmmoDisable;
		bool L_NotAmmoType;
		bool L_NotEqualRangeWeapon;
		bool L_ValidExtraWeapon;

	case EItemType::MeleeWeapon:
		//전투활성화 상태에 따라 활성화 상태를 설정한다
		DisableEquipSlot(EEquipSlots::OffHand_WeaponSlot, !bIsCombatEnabled);
		DisableEquipSlot(EEquipSlots::WeaponSlot, !bIsCombatEnabled);

	//무기의 손 유형에 따른 로직처리
		switch (CInventoryEquipmentLibrary::GetSlotWeaponHandType(NewItem))
		{
		//한손 유형이라면
		case EItemHandType::OneHand:
			//새로운 아이템의 유형이 근접무기가 아니거나 (또는) 아니고 쉴드슬롯이 활성화되어 있지 않다면
			L_IsShieldDisable = !(IsItemTypeEquippedToActiveSlot(EItemType::Shield) &&
				CInventoryEquipmentLibrary::IsSlotItemTypeEqual(NewItem, EItemType::MeleeWeapon));
			DisableEquipSlot(EEquipSlots::ShieldSlot, L_IsShieldDisable || !bIsCombatEnabled); //쉴드슬롯의 활성화상태를 설정합니다
			break;

		case EItemHandType::TwoHanded:
		case EItemHandType::DualWield:
			//아이템의 손 유형이 한손유형이 아니라면
			L_NotOneHand =
				CInventoryEquipmentLibrary::GetSlotWeaponHandType(GetActiveEquippedSlotItem(EEquipSlots::WeaponSlot)) != EItemHandType::OneHand;
		//추가무기 아이템이 유효하다면
			L_ValidExtraWeapon = CInventoryEquipmentLibrary::OnItemValid(GetActiveEquippedSlotItem(EEquipSlots::OffHand_WeaponSlot));
			DisableEquipSlot(EEquipSlots::ShieldSlot, L_NotOneHand || L_ValidExtraWeapon || !bIsCombatEnabled);
			break;

		default: break; ////////////////////////Default Break////////////////////////
		}
		break;

	case EItemType::RangedWeapon:
		//전투활성화 상태에 따라 활성화 상태를 설정한다
		DisableEquipSlot(EEquipSlots::OffHand_WeaponSlot, !bIsCombatEnabled);
		DisableEquipSlot(EEquipSlots::WeaponSlot, !bIsCombatEnabled);

	//무기의 손 유형에 따른 로직처리
		switch (CInventoryEquipmentLibrary::GetSlotWeaponHandType(NewItem))
		{
		//한손 유형이라면
		case EItemHandType::OneHand:
			//새로운 아이템의 유형이 근접무기가 아니거나 (또는) 아니고 쉴드슬롯이 활성화되어 있지 않다면
			L_IsShieldDisable = !(IsItemTypeEquippedToActiveSlot(EItemType::Shield) &&
				CInventoryEquipmentLibrary::IsSlotItemTypeEqual(NewItem, EItemType::MeleeWeapon));
			DisableEquipSlot(EEquipSlots::ShieldSlot, L_IsShieldDisable || !bIsCombatEnabled); //쉴드슬롯의 활성화상태를 설정합니다
			break;

		case EItemHandType::TwoHanded:
		case EItemHandType::DualWield:
			//아이템의 손 유형이 한손유형이 아니라면
			L_NotOneHand =
				CInventoryEquipmentLibrary::GetSlotWeaponHandType(GetActiveEquippedSlotItem(EEquipSlots::WeaponSlot)) != EItemHandType::OneHand;
		//추가무기 아이템이 유효하다면
			L_ValidExtraWeapon = CInventoryEquipmentLibrary::OnItemValid(GetActiveEquippedSlotItem(EEquipSlots::OffHand_WeaponSlot));
			DisableEquipSlot(EEquipSlots::ShieldSlot, L_NotOneHand || L_ValidExtraWeapon || !bIsCombatEnabled);
			break;

		default: break; ////////////////////////Default Break////////////////////////
		}

	//현재 무기슬롯의 아이템타입이 레인지웨폰타입 무기가 아니라면
		L_NotEqualRangeWeapon =
			!CInventoryEquipmentLibrary::IsSlotItemTypeEqual(GetActiveEquippedSlotItem(EEquipSlots::WeaponSlot), EItemType::RangedWeapon);

	//현재 무기의 사용 탄약유형 안에 현재활성화된 탄약유형아이템의 탄약유형이 포함되어 있지않다면
		L_NotAmmoType =
			!CInventoryEquipmentLibrary::GetWeaponAmmoUseTypes(GetActiveEquippedSlotItem(EEquipSlots::WeaponSlot)).Contains(
				CInventoryEquipmentLibrary::GetItemAmmoType(GetActiveEquippedSlotItem(EEquipSlots::AmmoSlot)));

		L_AmmoDisable = (L_NotEqualRangeWeapon || L_NotAmmoType) || !bIsCombatEnabled;
		DisableEquipSlot(EEquipSlots::AmmoSlot, L_AmmoDisable);

	//아처 웨폰의 무기가 유효하다면
		if (IsValid(ArcheryWeaponEquipped))
			ArcheryWeaponEquipped->UpdateArrowVisibility(L_AmmoDisable && bIsCombatEnabled);

		break;


	case EItemType::Ammo:
		//현재 무기슬롯의 아이템타입이 레인지웨폰타입 무기가 아니라면
		L_NotEqualRangeWeapon =
			!CInventoryEquipmentLibrary::IsSlotItemTypeEqual(GetActiveEquippedSlotItem(EEquipSlots::WeaponSlot), EItemType::RangedWeapon);

	//현재 무기의 사용 탄약유형 안에 현재활성화된 탄약유형아이템의 탄약유형이 포함되어 있지않다면
		L_NotAmmoType =
			!CInventoryEquipmentLibrary::GetWeaponAmmoUseTypes(GetActiveEquippedSlotItem(EEquipSlots::WeaponSlot)).Contains(
				CInventoryEquipmentLibrary::GetItemAmmoType(GetActiveEquippedSlotItem(EEquipSlots::AmmoSlot)));

		L_AmmoDisable = (L_NotEqualRangeWeapon || L_NotAmmoType) || !bIsCombatEnabled;
		DisableEquipSlot(EEquipSlots::AmmoSlot, L_AmmoDisable);

	//아처 웨폰의 무기가 유효하다면
		if (IsValid(ArcheryWeaponEquipped))
			ArcheryWeaponEquipped->UpdateArrowVisibility(L_AmmoDisable && bIsCombatEnabled);
		break;


	case EItemType::Shield:
		//아이템의 손 유형이 한손유형이 아니라면
		L_NotOneHand =
			CInventoryEquipmentLibrary::GetSlotWeaponHandType(GetActiveEquippedSlotItem(EEquipSlots::WeaponSlot)) != EItemHandType::OneHand;
	//추가무기 아이템이 유효하다면
		L_ValidExtraWeapon = CInventoryEquipmentLibrary::OnItemValid(GetActiveEquippedSlotItem(EEquipSlots::OffHand_WeaponSlot));
		DisableEquipSlot(EEquipSlots::ShieldSlot, L_NotOneHand || L_ValidExtraWeapon || !bIsCombatEnabled);
		break;

	default: break; /////////////////////////Default Break/////////////////////////
	}
}

void UCEquipmentManagerComponent::ActiveSlotUnEquipped(EEquipSlots InEquipSlot, const FItemSlot& ItemToUnEquip)
{
	//장착을 해재해야 할 아이템이 유효한 경우에만 로직을 실행합니다
	if (!CInventoryEquipmentLibrary::OnItemValid(ItemToUnEquip)) return;

	//1.장착한 아이템이 존재한다면 활성화된 슬롯에서 제거합니다
	{
		//활성화된 장착슬롯의 아이템을 가져옵니다
		ACBaseItem* L_ActiveSlotActor = GetActiveEquippedSlotActor(InEquipSlot);
		//현재 장착슬롯의 액터가 유효하다면
		if (IsValid(L_ActiveSlotActor))
		{
			L_ActiveSlotActor->OnItemUnEquipped(); //아이테을 장착해재합니다
			L_ActiveSlotActor->Destroy(); //활성화된 액터를 삭제한다
			ActiveEquippedSlotActors.Remove(InEquipSlot); //활성화된 슬롯에서 액터를 삭제한다
		}
	}


	//2. 아이템 유형에 따라 장착을 해재합니다
	{
		switch (CInventoryEquipmentLibrary::GetSlotItemType(ItemToUnEquip))
		{
		//원거리 무기인 경우
		case EItemType::RangedWeapon:
			DisableEquipSlot(EEquipSlots::AmmoSlot, true); //탄약슬롯을 비활성화시킵니다
			break;

		//탄약인 경우
		case EItemType::Ammo:
			ActiveSlotUnEquipped(EEquipSlots::QuiverSlot, GetActiveEquippedSlotItem(EEquipSlots::QuiverSlot)); //화살통 슬롯을 장착해재합니다
		//현재 탄약슬롯의 갯수가 0보다 작거나 같다면
			if (GetHeldAmountForActiveSlot(EEquipSlots::AmmoSlot) <= 0)
			{
				//활 무기가 유효하다면
				if (IsValid(ArcheryWeaponEquipped))
				{
					ArcheryWeaponEquipped->OnAmmoAttached(false);
					ArcheryWeaponEquipped->UpdateArrowVisibility(false);
				}
			}
			break;
		default: break; //Default Case Break
		}
	}


	//3. 장착을 해재한 후 전투유형과 일반저투유형을 설정합니다
	{
		//장착해재 타입의 활성화여부를 비활성화로 설정
		ActiveEquippedItemTypes.Add(CInventoryEquipmentLibrary::GetSlotItemType(ItemToUnEquip), false);

		const FItemSlot L_WeaponSlotItem = GetActiveEquippedSlotItem(EEquipSlots::WeaponSlot);
		//웨폰슬롯의 아이템이 유효하다면
		if (CInventoryEquipmentLibrary::OnItemValid(L_WeaponSlotItem))
			UpdateCombatType(CInventoryEquipmentLibrary::GetSlotItemWeaponType(L_WeaponSlotItem), GetEquippedHandItemType());
		else //웨폰 슬롯의 아이템이유 유효하지 않다면
			UpdateCombatType(GetCurrentCombatType(), GetEquippedHandItemType());
	}

	//4. 활성화된 슬롯의 장착해재되었을 때 델리게이트를 호출합니다
	if (OnActiveSlotUnequipped.IsBound())
		OnActiveSlotUnequipped.Broadcast(ItemToUnEquip, InEquipSlot, GetEquipSlotActiveItemIndex(InEquipSlot));
}

void UCEquipmentManagerComponent::RemoveItemQuantity(EEquipSlots InEquipSlot, int32 AmountToRemove, int32 ItemIndex)
{
	const FItemSlot L_ItemToUnEquip = GetEquippedSlotItem(InEquipSlot, ItemIndex); //장착을 해재할 아이템을 정보를 가져옵니다

	//장착을 해재할 아이템이 유효한 경우에만
	if (CInventoryEquipmentLibrary::OnItemValid(L_ItemToUnEquip))
	{
		//인벤토리 컴포넌트가 유효하다면
		if (IsValid(InventoryComponent))
		{
			///나중에 인벤토리 로직을 추가합니다
			// //아이템 타입에 따른 로직
			// switch (InventoryEquipmentLibrary::GetSlotItemType(L_ItemToUnEquip))
			// {
			// case EItemType::Ammo:
			// case EItemType::Tool:
			// 	SwitchEquipSlotActiveIndex(InEquipSlot);
			// 	break;
			//
			// default: break; //////////////////DefaultBreak//////////////////
			// }
		}
		else //인벤토리컴포넌트가 유효하지 않다면
		{
			const int32 L_Amount = L_ItemToUnEquip.Amount + (AmountToRemove * -1); //조정해야 할 수량을 설정합니다
			UpdateSlotItemAmount(InEquipSlot, ItemIndex, L_Amount); //장착슬롯에서 수량을 조정합니다

			//장착 슬롯이 업데이트 되었을때
			if (OnEquipSlotUpdated.IsBound())
				OnEquipSlotUpdated.Broadcast(InEquipSlot, GetAllEquipmentSlots()[GetEquipSlotIndex(InEquipSlot)],
				                             EUpdateQuickSlotType::Update, ItemIndex);


			if (L_Amount > 0) return;; //조정할 수량이 0보다 크다면 함수를 종료합니다

			UnEquipItemAtIndex(InEquipSlot, ItemIndex); //인덱스 아이템을 장착해재합니다
		}


		//아이템 타입에 따른 로직
		switch (CInventoryEquipmentLibrary::GetSlotItemType(L_ItemToUnEquip))
		{
		case EItemType::Ammo:
		case EItemType::Tool:
			SwitchEquipSlotActiveIndex(InEquipSlot);
			break;
		default: break; //////////////////DefaultBreak//////////////////
		}
	}
}

bool UCEquipmentManagerComponent::AttachEquipSlotActor(EEquipSlots InEquipSlot, bool InUseCustomSocket, FName InSocketName, EAttachType InAttachType)
{
	ACBaseItem* L_AttachItemActor = GetActiveEquippedSlotActor(InEquipSlot);
	//장착활성화된 슬롯의 액터가 유효하다면
	if (IsValid(L_AttachItemActor))
	{
		UCRPGItemAsset* L_ItemDataAsset = GetActiveEquippedSlotActor(InEquipSlot)->DataAsset;
		//아이템 에셋이 유효하다면
		if (IsValid(L_ItemDataAsset))
		{
			//어태치 타입이 장착일 경우
			if (InAttachType == EAttachType::EquipSocket)
			{
				//장착 소켓
				FName L_EquipSocket;
				//커스텀 소켓이라면
				if (InUseCustomSocket)
					L_EquipSocket = InSocketName;
				else //커스텀 소켓이 아니라면
					if (L_ItemDataAsset->EquipSocket.Contains(InEquipSlot))
						L_EquipSocket = *L_ItemDataAsset->EquipSocket.Find(InEquipSlot);

				//액터를 장착소켓에 어태치 합니다
				const bool IsSuccess = L_AttachItemActor->AttachActor(L_EquipSocket, InEquipSlot == EEquipSlots::OffHand_WeaponSlot);

				if (OnEquipSlotActorAttached.IsBound())
					OnEquipSlotActorAttached.Broadcast(InEquipSlot, L_EquipSocket, L_AttachItemActor, GetActiveEquippedSlotItem(InEquipSlot));

				return IsSuccess;
			}
			//어태치 타입이 장착해재일 경우
			if (InAttachType == EAttachType::SheathSocket)
			{
				FName L_EquipSocket;
				if (InUseCustomSocket)
					L_EquipSocket = InSocketName;
				else
					L_EquipSocket = L_ItemDataAsset->GetUnEquipSocketForEquipSlot(InEquipSlot);

				//액터를 장착해재 소켓에 어태치합니다
				const bool IsSuccess = L_AttachItemActor->AttachActor(L_EquipSocket, InEquipSlot == EEquipSlots::OffHand_WeaponSlot);

				if (OnEquipSlotActorAttached.IsBound())
					OnEquipSlotActorAttached.Broadcast(InEquipSlot, L_EquipSocket, L_AttachItemActor, GetActiveEquippedSlotItem(InEquipSlot));

				return IsSuccess;
			}
		}
	}
	return false;
}

void UCEquipmentManagerComponent::DisableEquipSlot(EEquipSlots InEquipSlot, bool InDisable)
{
	//비활성화 시킬 아이템을 설정
	 const FItemSlot L_EquipSlotToDisable = GetActiveEquippedSlotItem(InEquipSlot);

	//비활성화시킬 아이템의 정보가 유효한 경우에만
	if (CInventoryEquipmentLibrary::OnItemValid(L_EquipSlotToDisable))
	{
		SetEquipSlotDisabled(InEquipSlot, InDisable); //장착슬롯 활성화 설정

		if (OnSlotDisabledUpdated.IsBound())
			OnSlotDisabledUpdated.Broadcast(InEquipSlot, L_EquipSlotToDisable, InDisable);

		//비활성화 시킬 아이템이 무기라면
		if (CInventoryEquipmentLibrary::IsSlotItemWeapon(L_EquipSlotToDisable))
		{
			EAttachType AttachType;
			InDisable ? AttachType = EAttachType::SheathSocket : AttachType = EAttachType::EquipSocket;
			AttachEquipSlotActor(InEquipSlot, false, FName(TEXT("None")), AttachType);
		}
	}
}

void UCEquipmentManagerComponent::DisableEquippedWeapons(bool InDisable)
{
	//추가무기슬롯의 액터가 유효하다면
	if (IsValid(GetActiveEquippedSlotActor(EEquipSlots::OffHand_WeaponSlot)))
		DisableEquipSlot(EEquipSlots::OffHand_WeaponSlot, InDisable);
	else //추가무기슬롯의 액터가 유효하지 않다면
	{
		//웨폰무기슬롯의 액터가 유효하다면
		if (IsValid(GetActiveEquippedSlotActor(EEquipSlots::WeaponSlot)))
			DisableEquipSlot(EEquipSlots::WeaponSlot, InDisable);
	}

	//쉴드슬롯의 액터가 유효하다면
	if (IsValid(GetActiveEquippedSlotActor(EEquipSlots::ShieldSlot)))
		DisableEquipSlot(EEquipSlots::ShieldSlot, InDisable);
}

void UCEquipmentManagerComponent::SetSlotDisabled(EEquipSlots InEquipSlot, bool InDisable)
{
	const FItemSlot L_EquipSlotToDisable = GetActiveEquippedSlotItem(InEquipSlot);

	//장착슬롯의 아이템이 유효한 경우에만
	if (CInventoryEquipmentLibrary::OnItemValid(L_EquipSlotToDisable))
	{
		SetEquipSlotDisabled(InEquipSlot, InDisable);

		if (OnSlotDisabledUpdated.IsBound())
			OnSlotDisabledUpdated.Broadcast(InEquipSlot, L_EquipSlotToDisable, InDisable);
	}
}

ACBaseItem* UCEquipmentManagerComponent::GetActiveEquippedSlotActor(EEquipSlots InEquipSlot)
{
	if (ActiveEquippedSlotActors.Contains(InEquipSlot))
		return *ActiveEquippedSlotActors.Find(InEquipSlot);
	return nullptr;
}

bool UCEquipmentManagerComponent::IsItemTypeEquippedToActiveSlot(EItemType InItemType)
{
	if (ActiveEquippedItemTypes.Contains(InItemType))
		return *ActiveEquippedItemTypes.Find(InItemType);
	return false;
}

FItemSlot UCEquipmentManagerComponent::GetActiveEquippedSlotItem(EEquipSlots InEquipSlot)
{
	FEquipmentSlot L_EquipSlot = EquipmentSlots[GetEquipSlotIndex(InEquipSlot)];
	if (L_EquipSlot.Items.IsValidIndex(L_EquipSlot.ActiveItemIndex))
		return L_EquipSlot.Items[L_EquipSlot.ActiveItemIndex];
	return {};
}

int32 UCEquipmentManagerComponent::GetEquipSlotActiveItemIndex(EEquipSlots InEquipSlot)
{
	const int32 L_EquipSlotIndex = GetEquipSlotIndex(InEquipSlot);
	return EquipmentSlots[L_EquipSlotIndex].ActiveItemIndex;
}

bool UCEquipmentManagerComponent::IsEquipSlotActive(EEquipSlots InEquipSlot)
{
	return !EquipmentSlots[GetEquipSlotIndex(InEquipSlot)].bDisabled;
}

// void UCEquipmentManagerComponent::SetEquippedItemActor(EEquipSlots InEquipSlot, ACBaseItem* InWorldActor)
// {
// 	GetActiveEquippedSlotActors().Add(InEquipSlot, InWorldActor);
// }

void UCEquipmentManagerComponent::SetActiveEquippedItemActor(EEquipSlots InEquipSlot, ACBaseItem* InWorldActor)
{
	ActiveEquippedSlotActors.Add(InEquipSlot, InWorldActor);
}

UAnimInstance* UCEquipmentManagerComponent::GetAnimInstance()
{
	return OwnerCharacterRef->GetMesh()->GetAnimInstance();
}

USkeletalMeshComponent* UCEquipmentManagerComponent::GetOwnerMesh()
{
	return OwnerCharacterRef->GetMesh();
}

void UCEquipmentManagerComponent::EventOn_OwnerDestroyed(AActor* DestroyedActor)
{
	GetOwner()->OnDestroyed.RemoveDynamic(this, &UCEquipmentManagerComponent::EventOn_OwnerDestroyed); //바인딩을 제거합니다

	for (const auto ActiveEquippedSlotActor : ActiveEquippedSlotActors)
	{
		if (IsValid(ActiveEquippedSlotActor.Value))
			ActiveEquippedSlotActor.Value->Destroy();
	}
}

void UCEquipmentManagerComponent::OnInventorySlotUpdated(FItemSlot InItem)
{
	EEquipSlots L_FindEquipSlot;
	int32 L_FindIndex;
	//아이템을 찾았다면
	if (FindItem(InItem, L_FindEquipSlot, L_FindIndex))
	{
		FItemSlot NewItemSlot;
		NewItemSlot.Item = InItem.Item;
		NewItemSlot.Amount = InItem.Amount;
		NewItemSlot.Index = L_FindIndex;
		NewItemSlot.bIsEquipped = InItem.bIsEquipped;
		NewItemSlot.Guid = InItem.Guid;

		//아이템의 수량이 1보다 크거나 같다면
		if (InItem.Amount >= 1)
			SetEquipSlot(L_FindEquipSlot, NewItemSlot, NewItemSlot.Index, EUpdateQuickSlotType::Update);
		else //아이템의 수량이 1보다 작다면
			UnEquipItemAtIndex(L_FindEquipSlot, NewItemSlot.Index);
	}
}

void UCEquipmentManagerComponent::OnEquipSlotActorSpawned(EEquipSlots InEquipSlots, ACBaseItem* InWorldItemActor)
{
	switch (InEquipSlots)
	{
	case EEquipSlots::WeaponSlot:
		//데이터 에셋이 유효하고
		if (IsValid(InWorldItemActor->DataAsset))
		{
			//원거리 무기라면
			if (InWorldItemActor->DataAsset->ITemType == EItemType::RangedWeapon)
			{
				ArcheryWeaponEquipped = Cast<ACBaseArcheryWeapon>(InWorldItemActor);
				CHECK(ArcheryWeaponEquipped!=nullptr);
			}
			else //원거리 무기가 아니라면
			{
				MainWeapon = Cast<ACBaseWeapon>(InWorldItemActor);
				CHECK(MainWeapon!=nullptr);
			}
		}
		break;

	case EEquipSlots::OffHand_WeaponSlot:
		Off_HandWeapon = Cast<ACBaseWeapon>(InWorldItemActor);
		break;

	case EEquipSlots::ShieldSlot:
		ShieldWeapon = Cast<ACBaseShield>(InWorldItemActor);
		CHECK(ShieldWeapon == nullptr);
		break;

	default: break; //////////////////Default Case Break//////////////////
	}
}

void UCEquipmentManagerComponent::OnEquipItemUpdated(const FItemSlot& InItem)
{
	if (!CInventoryEquipmentLibrary::OnItemValid(InItem)) return; //아이템이 유효하지 않다면 함수를 종료
	if (InItem.Item->ItemPanel == EInventoryPanels::None) return; //아이템 패널이 None이라면 함수를 종료

	//아이템이 장착되었다면
	if (InItem.bIsEquipped)
	{
		//인벤토리 컴포넌트가 유효하다면
		if (IsValid(InventoryComponent))
		{
			const FItemSlot L_FindItemSlot = InventoryComponent->FindItemByGuid(InItem.Item->ItemPanel, InItem.Guid);
			//찾은 아이템이 장착되어잇지 않다면
			if (!L_FindItemSlot.bIsEquipped)
				InventoryComponent->SetItemEquipped(true, InItem.Item->ItemPanel, L_FindItemSlot.Index);
		}
	}
	else //아이템이 장착되지 않았다면
	{
		//인벤토리 컴포넌트가 유효하다면
		if (IsValid(InventoryComponent))
		{
			const FItemSlot L_FindItemSlot = InventoryComponent->FindItemByGuid(InItem.Item->ItemPanel, InItem.Guid);
			//찾은 아이템이 장착되어있다면
			if (L_FindItemSlot.bIsEquipped)
				InventoryComponent->SetItemEquipped(false, InItem.Item->ItemPanel, L_FindItemSlot.Index);
		}
	}
}

bool UCEquipmentManagerComponent::UpdateWorldActor(EEquipSlots InEquipSlot, FItemSlot InItem)
{
	//1. 스폰시켜야 할 슬롯의 액터가 이미 존재한다면 지웁니다
	{
		ACBaseItem* L_EquipActorToDestroy = GetActiveEquippedSlotActor(InEquipSlot);
		if (IsValid(L_EquipActorToDestroy))
		{
			L_EquipActorToDestroy->Destroy();
			ActiveEquippedSlotActors.Remove(InEquipSlot);
		}
	}

	//스폰이 가능한지 조건을 체크합니다

	//아이템이 유효하다면
	const bool L_ValidItem = CInventoryEquipmentLibrary::OnItemValid(InItem);
	//스폰셋팅이라면
	const bool L_CanSpawn = CInventoryEquipmentLibrary::GetSlotItemEquipSpawnSetting(InItem) != EItemEquipSpawnSetting::CanNotSpawn;
	//스폰할 액터클래스를 지정
	const TSubclassOf<ACBaseItem> L_SpawnActorClass = CInventoryEquipmentLibrary::GetSlotItemMasterActorClass(InItem);
	//스폰할 액터클래스가 유효하다면
	const bool IsValidSpawnClass = UKismetSystemLibrary::IsValidClass(L_SpawnActorClass); //클래스가 유효하다면

	if (L_ValidItem && L_CanSpawn && IsValidSpawnClass)
	{
		//액터를(아이템을) 스폰합니다
		const FTransform L_SpawnTransform = GetOwner()->GetActorTransform();
		const auto L_SpawnItem = GetWorld()->SpawnActorDeferred<ACBaseItem>(L_SpawnActorClass, L_SpawnTransform, OwnerCharacterRef, nullptr,
		                                                                    ESpawnActorCollisionHandlingMethod::Undefined);
		CHECK(L_SpawnItem!=nullptr, false);
		L_SpawnItem->DataAsset = InItem.Item;
		L_SpawnItem->EquipmentComponent = this;
		UGameplayStatics::FinishSpawningActor(L_SpawnItem, L_SpawnTransform);

		//활성화된 슬롯의 액터에 추가합니다
		ActiveEquippedSlotActors.Add(InEquipSlot, L_SpawnItem);


		bool L_Hidden = false;
		switch (CInventoryEquipmentLibrary::GetSlotItemEquipSpawnSetting(InItem))
		{
		case EItemEquipSpawnSetting::Spawn_Visible:
			L_Hidden = false;
			break;
		case EItemEquipSpawnSetting::Spawn_Hidden:
			L_Hidden = true;
			break;
		case EItemEquipSpawnSetting::CanNotSpawn:
			L_Hidden = true;
			break;
		}
		//스폰설정 값에 따라 액터의 히든을 설정합니다
		L_SpawnItem->SetActorHiddenInGame(L_Hidden);
		L_SpawnItem->OnItemEquipped();

		OnEquipSlotActorSpawned(InEquipSlot, L_SpawnItem);
		return true;
	}


	return false;
}

FItemSlot UCEquipmentManagerComponent::FindNextValidEquippedItem(EEquipSlots InEquipSlot, int32 InIndex, int32& OutFoundIndex)
{
	int32 L_Index = InIndex;
	TArray<FItemSlot> L_ItemArray = GetEquippedSlotsItems(InEquipSlot);


	for (int32 i = 0; i < L_ItemArray.Num() - 2; i++)
	{		
		int32 L_NexIndex = CRPGLibrary::GetNextArrayIndex<FItemSlot>(L_ItemArray,L_Index,true);
		if (CInventoryEquipmentLibrary::OnItemValid(L_ItemArray[L_NexIndex]))
		{
			OutFoundIndex = L_NexIndex;
			return L_ItemArray[L_NexIndex];
		}
		L_Index++;
	}
	OutFoundIndex = 0;
	return {};
}

bool UCEquipmentManagerComponent::FindItem(const FItemSlot& InItem, EEquipSlots& OutEquipSlot, int32& OutIndex)
{
	if (CInventoryEquipmentLibrary::OnItemValid(InItem))
	{
		//아이템의 장착이 가능하고 && 그리드가 유효하다면
		if (InItem.bIsEquipped && UKismetGuidLibrary::IsValid_Guid(InItem.Guid))
		{
			auto L_EquipSlotItems = GetEquippedSlotsItems(InItem.Item->EquipSlot);
			for (int32 i = 0; i < L_EquipSlotItems.Num(); i++)
			{
				if (L_EquipSlotItems[i].Guid == InItem.Guid)
				{
					OutEquipSlot = InItem.Item->EquipSlot;
					OutIndex = i;
					return true;
				}
			}
		}
	}

	OutEquipSlot = EEquipSlots::None;
	OutIndex = 0;
	return false;
}
