#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Datas/CharacterData.h"
#include "Datas/StatData.h"
#include "Datas/CombatData.h"
#include "Datas/ItemData.h"
#include "CStatsManagerComponent.generated.h"


class UCRPGItemAsset;
class ACBaseItem;
class UUW_StatBar;
class AActionHUD;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStatusEffectApplied, EStatusEffects, StatusEffectApplied);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStatusEffectRemoved, EStatusEffects, StatusEffect);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnCurrentValueUpdated, EStats, StatUpdated, float, CurrentValue, float, MaxValue);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnModifierAdded, EStats, StatModified);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnModifierRemoved, EStats, StatModified);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMaxStatValueUpdated, EStats, StatUpdated, float, Value);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSoulsGained, int32, SoulsRecentlyGained);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSoulsAmountUpdated, int32, Souls, bool, PlayUpdateAnimation);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSoulsRequiredForNextLevelUpdated, int32, InSoulsRequired);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FDamageReceived, EDamageType, InDamageTypeRecieved, float, InDamageRecieved);

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ARPG_API UCStatsManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCStatsManagerComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


#pragma region                Initialize Methods
public:
	/**모든 스텟을 초기화합니다*/
	virtual void InitializeStats();

	/**스텟 컴포넌트를 초기화합니다 */
	UFUNCTION(BlueprintCallable)
	void InitializeStatsComponent();

	/**처음 초기화 될 때 호출 되야 함*/
	void OnStatInitialized(EStats InStat, float InValue);

	/**속성 값을 초기화한다*/
	UFUNCTION(BlueprintCallable)
	void InitializeAttributes();
#pragma endregion             Initialize Methods


#pragma region                CharacterMovement Methods
public:
	/* NewMovementSpeedMode가 기존 스피드 모드와 다를 경우에만 스피드 모드를 설정한다 */
	void SetGait(EGait NewGait);

protected:
	/**스피드모드를 설정합니다_종료*/
	void OnMovementSpeedModeBegin(EGait InMovementSpeedMode);

	/**스피드모드를 설정합니다_종료*/
	void OnMovementSpeedModeEnd(EGait InMovementSpeedMode);

	/** MovementSpeed 의 배열 값에 새로운 스피드 값을 설정	그 후 캐릭터의 스피드를 업데이트 합니다	 */
	void UpdateMovementSpeed(EGait InGait, float InMovementSpeed);

	/** 오너캐릭터의 워크스피드를 변경합니다 */
	void UpdateTickMove(float InDeltaTime);

#pragma endregion                CharacterMovement Methods


#pragma region                LevelSystem Methods
public:
	/**영혼을 획득한다*/
	void GainSouls(int32 InSoulsToAdd);

	/**최근 받은 영혼의 갯수를 0으로 초기화 하고 타이머를 클리어합니다 */
	UFUNCTION()
	void ResetRecentlyReceivedSouls();

	/**영혼 갯수 수정합니다*/
	int32 ModifySoulsAmount(int32 InSouls);

	/**영혼의 갯수를 업데이트 한다 */
	void UpdateSoulsAmount(int32 SoulAmount, bool PlayUpdateAnimation);

	/**다음 레벨에 필요한 영혼 설정*/
	void SetRequiredSoulsForNextLevel(int32 InSouls);

	/**캐릭터의 레벨을 증가시킨다*/
	void IncrementCharacterLevel();

#pragma endregion                              LevelSystem Methods


#pragma region                Attributes Methods
public:
	/**현재 레벨 속성 값 가져옵니다*/
	int32 GetCurrentAttributeLevel(EAttributes InAttribute);

	/**속성 레벨 업 인덱스를 가져오기 찾지못했다면 -1을 반환합니다*/
	int32 GetAttributeLevelUpIndex(EAttributes InAttribute);

	/**지정된 속성 레벨을 +1 증가시킵니다*/
	void IncrementAttributeLevel(EAttributes InAttribute);

	/**속성을 레벨업 합니다*/
	void LevelUpAttribute(EAttributes InAttributeToLevel);
#pragma endregion             Attributes Methods


