#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Datas/ItemData.h"
#include "CEquipmentManagerComponent.generated.h"


class ACBaseArcheryWeapon;
class ACBaseWeapon;
class ACBaseShield;
class ACBaseItem;
class UCInventoryComponent;
class UCBaseImpactEffect;
class UCBaseArcheryWeapon;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCombatTypeChanged, ECombatType, InCombatType);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCombatTypeEnd, ECombatType, InCombatType);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnSlotDisabledUpdated, EEquipSlots, InEquipSlots, FItemSlot, InSlotItem, bool, Hidden);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGeneralCombatTypeEnd, EGeneralCombatType, InGeneralCombatType);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGeneralCombatTypeChanged, EGeneralCombatType, InGeneralCombatType);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnActiveSlotEquipped, FItemSlot, NewSlotItem, EEquipSlots, InSlotEquipped, int32, InActiveIndex);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnActiveSlotUnequipped, FItemSlot, NewSlotItem, EEquipSlots, InSlotEquipped, int32, InActiveIndex);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnEquipSlotUpdated, EEquipSlots, InEquipSlot, FEquipmentSlot, EquipSlotUpdated, EUpdateQuickSlotType,
                                              UpdateType, int32, ItemIndex);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FItemUsed, FItemSlot, ItemToUse);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCombatChanged, bool, InCombatEnabled);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FEquipSlotActorAttached, EEquipSlots, InEquipSlot, FName, InSockeName, ACBaseItem*, InAttachedActor,
                                              FItemSlot, InItemAttached);


UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ARPG_API UCEquipmentManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCEquipmentManagerComponent();

protected:
	virtual void BeginPlay() override;


#pragma region                               Initialize Methods
public:
	/**장비 초기화 하기*/
	void InitializeEquipment(TArray<FEquipmentSlot> InEquipArray);

	/**캐릭터 프리뷰 초기화하기*/
	void InitializeCharacterPreview();

	/**컴포넌트 초기화 하기*/
	void InitializeEquipmentComponent();

#pragma endregion                            Initialize Methods

#pragma region                               Weapon Methods
public:
	/**무기가 장착되어 있습니까?*/
	bool IsWeaponEquipped();

	/**메인무기 또는 보조무기의 아이템이 유효하다면 아이템 유형을 가져옵니다*/
	EItemType GetEquippedHandItemType();
#pragma endregion                            Weapon Methods


#pragma region                               Equipment Methods
public:
	/**장비의 임팩트 효과를 가져옵니다*/
	TSubclassOf<UCBaseImpactEffect> GetEquipmentImpactEffect(EImpactDecision InImpactDecision, FVector& OutImpactEffectLocation);

	/**활성화된 슬롯의 아이템을 사용합니다*/
	void UseActiveSlotItem(EEquipSlots InEquipSlot);

#pragma endregion                            Equipment Methods


#pragma region                               Equip Slots Methods
public:
	/**해당 장비슬롯 유형의 아이템들을 가져옵니다*/
	TArray<FItemSlot> GetEquippedSlotsItems(EEquipSlots InEquipSlot);

	/**활성화된 장비슬롯의 인덱스를 스위치한다 */
	void SwitchEquipSlotActiveIndex(EEquipSlots InEquipSlot);

	/**장비슬롯을 설정합니다*/
	void SetEquipSlot(EEquipSlots InEquipSlot, const FItemSlot& InItem, int32 ItemIndex, EUpdateQuickSlotType UpdateType);

	/**장착된 슬롯의 아이템을 가져온다*/
	FItemSlot GetEquippedSlotItem(EEquipSlots InEquipSlot, int32 ItemIndex);

	/**장착 슬롯에 대한 수량 가져오기*/
	int32 GetHeldAmountForEquipSlot(EEquipSlots InEquipSlot, int32 InItemIndex);

	/**장비 슬롯에서 해당 슬롯유형의 인덱스를 가져옵니다 인덱스가 없다면 -1을 반환합니다*/
	int32 GetEquipSlotIndex(EEquipSlots InEquipSlot);

	/**장비 슬롯이 비활성화되어 있습니까?*/
	bool IsEquipSlotDisabled(EEquipSlots InEquipSlot);

	/**지정된 슬롯유형의 아이템 갯수를 가져옵니다 */
	int32 GetHeldAmountForActiveSlot(EEquipSlots InEquipSlot);
