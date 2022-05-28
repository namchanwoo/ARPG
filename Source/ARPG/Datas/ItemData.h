#pragma once

#include "CoreMinimal.h"
#include "CharacterData.h"
#include "CombatData.h"
#include "Engine/DataTable.h"
#include "ItemData.generated.h"


/**장착슬롯 유형*/
UENUM(BlueprintType)
enum class EEquipSlots : uint8
{
	/**아무것도 아닌*/
	None UMETA(DisplayName = "None") ,
	/**무기슬롯*/
	WeaponSlot UMETA(DisplayName = "Weapon Slot") ,
	/**왼손 혹은 추가무기 슬롯*/
	OffHand_WeaponSlot UMETA(DisplayName = "Off-hand Weapon Slot") ,
	/**방패 슬롯*/
	ShieldSlot UMETA(DisplayName = "Shield Slot") ,
	/**머리 슬롯*/
	HeadSlot UMETA(DisplayName = "Head Slot") ,
	/**상체 슬롯*/
	ChestSlot UMETA(DisplayName = "Chest Slot") ,
	/**장갑 슬롯*/
	GauntletSlot UMETA(DisplayName = "Gauntlet Slot") ,
	/**무릎(다리) 유형*/
	GreavesSlot UMETA(DisplayName = "Greaves Slot") ,
	/**화살통 슬롯*/
	QuiverSlot UMETA(DisplayName = "Quiver Slot") ,
	/**탄약 슬롯*/
	AmmoSlot UMETA(DisplayName = "Ammo Slot") ,
	/**도구 슬롯*/
	ToolsSlot UMETA(DisplayName = "Tools Slot") ,
	/**반지 슬롯*/
	RingSlot UMETA(DisplayName = "Ring Slot"),
	/**MAX*/
	MAX UMETA(DisplayName = "MAX")
};


/**퀵슬롯 유형*/
UENUM(BlueprintType)
enum class EQuickSlots : uint8
{
	/**아무것도 아닌*/
	None UMETA(DisplayName = "None") ,
	/**무기*/
	Weapon UMETA(DisplayName = "Weapon") ,
	/**방패*/
	Shield UMETA(DisplayName = "Shield") ,
	/**도구*/
	Tools UMETA(DisplayName = "Tools") ,
	/**탄약*/
	Ammo UMETA(DisplayName = "Ammo")
};


/**장비해재 슬롯 유형*/
UENUM(BlueprintType)
enum class EUnEquipSlots : uint8
{
	/**아무것도 아닌*/
	None UMETA(DisplayName = "None") ,
	/**무기 해재 슬롯 */
	WeaponSheath UMETA(DisplayName = "Weapon Sheath") ,
	/**추가 무기 해재 슬롯*/
	ExtraWeaponSheath UMETA(DisplayName = "Extra Weapon Sheath") ,
	/**쉴드 해재 슬롯*/
	ShieldSheathSlot UMETA(DisplayName = "Shield Sheath Slot") ,
};


/**아이템 유형*/
UENUM(BlueprintType)
enum class EItemType : uint8
{
	/**아무것도 아닌*/
	None UMETA(DisplayName = "None") ,
	/**근접 무기*/
	MeleeWeapon UMETA(DisplayName = "Melee Weapon") ,
	/**원거리 무기*/
	RangedWeapon UMETA(DisplayName = "Ranged Weapon") ,
	/**갑옷*/
	Armor UMETA(DisplayName = "Armor") ,
	/**도구*/
	Tool UMETA(DisplayName = "Tool") ,
	/**탄약*/
	Ammo UMETA(DisplayName = "Ammo") ,
	/**화살통*/
	Quiver UMETA(DisplayName = "Quiver") ,
	/**방패*/
	Shield UMETA(DisplayName = "Shield") ,
	/**반지*/
	Ring UMETA(DisplayName = "Ring")
};


/**아이템 장착 핸드 유형*/
UENUM(BlueprintType)
enum class EItemHandType : uint8
{
	/**아무것도 아닌*/
	None UMETA(DisplayName = "None") ,
	/**한손 유형*/
	OneHand UMETA(DisplayName = "One Handed") ,
	/**양손 유형*/
	TwoHanded UMETA(DisplayName = "Two Handed") ,
	/**쌍수 유형*/
	DualWield UMETA(DisplayName = "Dual Wield")
};


