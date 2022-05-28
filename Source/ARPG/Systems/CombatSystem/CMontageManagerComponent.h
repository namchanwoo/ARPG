#pragma once

#include "CoreMinimal.h"
#include "Datas/CombatData.h"
#include "Datas/ItemData.h"
#include "CMontageManagerComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMontagesUpdated, FDataTableRowHandle, InDataTableInfo);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ARPG_API UCMontageManagerComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	UCMontageManagerComponent();
protected:
	virtual void BeginPlay() override;

public:
	/**기본 몽테이지들을 데이터테이블의 디폴트행의 데이터로 설정*/
	void SetDefaultMontages();

	/**몽테이지를 새로 업데이트 합니다*/
	void UpdateMontages(ECombatType InCombatType, FName InCustomRowName = FName(TEXT("None")), bool UseCustomRowName = false);

	/**웨폰몽테이지를 입력받은 아이템몽테이지로 업데이트 한다*/
	void UpdateWeaponMontages(const FItemMontage& WeaponMontages);

	/**공격 몽테이지 인덱스 리셋*/
	void ResetAttackMontageIndex();


#pragma region                             Get Montage Methods
public:
	/**일반 액션 몽테이지를 반환한다*/
	FAnimMontageStruct GetGeneralActionMontage(ECharacterAction InCombatAction, int32 InMontageIndex, bool IsRandomIndex);

	/**공격 몽테이지를 반환한다*/
	FAnimMontageStruct GetAttackMontage(EAttackTypes InAttackType, int32 InAttackIndex, bool RandomIndex);

	/**아이템의 몽테이지를 반환한다*/
	FAnimMontageStruct GetActionMontageFromItem(ECharacterAction InCombatAction, int32 MontageIndex, bool RandomIndex, const FItemSlot& InSlotItem);

	/**구르기 회피 몽테이지를 반환한다*/
	FAnimMontageStruct GetRollDodgeMontage(EDodgeDirection InDodgeDirection, int32 InMontageIndex);

	/**스텝 회피 몽테이지를 반환한다*/
	FAnimMontageStruct GetStepDodgeMontage(EDodgeDirection InDodgeDirection, int32 InMontageIndex);

	/**카운터어택 몽테이지를 반환한다*/
	FAnimMontageStruct GetCounterAttackMontage(EHitFacingDirection InDirection);

#pragma endregion                          Get Montage Methods





	/**********************************************************************************************************************Field Members****************/
	/**********************************************************************************************************************Field Members****************/
	/**********************************************************************************************************************Field Members****************/

#pragma region               Default Members
private:
	/**오너캐릭터 Ref*/
	UPROPERTY(BlueprintReadOnly, Category="MontageManager | Default", meta=(AllowPrivateAccess="true"))
	ACharacter* OwnerCharacter;

	/**컴뱃 컴포넌트*/
	UPROPERTY(BlueprintReadOnly, Category="MontageManager | Component", meta=(AllowPrivateAccess="true"))
	class UCCombatComponent* CombatComponent;

	/**데이터 테이블*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="MontageManager | Initialize", meta=(AllowPrivateAccess="true"))
	UDataTable* DataTable;

	/**마지막 공격 몽테이지*/
	FAnimMontageStruct LastAttackMontage;
#pragma endregion            Default Members


#pragma region               All Montage Structs
private:
	/**일반 액션 몽테이지들*/
	TMap<ECharacterAction, FActionMontage> GeneralActionMontages;

	/**공격 몽테이지들*/
	TMap<EAttackTypes, FActionMontage> AttackMontages;

	/**전투유형의 구르기 회피 몽테이지들*/
	TMap<EDodgeDirection, FActionMontage> CombatTypeRollDodgeMontages;

	/**전투 스텝 회피 몽테이지들*/
	TMap<EDodgeDirection, FActionMontage> CombatTypeStepDodgeMontages;

	/**마무리모드의 액션 몽테이지들*/
	TMap<ECharacterAction, FActionMontage> FinisherModeActionMontages;

	/**마무리모드의 공격 몽테이지들*/
	TMap<EAttackTypes, FActionMontage> FinisherModeAttackMontages;

	/**기본 일반 액션 몽테이지들*/
	TMap<ECharacterAction, FActionMontage> DefaultGeneralActionMontages;

	/**기본 공격 몽테이지들*/
	TMap<EAttackTypes, FActionMontage> DefaultAttackMontages;

	/**기본 구르기 회피 몽테이지들*/
	TMap<EDodgeDirection, FActionMontage> DefaultRollDodgeMontages;

	/**기본 스텝 회피 몽테이지들*/
	TMap<EDodgeDirection, FActionMontage> DefaultStepDodgeMontages;

	/**무기 액션 몽테이지들*/
	TMap<ECharacterAction, FActionMontage> WeaponActionMontages;

	/**웨폰기술 몽테이지들*/
	TMap<EAttackTypes, FActionMontage> WeaponAttackMontages;

	/**무기유형의 구르기 회피 몽테이지들*/
	TMap<EDodgeDirection, FActionMontage> WeaponRollDodgeMontages;

	/**무기별 유형의 스텝 회피 몽테이지들*/
	TMap<EDodgeDirection, FActionMontage> WeaponStepDodgeMontage;

	/**마무리모드의 공격 몽테이지들*/
	TMap<EHitFacingDirection, FAnimMontageStruct> CounterAttacks;