#pragma endregion                            Equip Slots Methods


#pragma region                               Equip / Unequip Slot Item Methods
public:
	/**아이템 장착합니다*/
	void EquipItem(const FItemSlot& NewItem, int32 InItemIndex, EEquipSameItemDecision EquipSameItemDecision);

	/**인덱스에서 아이템 장착을 해재합니다*/
	void UnEquipItemAtIndex(EEquipSlots InEquipSlot, int32 InIndex);

	/**활성화된 슬롯에 장착한다*/
	void ActiveSlotEquipped(FItemSlot PrevItem, FItemSlot NewItem, EEquipSlots SlotToEquipItem);

	void ActiveSlotVisibilitySetting(const FItemSlot& NewItem);

	/**활성화된 슬롯을 장착해재합니다*/
	void ActiveSlotUnEquipped(EEquipSlots InEquipSlot, const FItemSlot& ItemToUnEquip);

	/**항목의 수량을 제거합니다*/
	void RemoveItemQuantity(EEquipSlots InEquipSlot, int32 AmountToRemove, int32 ItemIndex);

	/**장착할 슬롯에 액터를 어태치한다*/
	bool AttachEquipSlotActor(EEquipSlots InEquipSlot, bool InUseCustomSocket, FName InSocketName, EAttachType InAttachType);
	//
	/**장비슬롯 비활성화를 설정합니다*/
	void DisableEquipSlot(EEquipSlots InEquipSlot, bool InDisable);

	/**장착 무기 비활성화를 설정합니다*/
	void DisableEquippedWeapons(bool InDisable);

	/**슬롯의 비활성화 여부를 설정합니다*/
	void SetSlotDisabled(EEquipSlots InEquipSlot, bool InDisable);

#pragma endregion                            Equip / Unequip Slot Item Methods


#pragma region                               Active Equip Slots Methods
public:
	/**활성화 된 장착슬롯의 액터 가져옵니다*/
	ACBaseItem* GetActiveEquippedSlotActor(EEquipSlots InEquipSlot);

	/**아이템 유형이 활성 슬롯에 장착되어 있습니까?*/
	bool IsItemTypeEquippedToActiveSlot(EItemType InItemType);

	/**장착이 활성화된 슬롯의 아이템을 가져옵니다*/
	FItemSlot GetActiveEquippedSlotItem(EEquipSlots InEquipSlot);

	/**장비 슬롯의 활성아이템 인덱스를 가져옵니다*/
	int32 GetEquipSlotActiveItemIndex(EEquipSlots InEquipSlot);

	/**장비 슬롯이 활성화 되어있는가?*/
	bool IsEquipSlotActive(EEquipSlots InEquipSlot);

	// /**활성화된 장착유형의 액터를 설정합니다*/
	// void SetEquippedItemActor(EEquipSlots InEquipSlot, ACBaseItem* InWorldActor);

	/**활성화된 장착유형의 액터를 설정합니다*/
	void SetActiveEquippedItemActor(EEquipSlots InEquipSlot, ACBaseItem* InWorldActor);

#pragma  endregion                            Active Equip Slots Methods


#pragma region                                Combat Methods
public:
	/**전투 유형을 설정 합니다*/
	void SetCombatType(ECombatType NewCombatType);

	/**전투타입을 업데이트 한다*/
	void UpdateCombatType(ECombatType InWeaponType, EItemType InItemType);

	/**일반 전투유형을 설정 합니다*/
	void SetGeneralCombatType(EGeneralCombatType NewGeneralCombatType);

	/**전투활성화 상태를 설정합니다*/
	void SetCombatEnabled(bool InCondition);

	/**전투상태를 토글한다*/
	void ToggleCombat();
#pragma endregion                             Combat Methods


