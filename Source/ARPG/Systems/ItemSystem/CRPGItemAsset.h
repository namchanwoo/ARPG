#pragma once

#include "CoreMinimal.h"
#include "Datas/CombatData.h"
#include "Datas/ItemData.h"
#include "Datas/StatData.h"
#include "Engine/DataAsset.h"
#include "CRPGItemAsset.generated.h"


class UCBaseImpactEffect;
class ACBaseItem;


UCLASS(Blueprintable)
class ARPG_API UCRPGItemAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UCRPGItemAsset();

#pragma region                           Item Methods
public:
	/**기본 임팩트 이펙트를 가져옵니다*/
	UFUNCTION(BlueprintNativeEvent)
	TSubclassOf<UCBaseImpactEffect> GetImpactEffect(EImpactDecision InImpactDecision);
	virtual TSubclassOf<UCBaseImpactEffect> GetImpactEffect_Implementation(EImpactDecision InImpactDecision);
	
	/**아이템 수량 스텍의 남은 공간을 갖고옵니다*/
	UFUNCTION()
	int32 GetRoomOnStack(int32 InAmount);

	/**장비장착 소켓을 가져옵니다*/
	UFUNCTION(BlueprintCallable)
	FName GetEquipSocket();

	/**장착해재할 소켓을 가져옵니다*/
	UFUNCTION()
	FName GetUnEquipSocket();

	/**아이템의 물리 데미지를 가져옵니다*/
	UFUNCTION(BlueprintNativeEvent)
	float GetPhysicalDamage();
	virtual float GetPhysicalDamage_Implementation();

	/**지정된 스텟에 대해 처음 발견된 스텟 수정자를 반환합니다.*/
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FItemStatModifier GetModifierForStat(EStats InStat);

	/**아이템의 스텟값을 가져옵니다*/
	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetItemStatValue(EStats InStat);

	/**아이템을 사용합니다*/
	UFUNCTION(BlueprintCallable)
	virtual void UseItem(AActor* InCaller);

	/**아이템의 유형이 무기인가? 무기라면 true를 반환합니다 */
	UFUNCTION(BlueprintCallable)
	bool IsWeapon();

	/**지정한 장비슬롯의 장착 소켓을 찾습니다*/
	FName FindEquipSocket(EEquipSlots InEquipSlot);

	/**지정한 장비슬롯의 장착 해재 소켓을 찾습니다*/
	FName FindUnEquipSocket(EEquipSlots InEquipSlot);

	/**중첩이 가능한 아이템이라면 true,아니라면 false를 반환합니다*/
	UFUNCTION(BlueprintCallable)
	bool CanStack();

	/**재판매할 때의 값을 가져옵니다*/
	UFUNCTION(BlueprintCallable)
	int32 GetResellValue();

	/**장착 슬롯에 대한 장착 해제 소켓 가져오기*/
	UFUNCTION(BlueprintCallable)
	FName GetUnEquipSocketForEquipSlot(EEquipSlots InEquipSlot);

#pragma endregion                        Item Methods


/**************************************Field Members**************************************/

