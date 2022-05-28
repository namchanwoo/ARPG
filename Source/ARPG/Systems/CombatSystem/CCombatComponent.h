#pragma once

#include "CoreMinimal.h"
#include "AttackInfo/CAttackInfo.h"
#include "Components/ActorComponent.h"
#include "Datas/CombatData.h"
#include "Datas/ItemData.h"
#include "CCombatComponent.generated.h"


class UCharacterMovementComponent;
class UCCharacterStateManagerComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInputBufferConsumed, EBufferKey, InputBufferKey);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPunisherModeChanged, bool, PunisherMode);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInputBufferOpen);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInputBufferClose);


UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ARPG_API UCCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCCombatComponent();

protected:
	virtual void BeginPlay() override;

#pragma region                           Input Buffer Methods
public:
	/**버퍼 키를 저장합니다. 입력버퍼가 열려있으면 입력을 저장하고 열려있지않다면 작업을 수행합니다*/
	void StoreBufferKey(EBufferKey InStoredBufferKey);

	/**입력 버퍼를 오픈합니다*/
	void OpenInputBuffer();

	/**입력 버퍼를 닫습니다*/
	void CloseInputBuffer();

	/**버퍼들을 클리어합니다*/
	void ClearInputBuffer();

	/**저장된 입력이 참이면 버퍼링된 작업을 수행하고 입력 버퍼를 닫습니다. 입력이 저장되지 않으면 입력 버퍼를 닫습니다.*/
	void OnlyBufferCertainKeys(TMap<EBufferKey, bool> InBufferKey, bool IsResetState);

	/**공격 버퍼 키를 저장합니다*/
	void StoreAttackBufferKey(EAttackTypes InAttackBufferKey);

	/**저장된 키들을 초기화합니다*/
	void ClearStoredBufferKeys();

	/**퀵 슬롯 버퍼 키 저장*/
	void StoreQuickSlotBufferKey(EQuickSlots InQuickSlotBufferKey);

	/**
	 * @brief 보유한 버퍼키를 저장합니다.
	 * @param HeldBufferKey 검사할 버퍼 키
	 * @param InValue 검사 할 값
	 * @param IsConsume 버퍼키의 사용여부
	 */
	void StoreHeldBufferKey(EBufferKey HeldBufferKey, bool InValue, bool IsConsume);

	/**보유한 키가 존재하고 키 값이 참이라면 true를 반환합니다*/
	bool GetHeldBufferKeyValue(EBufferKey Key);

	/**입력한 키가 얼마나 눌려있는 지 시간을 반환합니다*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Input Buffer")
	float GetInputHeldTime(FKey Key);

#pragma endregion                        Input Buffer Methods


#pragma region                           Combat Methods
public:
	/**캐릭터 어택 타입을 Set한다*/
	void SetCharacterAttackType(EAttackTypes InAttackType);

	/**전투가 활성화 상태라면 Idle로 바꾼다*/
	void ResetCombatState();

	/**피니셔모드를 설정한다*/
	void SetFinisherMode(bool InFinisherMode);

	/**선택적 능력을 수행할 수 있는 어빌리티가 존재하는가?*/
	bool CanPerformOptionalAbility(EOptionalCombatAbilities InOptionalAbility);

#pragma endregion                        Combat Methods


#pragma region                           Dodge Methods
public:
	/**닷지타입을 설정한다t*/
	void SetDodgeType(EDodgeType NewDodgeType);

	/**회피의 방향을 계산합니다*/
	EDodgeDirection CalculateDirectionalDodge();

	/**Dodge의 방향을 결정한다*/
	EDodgeDirection SetDodgeDirection(bool OrientRotationToMovement);
#pragma endregion                        Dodge Methods

	
	/**마주보는 방향을 계산합니다*/
	EGetupType CalculateFacingDirection();

	/**공격 정보 저장*/
	void StoreAttackInfo(EMeleeSwingDirection InMeleeSwingDirection, EAttackPower InAttackPower,
	                     ECustomAttackTypes InCustomAttackTypes, bool InCauseKnockBack, float InKnockBackTime,
	                     float InKnockBackSpeed, bool InIsUnBlockAbleAttack, EAttackCategory InAttackCategory,
	                     ECombatType InCombatType, int32 InAttackIndex);

	/**애니메이션몽테이지를 업데이트합니다*/
	void OnMontagesUpdated(FDataTableRowHandle InDataTableInfo);

	/**기본적인 공격정보를 저장합니다*/
	void StoreBasicAttackInfo(EGeneralCombatType AttackCategory, ECombatType CombatType, int32 AttackIndex);


#pragma region                                Help Methods
private:
	/**오너캐릭터의 메쉬를 가져옵니다*/
	USkeletalMeshComponent* GetMesh();

	/**오너캐릭터가 추락중인가?*/
	bool IsFalling();

	/**오너캐릭터의 애님인스턴스를 가져옵니다*/
	UAnimInstance* GetAnimInstance();

	/**캐릭터의 무브먼트컴포넌트를 가져옵니다*/
	UCharacterMovementComponent* GetCharacterMovement();

	/**현재 애니메이션 몽테이지가 재생상태인가?*/
	bool MontagePlaying(UAnimMontage* InMontage);
#pragma endregion                             Help Methods

	

/******************************************************************************************************************Field Members********************/
/******************************************************************************************************************Field Members********************/
/******************************************************************************************************************Field Members********************/