#pragma region                               Helper Methods
private:
	/*애님 인스터스를 가져옵니다*/
	UAnimInstance* GetAnimInstance();

	/**오너캐릭터의 메쉬를 가져옵니다*/
	USkeletalMeshComponent* GetOwnerMesh();

	/**해당 슬롯안에 아이템을 설정합니다*/
	void SetItemInSlot(EEquipSlots InEquipSlot, const FItemSlot& InItem, int32 Index);
	// void SetItemInSlot(EEquipSlots InEquipSlot, UCRPGItemAsset* InItemAsset, int32 ItemAmount, int32 ItemIndex,
	//                    bool ItemEquipped, FGuid ItemIGuid, int32 Index);

	/**해당 장비 슬롯의 비활성화상태를 설정합니다*/
	void SetEquipSlotDisabled(EEquipSlots InEquipSlots, bool IsDisabled);

	/**해당 슬롯아이템의 수량을 업데이트합니다*/
	void UpdateSlotItemAmount(EEquipSlots InEquipSlot, int32 InItemIndex, int32 InAmount);

	/**해당 유형의 활성 아이템의 인덱스를 설정합니다*/
	void SetActiveItemIndex(EEquipSlots InEquipSlot, int32 ActiveItemIndex);

	/**해당 장비유형의 아이템 인덱스가 유효한가?*/
	bool OnItemIndexValid(EEquipSlots InEquipSlot, int32 ItemIndex);

#pragma endregion                            Helper Methods


public:
	/**오너의 액터가 죽거나 소멸할 때 호출되며 장비슬롯의 아이템들을 삭제합니다*/
	UFUNCTION()
	void EventOn_OwnerDestroyed(AActor* DestroyedActor);

	/**인벤토리 슬롯이 업데이트 될 때 호출됩니다*/
	UFUNCTION()
	void OnInventorySlotUpdated(FItemSlot InItem);

	/**장착슬롯이 스폰될 때 호출된다*/
	void OnEquipSlotActorSpawned(EEquipSlots InEquipSlots, ACBaseItem* InWorldItemActor);

	/**장착 아이템이 업데이트 될 때 호출됩니다*/
	void OnEquipItemUpdated(const FItemSlot& InItem);

	/**액터를 월드에 업데이트합니다*/
	bool UpdateWorldActor(EEquipSlots InEquipSlot, FItemSlot InItem);

	/**다음 유효한 장착아이템을 찾습니다 */
	FItemSlot FindNextValidEquippedItem(EEquipSlots InEquipSlot, int32 InIndex, int32& OutFoundIndex);

	/**장착 된 아이템을 찾습니다*/
	bool FindItem(const FItemSlot& InItem, EEquipSlots& FindEquipSlot, int32& FindIndex);


	/*******************************************************************************************************************FIELD MEMBERS****************/
	/*******************************************************************************************************************FIELD MEMBERS****************/
	/*******************************************************************************************************************FIELD MEMBERS****************/

#pragma region               Default Members
private:
	/**컴포넌트를 소유한 캐릭터*/
	UPROPERTY(BlueprintReadOnly, Category = "EquipmentManager | Default", meta = (AllowPrivateAccess = "true"))
	ACharacter* OwnerCharacterRef;

	//UPROPERTY(BlueprintReadOnly, Category = "Default", meta = (AllowPrivateAccess = "true"))
	//APreviewCharacter* CharacterPreview;

#pragma endregion            Default Members


#pragma region               Component Members
private:
	UPROPERTY(BlueprintReadOnly, Category = "EquipmentManager | Component", meta = (AllowPrivateAccess = "true"))
	UCInventoryComponent* InventoryComponent;
#pragma endregion            Component Members


#pragma  region              Weapons Members
private:
	/**오른 손 무기 장착*/
	UPROPERTY(BlueprintReadOnly, Category = "EquipmentManager | Weapons", meta = (AllowPrivateAccess = "true"))
	ACBaseWeapon* MainWeapon;

	/**왼 손 무기 장착*/
	UPROPERTY(BlueprintReadOnly, Category = "EquipmentManager | Weapons", meta = (AllowPrivateAccess = "true"))
	ACBaseWeapon* Off_HandWeapon;

	/**방패 장착*/
	UPROPERTY(BlueprintReadOnly, Category = "EquipmentManager | Weapons", meta = (AllowPrivateAccess = "true"))
	ACBaseShield* ShieldWeapon;