#pragma endregion            All Montage Structs


#pragma region               Montage Index Members
private:
	/**일반 전투 액션 마지막 인덱스*/
	int32 GeneralCombatActionLastIndex;

	/**일반 전투 액션 몽테이지 인덱스*/
	int32 GeneralCombatActionMontageIndex;

	/**스텝 회피 인덱스*/
	int32 StepDodgeIndex;

	/**공격몽테이지의 마지막인덱스*/
	int32 AttackMontageLastIndex;

	/**현재 공격 몽테이지의 인덱스*/
	int32 CurrentAttackMontageIndex;
#pragma endregion            Montage Index Members

public:
	/**몽테이지가 업데이트 될 때 호출 */
	UPROPERTY(BlueprintAssignable)
	FOnMontagesUpdated OnMontagesUpdated;

#pragma region                             FORCEINLINE METHODS
public:
	FORCEINLINE TMap<ECharacterAction, FActionMontage> GetAllGeneralActionMontages() const { return GeneralActionMontages; }
	FORCEINLINE TMap<ECharacterAction, FActionMontage> GetAllFinisherModeActionMontages() const { return FinisherModeActionMontages; }
	FORCEINLINE TMap<ECharacterAction, FActionMontage> GetAllDefaultGeneralActionMontages() const { return DefaultGeneralActionMontages; }
	FORCEINLINE TMap<ECharacterAction, FActionMontage> GetAllWeaponActionMontages() const { return WeaponActionMontages; }
	FORCEINLINE TMap<EAttackTypes, FActionMontage> GetAllAttackMontages() const { return AttackMontages; }
	FORCEINLINE TMap<EAttackTypes, FActionMontage> GetAllFinisherModeAttackMontages() const { return FinisherModeAttackMontages; }
	FORCEINLINE TMap<EAttackTypes, FActionMontage> GetAllDefaultAttackMontages() const { return DefaultAttackMontages; }
	FORCEINLINE TMap<EAttackTypes, FActionMontage> GetAllWeaponAttackMontages() const { return WeaponAttackMontages; }
	FORCEINLINE TMap<EDodgeDirection, FActionMontage> GetAllCombatTypeRollDodgeMontages() const { return CombatTypeRollDodgeMontages; }
	FORCEINLINE TMap<EDodgeDirection, FActionMontage> GetAllCombatTypeStepDodgeMontages() const { return CombatTypeStepDodgeMontages; }
	FORCEINLINE TMap<EDodgeDirection, FActionMontage> GetAllDefaultStepDodgeMontages() const { return DefaultStepDodgeMontages; }
	FORCEINLINE TMap<EDodgeDirection, FActionMontage> GetAllDefaultRollDodgeMontages() const { return DefaultRollDodgeMontages; }
	FORCEINLINE TMap<EDodgeDirection, FActionMontage> GetAllWeaponRollDodgeMontages() const { return WeaponRollDodgeMontages; }
	FORCEINLINE TMap<EDodgeDirection, FActionMontage> GetAllWeaponStepDodgeMontage() const { return WeaponStepDodgeMontage; }

	FORCEINLINE int32 GetGeneralCombatActionLastIndex() const { return GeneralCombatActionLastIndex; }
	FORCEINLINE int32 GetGeneralCombatActionMontageIndex() const { return GeneralCombatActionMontageIndex; }
	FORCEINLINE int32 GetStepDodgeMontageIndex() const { return StepDodgeIndex; }
	FORCEINLINE int32 GetAttackMontageLastIndex() const { return AttackMontageLastIndex; }
	FORCEINLINE int32 GetAttackMontageIndex() const { return CurrentAttackMontageIndex; }

	FORCEINLINE FAnimMontageStruct GetLastAttackMontage() const { return LastAttackMontage; }
#pragma endregion                          FORCEINLINE METHODS
};