#pragma region                Stats Methods
public:
	/**로컬 스텟의 현재 값을 가져온다*/
	UFUNCTION(BlueprintPure,Category="Stats")
	float GetCurrentValueForStat(EStats InStat);

	/**로컬스텟의 최대값 가져옵니다*/
	UFUNCTION(BlueprintPure,Category="Stats")
	float GetMaxValueForStat(EStats InStat);

	/**Stats의 기본값을 가져옵니다*/
	UFUNCTION(BlueprintPure,Category="Stats")
	float GetBaseValueForStat(EStats InStat);

	/**LocalStatMultiplier 의 수정값을 가져옵니다 */
	float GetMultiplierValueForStat(EStats InStat);

	/**로컬현재 값을 값을 Mix과 Max의 값으로 클램프해서 반환한다*/
	float GetCurrentStatValue_Clamped(EStats InStat, float Min, float Max);

	/** InternalStats 의 ValueType에 따라 값을 가져온다 InternalStats 의 InStat 값이 없다면 0.0f를 반환한다	 */
	UFUNCTION(BlueprintPure,Category="Stats")
	float GetValueForStat(EStats InStat, EStatValueType ValueType);

	/**스탯유형의 현재값이 맥스값의 전체와 비교하여 비율을 가져옵니다 */
	float GetValuePercentage(EStats InStat);

	/**입력받은  스텟유형의 현재 값을 수정한다 * 스텟재생의 여부에 따라 리젠타이머를 설정합니다*/
	UFUNCTION(BlueprintCallable, Category= "Stats")
	float ModifyCurrentStatValue(EStats StatToModify, float InValue, bool StopRegen = true, bool IncludeModifiers = true);

	/**StatToModify의 최대 스텟 값을 수정한다*/
	UFUNCTION(BlueprintCallable, Category= "Stats")
	float ModifyMaxStatValue(EStats StatToModify, float InValue);

	/**수정할 스텟 유형의 현재 값을 설정합니다 */
	void SetCurrentStatValue(EStats StatToModify, float InValue);

	/**수정할 스텟 유형의 기본 값을 설정합니다 */
	void SetBaseStatValue(EStats StatToModify, float InValue);

	/**Stats의 맵의 MaxValue 값을 설정		 로컬스텟맵의 최대 값을 설정 값을*/
	void SetMaxStatValue(EStats StatToModify, float InValue);

	/**최대값과 기본값을 설정합니다 */
	void SetMaxAndBaseStatValue(EStats StatToModify, float InValue);

	/**최대 값을 업데이트하고 현재값을 설정합니다*/
	void UpdateStat(EStats StatToUpdate);

	/**	Stats 의 맵에 InStat과 InValue 값을 Add한다*/
	void UpdateBaseStats(EStats InStat, FUserEditableStats InValue);


protected:
	/**내부 스텟 현재 값을 업데이트한다		 로컬 스텟 현재 값을 업데이트한다*/
	void UpdateCurrentStatValue(EStats StatToUpdate, float InValue);

	/**내부스텟의 기본값을 값을 업데이트 합니다*/
	void UpdateBaseStatValue(EStats StatToUpdate, float InValue);

	/**내부스텟 최대 값과 로컬스텟 최대 값을 업데이트 합니다*/
	void UpdateMaxStatValue(EStats StatToUpdate, float InValue);

	/**	내부 스텟 맵의 기본값과 최대값을 업데이트 합니다*/
	void UpdateStatsMap(EStats InStat, float InBaseValue, float InMaxValue);

	/**해당스텟의 기본 값이 업데이트된 경우 호출됩니다	 */
	void OnBaseStatValueUpdated(EStats InStat, float InValue);

	/**해당스텟의 현재 값이 업데이트된 경우 호출됩니다*/
	void OnCurrentStatValueUpdated(EStats StatModified, float InValue);

	/**해당스텟의 최대값이 업데이트 된 경우 호출됩니다*/
	void OnMaxValueSet(EStats InStat, float InValue);

	/**해당스텟의 최대 값이 업데이트된 경우 호출됩니다(위의 함수랑은 호출되는 함수가 다릅니다. 주로 아이템관련) */
	void WhenMaxStatValueUpdated(EStats InStat, float InValue);