#pragma region               Default Members
private:
	/**현재 회피 타입*/
	EDodgeType CurrentDodgeType;

	/**오너 캐릭터*/
	UPROPERTY(BlueprintReadOnly, Category = "CombatComponent | Default", meta = (AllowPrivateAccess = "true"))
	ACharacter* OwnerCharacterRef;

	/**플레이어 컨트롤러*/
	UPROPERTY(BlueprintReadOnly, Category = "CombatComponent | Default", meta = (AllowPrivateAccess = "true"))
	APlayerController* GenericPlayerController;

	/**선택 적 전투능력 타입*/
	UPROPERTY(BlueprintReadOnly, Category = "CombatComponent | CombatSystem", meta = (AllowPrivateAccess = "true"))
	TMap<EOptionalCombatAbilities, bool> OptionalCombatAbilities;

	/**충돌추적 대상에 따른 이름*/
	UPROPERTY(BlueprintReadOnly, Category = "CombatComponent | CombatSystem", meta = (AllowPrivateAccess = "true"))
	TMap<ECollisionTraceTarget, FName> TraceTargetSockets;

#pragma endregion            Default Members


#pragma region               Component Members
private:
	/**캐릭터 스테이트 매니저*/
	UPROPERTY(BlueprintReadOnly, Category = "CombatComponent | Components", meta=(AllowPrivateAccess="true"))
	UCCharacterStateManagerComponent* StateManagerComponent;
#pragma endregion            Component Members


#pragma region               InputBuffer Members
private:
	/**버퍼가 열려있는 지*/
	bool bIsBufferOpen;

	/**입력 저장*/
	bool bSaveInput;

	/**입력 버퍼를 사용할 수 있는지*/
	bool bCanConsumeInputBuffer;

	/**저장 된 키*/
	EBufferKey StoredBufferKey;

	/**어택 키*/
	EAttackTypes AttackBufferKey;

	/**퀵 슬롯 키*/
	EQuickSlots QuickSlotBufferKey;

	/**마지막으로 저장된 버퍼키*/
	EBufferKey LastStoredHeldBufferKey;

	/**보유 버퍼 키*/
	TMap<EBufferKey, bool> HeldBufferKeys;
#pragma endregion            InputBuffer Members


#pragma region               Region Members
private:
	/**퍼니셔 모드인가?*/
	bool bFinisherMode;

	/**현재 공격 타입*/
	EAttackTypes CurrentAttackType;

	/**공격 정보*/
	UPROPERTY(BlueprintReadOnly, Category = "CombatComponent | Combat", meta=(AllowPrivateAccess="true"))
	UCAttackInfo* AttackInfo;
#pragma endregion            Region Members


#pragma region               Parry System Members
private:
	/**Parry에 성공했는가?*/
	bool bParrySuccessful;

	/**해당 페리 프레임인가?*/
	bool bParryFrames;

	/**최소 허용 Parry 거리*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "CombatComponent | Parry System", meta = (AllowPrivateAccess = "true"))
	float MinimumAllowedParryDistance;

#pragma endregion            Parry System Members


#pragma region               Perfect Block Members
private:
	/**완벽한 방어 타이밍*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "CombatComponent | Perfect Block",meta=(AllowPrivateAccess="true"))
	float PerfectBlockTiming;
#pragma endregion            Perfect Block Members


#pragma region               DECLARE_DYNAMIC_MULTICAST_DELEGATE
public:
	UPROPERTY(BlueprintAssignable)
	FOnInputBufferConsumed OnInputBufferConsumed;

	UPROPERTY(BlueprintAssignable)
	FOnPunisherModeChanged OnFinisherModeChanged;

	UPROPERTY(BlueprintAssignable)
	FOnInputBufferOpen OnInputBufferOpen;

	UPROPERTY(BlueprintAssignable)
	FOnInputBufferClose OnInputBufferClose;
#pragma endregion            DECLARE_DYNAMIC_MULTICAST_DELEGATE


#pragma region                                FORCEINLINE METHODS
public:
	/**저장 된 버퍼키를 가져옵니다*/
	FORCEINLINE EBufferKey GetStoredBufferKey() { return StoredBufferKey; }
	/**저장 된 퀵슬롯 버퍼키를 가져옵니다*/
	FORCEINLINE EQuickSlots GetStoredQuickSlotBufferKey() { return QuickSlotBufferKey; }
	/**저장된 공격 버퍼키를 가져옵니다*/
	FORCEINLINE EAttackTypes GetStoredAttackBufferKey() { return AttackBufferKey; }
	/**보유한 버퍼키의 키값들을 가져옵니다*/
	FORCEINLINE TMap<EBufferKey, bool> GetHeldBufferKeys() { return HeldBufferKeys; }
	/**마지막 보유한 버퍼키의 값을 가져옵니다*/
	FORCEINLINE EBufferKey GetLastStoredHeldBufferKey() { return LastStoredHeldBufferKey; }
	/**현재 공격 유형을 가져옵니다*/
	FORCEINLINE EAttackTypes GetCurrentAttackType() { return CurrentAttackType; }
	/**현재 공격정보를 가져옵니다*/
	FORCEINLINE UCAttackInfo* GetAttackInfo() { return AttackInfo; }
	/**현재 회피유형을 가져옵니다*/
	FORCEINLINE EDodgeType GetDodgeType() { return CurrentDodgeType; }
	/**피니셔 모드 상태인가?*/
	FORCEINLINE bool IsFinisherMode() { return bFinisherMode; }
	/**현재버퍼가 오픈된 상태인가?*/
	FORCEINLINE bool IsBufferOpen() { return bIsBufferOpen; }
	/**버퍼가 세이브되었는가?*/
	FORCEINLINE bool InputSaved() { return bSaveInput; }
#pragma endregion                             FORCEINLINE METHODS
	
};