#pragma  endregion           Weapons Members


#pragma  region              Archery Members
private:
	/**활 무기 장착*/
	UPROPERTY(BlueprintReadOnly, Category = "EquipmentManager | Archery", meta = (AllowPrivateAccess = "true"))
	ACBaseArcheryWeapon* ArcheryWeaponEquipped;

	/**재 장전 여부*/
	bool bCanReload;

#pragma  endregion           Archery Members


#pragma  region              Equipment Members
private:
	/**무기 손 타입*/
	EItemHandType WeaponHandType;

#pragma  endregion           Equipment Members


#pragma  region              Combat Type Parameters
private:
	/**현재 전투 타입*/
	UPROPERTY(BlueprintReadOnly,Category="Equipment | Combat",meta=(AllowPrivateAccess="true"))
	ECombatType CurrentCombatType;

	/**이전 전투 타입*/
	ECombatType PrevCombatType;

	/**포커스상태에 진입 할 수 있는 지?*/
	bool bCanEnterFocusedState;

	/**피니셔 모드에 진입할 수 있는 지?*/
	bool bCanEnterFinisherMode;

	/**Parry를 수행 할 수 있는 지?*/
	bool bCanPerformParry;

	/**완벽한 방어를 수행할 수 있는 상태인지?*/
	bool bCanPerformPerfectBlock;

	/**회피공격을 수행할 수 있는 지?*/
	bool bCanPerformDodgeAttack;

	/**선택적 전투를 할 수 있는 활성화 목록*/
	TMap<EOptionalCombatAbilities, bool> OptionalCombatActivities;
#pragma endregion            Combat Type Parameters


#pragma  region              Equip Slots Members
private:
	/**활성화 된 장비 슬롯의 액터*/
	UPROPERTY()
	TMap<EEquipSlots, ACBaseItem*> ActiveEquippedSlotActors;

	/**활성화 된 장비 슬롯의 아이템*/
	TMap<EEquipSlots, FItemSlot> ActiveEquipmentSlots;

	/**슬롯의 숨김 상태 목록*/
	TMap<EEquipSlots, bool> IsSlotDisabled;

	/**장비 슬롯 배열*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "EquipmentManager | Equip Slots", meta=(AllowPrivateAccess="true"))
	TArray<FEquipmentSlot> EquipmentSlots;
#pragma  endregion            Equip Slots Members


#pragma region               Equip /UnEquip Slot Item Members
private:
	/**활성화된 장착 아이템 유형*/
	UPROPERTY(BlueprintReadOnly, Category = "EquipmentManager | Equip/ Unequip Slot Item", meta=(AllowPrivateAccess="true"))
	TMap<EItemType, bool> ActiveEquippedItemTypes;
#pragma endregion            Equip /UnEquip Slot Item Members


#pragma  region              Combat Members
public:
	/**현재 회피 타입*/
	EDodgeType CurrentDodgeType;

	/**일반 전투 타입*/
	EGeneralCombatType GeneralCombatType;

	/**전투가 활성화 되어있는 상태*/
	bool bIsCombatEnabled;
#pragma  endregion            Combat Members