/**아이템 스폰 설정 유형*/
UENUM(BlueprintType)
enum class EItemEquipSpawnSetting : uint8
{
	/**스폰(보이는)*/
	Spawn_Visible UMETA(DisplayName = "Spawn (visible)") ,
	/**스폰(숨겨진)*/
	Spawn_Hidden UMETA(DisplayName = "Spawn (hidden)") ,
	/**스폰하지 않습니다*/
	CanNotSpawn UMETA(DisplayName = "Don't spawn")
};


/**아이템 등급 유형 */
UENUM(BlueprintType)
enum class EItemRarity : uint8
{
	/**아무것도 아닌*/
	None UMETA(DisplayName = "None") ,
	/**흔한(보통) */
	Common UMETA(DisplayName = "Common") ,
	/**희귀한 */
	Rare UMETA(DisplayName = "Rare") ,
	/**서사*/
	Epic UMETA(DisplayName = "Epic") ,
	/**전설 */
	Legendary UMETA(DisplayName = "Legendary") ,
};


/**인벤토리 패널 유형*/
UENUM(BlueprintType)
enum class EInventoryPanels : uint8
{
	/**아무것도 아닌*/
	None UMETA(DisplayName = "None") ,
	/**기본 패널*/
	DefaultPanel UMETA(DisplayName = "Default Panel") ,
	/**패널1 */
	Panel1 UMETA(DisplayName = "Panel 1") ,
	/**패널2*/
	Panel2 UMETA(DisplayName = "Panel 2") ,
	/**패널3*/
	Panel3 UMETA(DisplayName = "Panel 3") ,
	/**패널4 */
	Panel4 UMETA(DisplayName = "Panel 4") ,
	/**패널5*/
	Panel5 UMETA(DisplayName = "Panel 5") ,
	/**패널6*/
	Panel6 UMETA(DisplayName = "Panel 6")
};


/**갑옷 유형*/
UENUM(BlueprintType)
enum class EArmorType : uint8
{
	/**아무것도 아닌*/
	None UMETA(DisplayName = "None") ,
	/**핼맷(머리에쓰는) */
	HeadGear UMETA(DisplayName = "Headgear") ,
	/**가슴 갑옷 */
	ChestArmor UMETA(DisplayName = "ChestArmor"),
	/**장갑*/
	Gauntlets UMETA(DisplayName = "Gauntlets"),
	/**다리 갑옷*/
	TwoHanded UMETA(DisplayName = "Greaves")
};


/**탄약 유형*/
UENUM(BlueprintType)
enum class EAmmoType : uint8
{
	/**아무것도 아닌*/
	None UMETA(DisplayName = "None") ,
	/**화살 유형*/
	Arrows UMETA(DisplayName = "Arrows") ,
	/**볼트 유형*/
	Bolts UMETA(DisplayName = "Bolts")
};

/**장비가 동일한 아이템 결정유형*/
UENUM(BlueprintType)
enum class EEquipSameItemDecision : uint8
{
	/**업데이트하다*/
	Update UMETA(Display = "Update"),
	/**바꾸다*/
	Replace UMETA(Display = "Replace")
};


/**탄약 유형*/
UENUM(BlueprintType)
enum class EToolType : uint8
{
	/**아무것도 아닌*/
	None UMETA(DisplayName = "None") ,
	/**소모품 */
	Consumable UMETA(DisplayName = "Consumable") ,
	/**발사체*/
	Projectile UMETA(DisplayName = "Projectile")
};

/**메쉬의 유형*/
UENUM(BlueprintType)
enum class EMeshType : uint8
{
	/**아무것도 아닌*/
	None UMETA(DisplayName = "None") ,
	StaticMesh UMETA(DisplayName = "Static Mesh") ,
	SkeletalMesh UMETA(DisplayName = "Skeletal Mesh")
};


/**무기 발사체의 상태*/
UENUM(BlueprintType)
enum class EWeaponProjectileState : uint8
{
	/**아무것도 아닌*/
	None UMETA(DisplayName = "None") ,
	/**미달 (획득되지 않음)*/
	UnObtained UMETA(DisplayName = "UnObtained") ,
	/**무기안에*/
	InWeapon UMETA(DisplayName = "InWeapon") ,
	/**공중에*/
	InAir UMETA(DisplayName = "InAir") ,
	/**손에*/
	InHand UMETA(DisplayName = "InHand") ,
	/**객체에 고정 된*/
	StuckInObject UMETA(DisplayName = "Stuck In Object") ,
};