#pragma endregion                              Stats Methods


#pragma region                Stats Regen Event
public:
	/**체력 재생*/
	UFUNCTION()
	void HealthRegen();

	/**마나 재생*/
	UFUNCTION()
	void ManaRegen();

	/**스테미나 재생*/
	UFUNCTION()
	void StaminaRegen();

	/**평정을 재생*/
	UFUNCTION()
	void PoiseRegen();

	/**균형 재생*/
	UFUNCTION()
	void BalanceRegen();

	/**화염 저항 재생*/
	UFUNCTION()
	void BurningResistRegen();

	/**독 저항 재생*/
	UFUNCTION()
	void PoisonResistRegen();

	/////////////////////////////////////////////////////////////////

	UFUNCTION()
	void BurningStatTimer();

	UFUNCTION()
	void HealingMedicineTimer();

	UFUNCTION()
	void PoisonStatTimer();

	UFUNCTION()
	void SprintStaminaCost();
#pragma endregion             Stats Regen Event


#pragma region                Stat Cost Methods
public:
	/**공격 스텟에 필요한 비용을 반환합니다 */
	float AttackStatCost(EAttackTypes InAttackType);

	/**캐릭터 액션에 소모되는 코스트 반환*/
	float GetCombatActionStatCost(ECharacterAction CombatAction);

	/**AttackType에 맞는 배율 값을	 * 1.0~100.0f의 값으로 제한해서 반환한다   키 값이 없다면 1.0f를 반환*/
	float GetAttackTypeMultiplier(EAttackTypes InAttackType);

#pragma endregion                              Stat Cost Methods


#pragma region                Stat Modifiers Methods
public:
	/**스텟에 수정 될 값을 추가한다
	 * InStatModifier : 증가시킬 스텟 수정 값
	 * IsReplace : 적용 된 스텟 수정 값들중에서 바꿔야 한다면 	 */
	void AddStatModifier(FStatModifier InStatModifier, bool IsReplace);

	/**InStatModifier 를 제거하고 증가된 값과 배율값을 제거하고 남은 값으로 변경한다 */
	void RemoveStatModifier(FStatModifier InStatModifier);

	/** AddStatModifier 배열버전	 * InModifiers 수정 될 값이 배열로 들어온다*/
	void AddMultipleModifiers(const TArray<FStatModifier>& InModifiers);

	/** RemoveStatModifier 배열버전	 * InModifiers 수정 될 값이 배열로 들어온다*/
	void RemoveMultipleModifiers(const TArray<FStatModifier>& InModifiers);

	/** 수정 된 값을 계산해줍니다. 내부 스텟의 (기본값+ 증가값) + (기본값 * 변한 값) 을 계산해서 반환합니다 */
	float CalculateModifiedValue(EStats InStat);

protected:
	/**내부스텟의 증가 값을 업데이트한다*/
	void UpdateStatIncrementValue(EStats StatToUpdate, float InValue);

	/*내부스텟 배율 값과 로컬스텟 배율 값을 업데이트 한다. */
	void UpdateMultiplierValue(EStats StatToUpdate, float InValue);
#pragma endregion             Stat Modifiers Methods