#pragma region                           World Item Info Members
public:
	/**아이템 메쉬*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="RPG Item | World Item Info")
	UStaticMesh* ItemMesh;

	/**아이템 스켈레탈 메쉬*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="RPG Item | World Item Info")
	USkeletalMesh* ItemSkeletalMesh;

	/**아이템 클래스 레퍼런스*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="RPG Item | World Item Info")
	TSubclassOf<ACBaseItem> WorldItemActorClass;

	/**아이템 스폰 설정*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="RPG Item | World Item Info")
	EItemEquipSpawnSetting ItemEquipSpawnSetting;

	/**상대 트렌스폼*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="RPG Item | World Item Info")
	FTransform RelativeTransform;
#pragma endregion                        World Item Info Members


#pragma region                           Item Info Members
public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="RPG Item | Item Info")
	FName Item_ID;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="RPG Item | Item Info")
	EItemType ITemType;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="RPG Item | Item Info")
	EInventoryPanels ItemPanel;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="RPG Item | Item Info")
	FText ItemName;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="RPG Item | Item Info")
	FText ItemDescription;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="RPG Item | Item Info")
	EItemRarity ItemRarity;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="RPG Item | Item Info")
	FSlateBrush ItemIcon;

	/**아이템이 사용가능한가?*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="RPG Item | Item Info")
	bool bCanBeUsed;

	/**아이템이 퀘스트용인가?*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="RPG Item | Item Info")
	bool IsQuestItem;

	/**최대 수량*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="RPG Item | Item Info")
	int32 MaxCount;

#pragma endregion                        Item Info Members


#pragma region                           Item Equip Members
public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="RPG Item | Item Equip")
	EEquipSlots EquipSlot;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="RPG Item | Item Equip")
	TMap<EEquipSlots, FName> EquipSocket;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="RPG Item | Item Equip")
	TMap<EUnEquipSlots, FName> UnEquipSocket;

#pragma endregion                        Item Equip Members


#pragma region                           Item Stats Members
public:
	/**아이템이 가지고 있는 스텟*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="RPG Item | Item Stats")
	TMap<EStats, FItemStatModifier> ItemStats;

	/**아이템 가격*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="RPG Item | Item Stats")
	int32 Price;

	/**재판매 가치 배율*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="RPG Item | Item Stats")
	float ResellValueMultiplier;

#pragma endregion                        Item Stats Members


#pragma region                           Weapon Members
public:
	/**웨폰트레일 파티클*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="RPG Item | Weapon")
	UParticleSystem* WeaponTrail;

	/**피니셔 모드에 진입할 수 있는가 ?*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="RPG Item | Weapon")
	bool bCanEnterFinisherMode;

	/**무기가 손에 들릴 장착 유형*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="RPG Item | Weapon")
	EItemHandType WeaponHandType;

	/**무기의 전투 유형*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="RPG Item | Weapon")
	ECombatType WeaponType;

	/**무기종류와 방패*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="RPG Item | Weapon")
	ECombatType WeaponTypeWithShield;

	/**활 그리기 애니메이션 재생률*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="RPG Item | Weapon")
	float BowDrawSpeedRate;

	/**궁수 발사체 장착 소켓*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="RPG Item | Weapon")
	FName ArcheryProjectileEquipSocket;

	/***/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="RPG Item | Weapon")
	FName PlayerHandStringSocketName;

	/**탄약 유형*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="RPG Item | Weapon")
	TArray<EAmmoType> AmmoUseTypes;

	/**무기 시작 지점의 소켓*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="RPG Item | Weapon")
	FName WeaponStartSocket;

	/**무기 종료 지점의 소켓*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="RPG Item | Weapon")
	FName WeaponEndSocket;

#pragma endregion                        Weapon Members


#pragma region                           Impact Effects Members
public:
	/**일반 충격 효과*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="RPG Item | Impact Effects")
	TSubclassOf<UCBaseImpactEffect> GeneralImpactEffect;

	/**차단된 충격 효과*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="RPG Item | Impact Effects")
	TSubclassOf<UCBaseImpactEffect> BlockedImpactEffect;

	/**방어 브레이크 충격 효과*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="RPG Item | Impact Effects")
	TSubclassOf<UCBaseImpactEffect> DefenseBreakImpactEffect;

	/**완벽 방어 충격 효과*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="RPG Item | Impact Effects")
	TSubclassOf<UCBaseImpactEffect> PerfectBlockImpactEffect;

	/**페리 충격 효과*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="RPG Item | Impact Effects")
	TSubclassOf<UCBaseImpactEffect> ParryImpactEffect;
#pragma endregion                        Impact Effects Members


#pragma region                           Armor Info Members
public:
	/**갑옷 유형*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="RPG Item | Armor Info")
	EArmorType ArmorType;

	/**갑옷 발자국 소리를 사용하시겠습니까?*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="RPG Item | Armor Info")
	bool UseArmorFootstepSounds;

	/**갑옷 발자국 소리*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="RPG Item | Armor Info")
	TMap<TEnumAsByte<EPhysicalSurface>, USoundBase*> ArmorFootstepSounds;

#pragma endregion                        Armor Info Members


#pragma region                           Ammo Members
public:
	/**탄약 유형*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="RPG Item | Ammo")
	EAmmoType AmmoType;
	
	/**화살통 아이템*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="RPG Item | Ammo")
	UCRPGItemAsset* QuiverItem;
#pragma endregion                        Ammo Members


#pragma region                           Item Animations Members
public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="RPG Item | Item Animations")
	TSubclassOf<UAnimInstance> AnimationBlueprint;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="RPG Item | Item Animations")
	FItemMontage ItemMontages;
#pragma endregion                        Item Animations Members


#pragma region                           Tool Members
public:
	/**도구유형*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="RPG Item | Tools")
	EToolType ToolType;

	/**수정될 스텟*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="RPG Item | Tools")
	EStats StatToModify;

	/**수정 비율*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="RPG Item | Tools")
	int32 ModificationPercentage;

	/**아이템 상태효과*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="RPG Item | Tools")
	FStatusEffect ItemStatusEffect;
#pragma endregion                        Tool Members
};