#pragma region               Delegate Members
public:
	/**전투유형이 변경되었을 때*/
	UPROPERTY(BlueprintAssignable)
	FOnCombatTypeChanged OnCombatTypeChanged;

	/**전투유형이 종료되었을 때*/
	UPROPERTY(BlueprintAssignable)
	FOnCombatTypeEnd OnCombatTypeEnd;

	/**슬롯비활성화가 업데이트 되었을 때*/
	UPROPERTY(BlueprintAssignable)
	FOnSlotDisabledUpdated OnSlotDisabledUpdated;

	/**일반전투유형 변경이 종료되었을 때*/
	UPROPERTY(BlueprintAssignable)
	FOnGeneralCombatTypeEnd OnGeneralCombatTypeEnd;

	/**일반 전투유형이 변경되었을 때*/
	UPROPERTY(BlueprintAssignable)
	FOnGeneralCombatTypeChanged OnGeneralCombatTypeChanged;

	/**활성화 슬롯이 장착되었을 때*/
	UPROPERTY(BlueprintAssignable)
	FOnActiveSlotEquipped OnActiveSlotEquipped;

	/**활성화 슬롯이 장착해재되었을 때*/
	UPROPERTY(BlueprintAssignable)
	FOnActiveSlotUnequipped OnActiveSlotUnequipped;

	/**장착슬롯이 업데이트 되었을 때*/
	UPROPERTY(BlueprintAssignable)
	FOnEquipSlotUpdated OnEquipSlotUpdated;

	/**아이템이 사용되었을 때*/
	UPROPERTY(BlueprintAssignable)
	FItemUsed OnItemUsed;

	/**전투상태가 변경되었을때*/
	UPROPERTY(BlueprintAssignable)
	FOnCombatChanged OnCombatChanged;

	/**장착슬롯의 액터가 어태치되었을때*/
	UPROPERTY(BlueprintAssignable)
	FEquipSlotActorAttached OnEquipSlotActorAttached;

#pragma endregion            Delegate Members


#pragma region                               FORCEINLINE METHODS
public:
	/**현재 전투 타입 가져오기*/
	UFUNCTION(BlueprintCallable)
	FORCEINLINE ECombatType GetCurrentCombatType() const { return CurrentCombatType; }
	/**전투가 활성화된 상태를 가져온다*/
	FORCEINLINE bool IsCombatEnabled() const { return bIsCombatEnabled; }
	/**이전 전투 타입 가져오기*/
	FORCEINLINE ECombatType GetPrevCombatType() const { return PrevCombatType; }
	/**일반적인 전투 타입 가져오기*/
	UFUNCTION(BlueprintCallable)
	FORCEINLINE EGeneralCombatType GetGeneralCombatType() const { return GeneralCombatType; }

	/**메인웨폰을 가져옵니다*/
	FORCEINLINE ACBaseWeapon* GetEquippedWeaponActor() const { return MainWeapon; }
	/**보조핸드의 무기를 가져옵니다*/
	FORCEINLINE ACBaseWeapon* GetEquippedOff_HandWeaponActor() const { return Off_HandWeapon; }
	/**쉴드 액터 가져오기*/
	FORCEINLINE ACBaseShield* GetEquippedShieldActor() const { return ShieldWeapon; }
	/**아처 액터 가져오기*/
	FORCEINLINE ACBaseArcheryWeapon* GetArcheryWeaponEquipped() const { return ArcheryWeaponEquipped; }
	/**장비슬롯 배열 가져오기*/
	FORCEINLINE TArray<FEquipmentSlot> GetAllEquipmentSlots() const { return EquipmentSlots; }
	/**활성화된 장비슬롯의 슬롯아이템 정보들을 모두 가져오기*/
	FORCEINLINE TMap<EEquipSlots, FItemSlot> GetAllActiveEquipSlots() const { return ActiveEquipmentSlots; }
	/**활성화된 장비슬롯의 액터들을 모두 가져오기*/
	FORCEINLINE TMap<EEquipSlots, ACBaseItem*> GetActiveEquippedSlotActors() const { return ActiveEquippedSlotActors; }

	/**메인 웨폰액터를 설정합니다*/
	FORCEINLINE void SetMainWeaponActor(ACBaseWeapon* InWeaponActor) { MainWeapon = InWeaponActor; }
	/**보조 무기 웨폰을 설정합니다*/
	FORCEINLINE void SetOff_HandWeaponActor(ACBaseWeapon* InWeaponActor) { Off_HandWeapon = InWeaponActor; }
	/**활 무기를 설정합니다*/
	FORCEINLINE void SetArcherWeapon(ACBaseArcheryWeapon* InArcherWeapon) { ArcheryWeaponEquipped = InArcherWeapon; }
	/**방패 무기를 설정합니다*/
	FORCEINLINE void SetShieldWeaponActor(ACBaseShield* InShieldActor) { ShieldWeapon = InShieldActor; }
#pragma endregion                            FORCEINLINE METHODS
};