#pragma region                Stats Regen Methods
public:
	/**	지정한 스텟유형에 따라 스텟을 재생합니다 */
	UFUNCTION()
	void StatRegen(EStats StatToRegen);

	/**지정한 스텟유형의 현재 재생 값을 가져온다*/
	float GetCurrentStatRegenRate(EStats InStat);

	/**지정한 스텟유형의 재생량을 스텟유형의 타입에 따라 가져옵니다*/
	float GetStatRegenRate(EStats InStat, EStatValueTypeSimplified StatValueType);

	/**지정한 스텟유형의 재생 쿨다운 값을 스텟유형의 타입에 따라 가져옵니다*/
	float GetStatRegenTimerCoolDown(EStats InStat, EStatValueTypeSimplified StatValueType);

	/**지정한 스텟유형의 재생 틱간격을 스텟유형의 타입에 따라 가져옵니다*/
	float GetStatRegenTickInterval(EStats InStat, EStatValueTypeSimplified StatValueType);

	/**지정한 스텟에 대한 재생성 백분율 수정한다*/
	void ModifyRegenPercentForStat(EStats InStat, int32 InPercentage);

	/**	지정한 스텟 유형의 현재재생 쿨다운을 업데이트 합니다	*/
	void UpdateCurrentStatRegenCoolDown(EStats InStat, float InValue);

	/**	지정한 스텟 유형의 기본재생 쿨다운을 업데이트 합니다	*/
	void UpdateBaseStatRegenCoolDown(EStats InStat, float InValue);

	/**	지정한 스텟 유형의 현재재생량을 업데이트 합니다	*/
	void UpdateCurrentStatRegenRate(EStats InStat, float InValue);

	/**	지정한 스텟 유형의 기본재생량 업데이트 합니다*/
	void UpdateBaseStatRegenRate(EStats InStat, float InValue);

	/**	지정한 스텟 유형의 현재재생 틱간격을 업데이트 합니다*/
	void UpdateCurrentStatRegenTickInterval(EStats InStat, float InValue);

	/**	지정한 스텟 유형의 기본재생 틱간격을 업데이트 합니다*/
	void UpdateBaseStatRegenTickInterval(EStats InStat, float InValue);

	/**지정한 유형의 재생중인 함수를 일시정지합니다*/
	void PauseStatRegenTimer(EStats StatToPause);

	/**Stat Regen Functions Timer를 지운다*/
	void ClearRegenTimers(EStats StatToClearRegen);

public:
	/**입력받은 스텟유형의 스텟재생을 시작합니다*/
	UFUNCTION()
	void StartStatRegen(EStats StatToRegen);

	/**스탯재생을 시작한 후(딜레이 후)의 로직입니다*/
	UFUNCTION()
	void StartStatRegen_Logic(EStats StatToRegen);
#pragma endregion             Stats Regen Methods


#pragma region                Encumberence Methods
public:
	/**무게를 계산해준다*/
	EEncumbrance CalculateEncumbrance(float InValue);

	/**무게 타입의 계산해준 값을 업데이트 한다*/
	void UpdateEncumbrance();
#pragma endregion             EncumberenceMethods


#pragma region                Status Effects Methods
public:
	/**상태 효과를 적용한다*/
	void ApplyStatusEffect(const FStatusEffect& InStatusEffect, float InEffectDuration);

	/**해당 상태효과가 적용되어있는가?*/
	bool IsStatusEffectApplied(FStatusEffect InStatusEffect);

	/**상태효과의 Rate(양)를 가져옵니다*/
	float GetStatusEffectModificationRate(EStatusEffects InStatusEffect);

	/**현재 상태효과의 파티클을 가져옵니다*/
	UParticleSystem* GetStatusEffectParticle(EStatusEffects InStatusEffectType);

	/**현재 상태효과의 파티클 컴포넌트를 가져온다*/
	UParticleSystemComponent* GetAppliedStatusParticleComponent(EStatusEffects InStatusType);

	/**상태효과 타이머를 클리어합니다 */
	UFUNCTION(BlueprintCallable, Category="Status Effects")
	void ClearStatusEffectTimer(EStatusEffects StatusEffectTimerToClear);

	/** 지정한 상태 효과를 제거합니다*/
	void RemoveStatusEffect(const FStatusEffect& InStatusEffect);

	/**적용된 상태 효과 업데이트 적용된 상태효과에 고유추가하고 Rate와 TickInterval의 값을 설정해줍니다 */
	void UpdateAppliedStatusEffects(const FStatusEffect& InStatusEffect);

	/**적용 된 상태효과 타입의 파티클 컴포넌트를 업데이트합니다 */
	void UpdateAppliedStatusParticleComponent(EStatusEffects InStatusType, UParticleSystemComponent* InParticleComponent);

	/**적용된 상태효과타입의 적용된 상태효과 파티클 컴포넌트에서 값을 제거하고 제거에 성공하면 true를 반환합니다*/
	bool RemoveAppliedStatusParticleComponent(EStatusEffects InStatusType);

	/*상태 효과 적용 이벤트 델리게이트 호출*/
	void Event_OnStatusEffectApplied(EStatusEffects Selection);