/**퀵슬롯을 업데이트 할때의 유형*/
UENUM(BlueprintType)
enum class EUpdateQuickSlotType : uint8
{
	/**내부 업데이트*/
	InternalUpdate UMETA(Display = "InternalUpdate"),
	/**아이템 교체*/
	SwitchItems UMETA(Display = "Switch Items"),
	/**업데이트*/
	Update UMETA(Display = "Update")
};

/**어태치 타입*/
UENUM(BlueprintType)
enum class EAttachType : uint8
{
	/**장착 소켓*/
	EquipSocket UMETA(Display = "Equip Socket"),
	/**해재 소켓*/
	SheathSocket UMETA(Display = "Sheath Socket")
};


/**아이템 슬롯 구조체*/
USTRUCT(BlueprintType)
struct FItemSlot
{
	GENERATED_BODY()
public:
	/**아이템 에셋*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	class UCRPGItemAsset* Item;
	/**수량*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	int32 Amount;
	/**슬롯 인덱스*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	int32 Index;
	/**장착되어있습니까? */
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	bool bIsEquipped;
	/**식별자 */
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FGuid Guid;

public:
	FItemSlot(): Item(nullptr), Amount(0), Index(0), bIsEquipped(false)
	{
	}

	FItemSlot(int32 InAmount, int32 NewIndex, bool InEquipped): Item(nullptr)
	{
		Amount = InAmount;
		Index = NewIndex;
		bIsEquipped = InEquipped;
	}

	FItemSlot(UCRPGItemAsset* InItem, int32 InAmount, int32 NewIndex, bool InEquipped)
	{
		Item = InItem;
		Amount = InAmount;
		Index = NewIndex;
		bIsEquipped = InEquipped;
	}

	FItemSlot(UCRPGItemAsset* InItem, int32 InAmount, int32 NewIndex, bool InEquipped, FGuid InGuid)
	{
		Item = InItem;
		Amount = InAmount;
		Index = NewIndex;
		bIsEquipped = InEquipped;
		Guid = InGuid;
	}
};


/**아이템 스탯 수정 구조체*/
USTRUCT(BlueprintType)
struct FItemStatModifier
{
	GENERATED_BODY()
public:
	/**아이템 값 */
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float Value;
	/**수정 ID */
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FGuid ModifierID;
};


/**아이템 몽테이지*/
USTRUCT(BlueprintType)
struct FItemMontage
{
	GENERATED_BODY()

public:
	/**캐릭터 액션와 몽테이지들*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TMap<ECharacterAction, FActionMontage> ActionMontages;
	/**공격유형와 행동 몽테이지들*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TMap<EAttackTypes, FActionMontage> AttackMontages;
	/**구르기회피와 몽테이지들*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TMap<EDodgeDirection, FActionMontage> RollDodgeMontages;
	/**스텝회피와 몽테이지들*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TMap<EDodgeDirection, FActionMontage> StepDodgeMontages;
};


/**장착슬롯 구조체*/
USTRUCT(BlueprintType)
struct FEquipmentSlot
{
	GENERATED_BODY()
public:
	/**장착 슬롯*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	EEquipSlots EquipSlot;
	/**비활성화 여부*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	bool bDisabled;
	/**활성화된 아이템 인덱스*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	int32 ActiveItemIndex;
	/**아이템들(아이템슬롯의 정보들)*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TArray<FItemSlot> Items;

public:
	FEquipmentSlot(): EquipSlot(), bDisabled(false), ActiveItemIndex(0)
	{
	}

	FEquipmentSlot(EEquipSlots InEquipSlot): bDisabled(false), ActiveItemIndex(0)
	{
		EquipSlot = InEquipSlot;
	}

	FEquipmentSlot(EEquipSlots InEquipSlot, bool InDisabled, int32 InActiveItemIndex, TArray<FItemSlot> InItems)
	{
		EquipSlot = InEquipSlot;
		bDisabled = InDisabled;
		ActiveItemIndex = InActiveItemIndex;
		Items = InItems;
	}
};