public:
	/**상태효과 함수를 시작합니다 **/
	UFUNCTION()
	void StartStatusEffectTimer(EStatusEffects InStatusEffect);

	/**상태효과 타입의 지속시간이 지나면 (리트리거딜레이) 상태효과의 함수를 클리어한다*/
	UFUNCTION()
	void StopStatusEffect(EStatusEffects InStatusEffect);
#pragma endregion             Status Effects Methods


#pragma region                Consumables Methods
public:
	/**지정된 유형의 최대 맥스값의 비율(퍼센트)을 계산한 값을 가져옵니다*/
	float StatPercentageIncreaseAmount(EStats InStatToIncrease, int32 InPercentageAmount);

	/**아이템을 소비(사용)합니다*/
	void ConsumeItem(EStats InStatToModify, FStatusEffect InStatusEffect, int32 InModificationPercentage);
#pragma endregion             Consumables Methods


#pragma region                Item Modifer Methods
public:
	/**아이템 수정 값을  스텟적용 값에 추가한다*/
	void AddItemModifiers(UCRPGItemAsset* InItem);

	/**아이템 수정 값을 제거한다*/
	void RemoveItemModifiers(UCRPGItemAsset* InItem);
	//
	/**비활성화 된 슬롯을 업데이트한다*/
	UFUNCTION()
	void OnSlotDisabledUpdated(EEquipSlots InEquipSlots, FItemSlot InSlotItem, bool IsDisabled);

#pragma endregion             Item Modifer Methods
	//
	//
	// #pragma region                Damage Methods
	// public:
	// 	/**물리 피해를 받는다*/
	// 	void TakePhysicalDamage(float InDamage, EImpactDecision InImpactDecision, EStats InDamageResistType = EStats::None,
	// 	                        EDamageType InDamageTypeReceived = EDamageType::None);
	//
	// 	/**데미지 저항*/
	// 	float DamageResist(EStats ResistStat, float InDamage);
	//
	// 	/**최근에 받은 데미지 *최근에 받은 히트횟수를 0으로 설정한다*/
	// 	UFUNCTION()
	// 	void ResetRecentlyReceivedDamage();
	//
	// 	/**평정 데미지*/
	// 	void DamagePoise(float InPoiseDamage);
	//
	// 	/**균형 저항값을 계산하여 데미지를 받는다
	// 	 * 현재 균형 값이 0.0f보다 작거나 같다면 true를 반환한다	 */
	// 	bool DamageBalance(float InBalanceDamage);
	//
	// 	/**물리공격 데미지를 가져온다*/
	// 	float GetPhysicalAttackDamage(EAttackTypes InAttackType, EStats DamageStat, bool PunisherMode, float StrongChargeAmount);
	//
	// 	/**포이즈 공격데미지를 가져온다*/
	// 	float GetPoiseDamage(EAttackTypes InAttackType, bool PunisherMode, float StrongChargeAmount);
	//
	// #pragma endregion             Damage Methods


public:
	/**오너캐릭터가 죽음설정이 된다면 호출됩니다.*/
	void OnDeath();

	/**모닥불에서 휴식할 시 호출됩니다 */
	void OnBonfireRested();

	/*****************************************************************************************************Field Members******************************/
	/*****************************************************************************************************Field Members******************************/
	/*****************************************************************************************************Field Members******************************/
#pragma region                                           Default Members
private:
	/**컴포넌트를 가지고 있는 캐릭터객체*/
	UPROPERTY(BlueprintReadOnly, Category = "StatsManager | Default", meta = (AllowPrivateAccess = "true"))
	ACharacter* CharacterRef;
	
	/** 가장 최근에 받은 데미지*/
	float RecentlyReceivedDamage;

	/**가장 최근에 받은 피해 횟수*/
	int32 RecentlyReceivedHitCount;

	/**방어 실패 여부*/
	bool bDefenseBroken;

	/**무게의 초과 여부*/
	bool bOverEncumbered;

	/**현재 무게유형*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "StatsManager | Default", meta = (AllowPrivateAccess = "true"))
	EEncumbrance CurrentEncumbrance;

	/**캐릭터 레벨*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "StatsManager | Default", meta = (AllowPrivateAccess = "true"))
	int32 CharacterLevel;
	
	/**시작 시 소울의 값*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "StatsManager | Initialize", meta = (AllowPrivateAccess = "true"))
	int32 StartingSoulsAmount;

	/**레벨 업 시 스텟 값*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "StatsManager | Initialize", meta = (AllowPrivateAccess = "true"))
	TArray<FAssociatedStats> LevelingUpStatsValues;
#pragma endregion                                        Default Members


#pragma region                                           Character Movement Members
private:
	/**이동속도 수정이 가능한가?*/
	bool bCanModifyMovementSpeed;

	/**캐릭터이동속도를 업데이트해야하는가?*/
	bool bShouldUpdateSpeed;

	/**이동속도가 변경될 때의 인텁스피드*/
	float MovementSpeedInterpSpeed;

	/**원하는 이동속도*/
	float DesiredSpeed;
	
	/**처음 시작될 보행타입*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="StatsManager | CharacterMovement", meta=(AllowPrivateAccess="true"))
	EGait StartingGait;

	/**현재 보행유형*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="StatsManager | CharacterMovement", meta=(AllowPrivateAccess="true"))
	EGait Gait;

	/**보행 타입에 따른 스피드*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="StatsManager | CharacterMovement", meta=(AllowPrivateAccess="true"))
	TMap<EGait, float> GaitSpeed;

#pragma endregion                                        Character Movement Members


#pragma region                                           Stats Members
protected:
	/**내부 스텟*/
	TMap<EStats, FFloatAttribute> InternalStats;

	/**로컬 현재 스텟*/
	TMap<EStats, float> LocalStatCurrentValues;

	/**로컬 최대 스텟*/
	TMap<EStats, float> LocalStatMaxValues;

	/**로컬 스텟 배율 값*/
	TMap<EStats, float> LocalStatMultiplierValues;

	/**초기화 스텟*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "StatsManager | Stats", meta = (AllowPrivateAccess = "true"))
	TMap<EStats, FUserEditableStats> Stats;

#pragma endregion                                        Stats Members


#pragma region                                           Attributes Members
private:
	/**스텟 속성 값들*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "StatsManager | Attributes", meta = (AllowPrivateAccess = "true"))
	TMap<EAttributes, int32> Attributes;
#pragma endregion                                        Attributes Members


#pragma region                                           Stats Modifiers Members
private:
	/**적용 된 스텟 수정 값*/
	TArray<FStatModifier> AppliedStatModifiers;

	/**적용된 아이템 스탯 수정값*/
	TArray<FItemStatModifier> AppliedItemStatModifiers;
#pragma endregion                                        Stats Modifiers Members


#pragma region                                           Stat Cost Members
private:
	/**방어를 하기 위한 최소 스테미너 기준 값*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "StatsManager | Stat Cost", meta = (AllowPrivateAccess = "true"))
	float MinimumStaminaDefenseBreak;

	/**강력한 차지 공격 배율*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "StatsManager | Stat Cost", meta = (AllowPrivateAccess = "true"))
	float StrongChargeAttackMultiplier;

	/**일반 데미지 배율*/
	float PhysicalDamageMultiplier;

	/**평정 데미지 배율*/
	float PoiseDamageMultiplier;

	/**공격 타입의 스텟 배율 (액터 초기화 시 블루프린트에서 입맛에 맞게 설정하세요)*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "StatsManager | Stat Cost", meta = (AllowPrivateAccess = "true"))
	TMap<EAttackTypes, float> AttackTypeStatMultipliers;

	/**캐릭터액션의 스텟 소모 값(액터 초기화 시 블루프린트에서 입맛에 맞게 설정하세요)*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "StatsManager | Stat Cost", meta = (AllowPrivateAccess = "true"))
	TMap<ECharacterAction, float> CombatActionStatCost;

#pragma endregion                                        Stat Cost Members


#pragma region                                           Stats Regen Members
protected:
	/**스텟의 타입마다 지연될 쿨타임*/
	TMap<EStats, float> CurrentStatRegenCoolDown;

	/**스텟의 타입마다 더해질 양*/
	TMap<EStats, float> CurrentStatRegenRate;

	/**스텟의 타입마다 루프시 일어날 틱 간격*/
	TMap<EStats, float> CurrentStatRegenTickInterval;

	/**UI용을 위한 스텟의 타입마다 지연될 쿨타임*/
	TMap<EStats, FFloatStatValue> StatRegenCoolDown;

	/**UI용을 위한 스텟의 타입마다 더해질 양*/
	TMap<EStats, FFloatStatValue> StatRegenRate;

	/**UI용을 위한 스텟의 타입마다 루프시 일어날 틱 간격*/
	TMap<EStats, FFloatStatValue> StatRegenTickInterval;

	/**스텟의 타입마다 리젠될 타이머의 핸들*/
	TMap<EStats, FTimerHandle> StatRegenTimerHandels;

	
	TArray<EStats> StatRegenExist;
	
	/**이니셜 라이즈 때 사용 될 스텟의 타이머가 계산될 구조체의 변수*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "StatsManager | Stats Regen", meta = (AllowPrivateAccess = "true"))
	FStatTimer StatTimerStruct;

	/**스텟의 타입마다 리젠될 타이머의 함수이름*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "StatsManager | Stats Regen", meta = (AllowPrivateAccess = "true"))
	TMap<EStats, FString> StatRegenFunctions;

#pragma endregion                                        Stats Regen Members


#pragma region                                           Level System Members
private:
	// UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "StatsManager | Level", meta = (AllowPrivateAccess = "true"))
	/**현재 소유한 영혼*/
	int32 Souls;

	/**가장 최근에 받은 영혼 */
	int32 RecentlyReceivedSouls;

	/**다음 레벨업에 필요한 소울*/
	int32 RequiredSoulsForNextLevel;
#pragma endregion                                        Level System Members


#pragma region                                           Status Effects Members
private:
	/**적용 된 상태효과(들)*/
	TArray<FStatusEffect> AppliedStatusEffects;
	
	TMap<EStatusEffects, FString> StatusEffectRegenFunctions;

	/**상태효과 종류에 따른 수정될 값*/
	TMap<EStatusEffects, float> StatusEffectModificationRate;

	/**상태효과 종류에 따른 지속시간들*/
	TMap<EStatusEffects, float> StatusEffectDuration;

	/**상태효과에 따른 틱 인터벌*/
	TMap<EStatusEffects, float> StatusEffectTickInterval;

	/**상태효과에 따른 타이머핸들*/
	TMap<EStatusEffects, FTimerHandle> StatusEffectStopTimerHandles;

	/**상태효과 종류에 따른 파티클들*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "StatsManager | Status Effects", meta = (AllowPrivateAccess = "true"))
	TMap<EStatusEffects, UParticleSystem*> StatusEffectParticles;

	/**상태효과 종류에 따른 파티클시스템들*/
	UPROPERTY(BlueprintReadOnly, Category = "StatsManager | Status Effects", meta = (AllowPrivateAccess = "true"))
	TMap<EStatusEffects, UParticleSystemComponent*> AppliedStatusParticleComponents;

#pragma endregion                                        Status Effects Members


#pragma region                                           DECLARE_DYNAMIC_MULTICAST_DELEGATE
public:
	/**델리게이트_ 상태 효과 적용 시*/
	UPROPERTY(BlueprintAssignable)
	FOnStatusEffectApplied OnStatusEffectApplied;

	/**델리게이트_ 상태 효과 제거 시*/
	UPROPERTY(BlueprintAssignable)
	FOnStatusEffectRemoved OnStatusEffectRemoved;

	/**델리게이트_ 현재 값이 업데이트 될 때*/
	UPROPERTY(BlueprintAssignable)
	FOnCurrentValueUpdated OnCurrentValueUpdated;

	/**델리게이트_ 스텟 값이 더해질 때*/
	UPROPERTY(BlueprintAssignable)
	FOnModifierAdded OnModifierAdded;

	/**델리게이트_ 스텟 값이 제거될 때*/
	UPROPERTY(BlueprintAssignable)
	FOnModifierRemoved OnModifierRemoved;

	/**델리게이트_ 최대 값이 업데이트 될 때*/
	UPROPERTY(BlueprintAssignable)
	FOnMaxStatValueUpdated OnMaxStatValueUpdated;

	/**델리게이트_ 영혼을 얻을 때*/
	UPROPERTY(BlueprintAssignable)
	FOnSoulsGained OnSoulsGained;

	/**델리게이트_ 영혼 수가 업데이트 될 때*/
	UPROPERTY(BlueprintAssignable)
	FOnSoulsAmountUpdated OnSoulsAmountUpdated;

	/**델리게이트_ 다음 레벨에 필요한 영혼이 업데이트 될 때*/
	UPROPERTY(BlueprintAssignable)
	FSoulsRequiredForNextLevelUpdated OnSoulsRequiredForNextLevelUpdated;

	/**델리게이트_ 데미지를 받았을 때*/
	UPROPERTY(BlueprintAssignable)
	FDamageReceived OnDamageReceived;

#pragma endregion                                        DECLARE_DYNAMIC_MULTICAST_DELEGATE


#pragma region                                           FORCEINLINE
public:
	/**현재 이동속도를 수정할 수 있는 상태 여부를 가져온다*/
	FORCEINLINE bool GetCanModifyMovementSpeed() const { return bCanModifyMovementSpeed; }
	/**현재 방어가 깨진 여부를 가져온다*/
	FORCEINLINE bool IsDefenseBroken() const { return bDefenseBroken; }

	/**현재 스피드모드를 가져온다*/
	FORCEINLINE EGait GetGait() const { return Gait; }
	/**현재 무게 타입을 가져온다*/
	FORCEINLINE EEncumbrance GetCurrentEncumbrance() const { return CurrentEncumbrance; }

	/**현재 소유 한 소울을 가져온다*/
	FORCEINLINE int32 GetCurrentHeldSouls() const { return Souls; }
	/**최근에 받은 영혼을 가져온다*/
	FORCEINLINE int32 GetRecentlyReceivedSouls() const { return RecentlyReceivedSouls; }
	/**다음 레벨업에 필요한 영혼 값 가져오기*/
	FORCEINLINE int32 GetRequiredSoulsForNextLevel() const { return RequiredSoulsForNextLevel; }
	/**시작 소울 영혼 값*/
	FORCEINLINE int32 GetStartingSoulsAmount() const { return StartingSoulsAmount; }

	/**일반 데미지 배율을 가져온다*/
	FORCEINLINE float GetPhysicalDamageMultiplier() const { return PhysicalDamageMultiplier; }
	/**평정무게 데미지 배율을 가져온다*/
	FORCEINLINE float GetPoiseDamageMultiplier() const { return PoiseDamageMultiplier; }
	/**최근 받은 데미지를 가져온다*/
	FORCEINLINE float GetRecentlyReceivedDamage() const { return RecentlyReceivedDamage; }
	/**최근 받은 히트카운트를 가져온다*/
	FORCEINLINE float GetRecentlyReceivedHitCount() const { return RecentlyReceivedHitCount; }


	/**현재 적용된 상태효과들을 가지고 온다*/
	FORCEINLINE TArray<FStatusEffect> GetAppliedStatusEffects() const { return AppliedStatusEffects; }
	/** 적용 된 스텟의 수정 값들을 가져온다 */
	FORCEINLINE TArray<FStatModifier> GetAppliedStatModifiers() { return AppliedStatModifiers; }

	/**일반 데미지 비율을 설정한다*/
	FORCEINLINE void SetPhysicalDamageMultiplier(float InPhysicalDamageMultiplier) { PhysicalDamageMultiplier = InPhysicalDamageMultiplier; }
	/**일반 평정 비율을 설정한다*/
	FORCEINLINE void SetPoiseDamageMultiplier(float InPoiseDamageMultiplier) { PoiseDamageMultiplier = InPoiseDamageMultiplier; }
	/**이동속도를 수정할 수 있는 상태를 설정한다 */
	FORCEINLINE void SetCanModifyMovementSpeed(bool CanResetMovementSpeed) { bCanModifyMovementSpeed = CanResetMovementSpeed; }

#pragma endregion                                        FORCEINLINE
};
