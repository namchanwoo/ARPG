#include "CStatsManagerComponent.h"

#include "CEquipmentManagerComponent.h"
#include "Global.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Interfaces/Interface_GeneralCharacter.h"
#include "Particles/ParticleSystemComponent.h"
#include "Systems/ItemSystem/CRPGItemAsset.h"


UCStatsManagerComponent::UCStatsManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	//Default Members
	{
		CurrentEncumbrance = EEncumbrance::Light;
		StartingSoulsAmount = 100;
		CharacterLevel = 10;
	}


	//Character Movement
	{
		bCanModifyMovementSpeed = true;
		MovementSpeedInterpSpeed = 7.0f;
		GaitSpeed.Add(EGait::Walk, 300.f);
		GaitSpeed.Add(EGait::Run, 600.f);
		GaitSpeed.Add(EGait::Sprint, 900.f);
		GaitSpeed.Add(EGait::Blocking, 250.f);
		StartingGait = EGait::Run;
	}


	//Stat
	{
		const FUserEditableStats BaseStatStruct = FUserEditableStats(0.0f, 100.0f);
		for (int32 i = 0; i < static_cast<int32>(EStats::MAX); i++)
		{
			const EStats L_Stats = static_cast<EStats>(i);
			if (L_Stats == EStats::None || L_Stats == EStats::MAX) continue;;
			Stats.Add(L_Stats, BaseStatStruct);
		}
	}


	//Stat Cost
	{
		StrongChargeAttackMultiplier = 1.4f;
		MinimumStaminaDefenseBreak = 10.f;
		PhysicalDamageMultiplier = 1.0f;
		PoiseDamageMultiplier = 1.0f;

		//기본 캐릭터 행동 액션 설정
		for (int32 i = 0; i < static_cast<int32>(EAttackTypes::MAX); i++)
		{
			EAttackTypes L_AttackType = static_cast<EAttackTypes>(i);
			AttackTypeStatMultipliers.Add(L_AttackType, 1.0f);
		}

		//기본 캐릭터 행동 액션 설정
		for (int32 i = 0; i < static_cast<int32>(ECharacterAction::MAX); i++)
		{
			ECharacterAction L_CharacterAction = static_cast<ECharacterAction>(i);
			CombatActionStatCost.Add(L_CharacterAction, 0.0f);
		}
		CombatActionStatCost.Add(ECharacterAction::RollDodge, 40.0f);
	}


	//Stat Regen
	{
		//스텟 리젠 함수 등록
		StatRegenFunctions.Add(EStats::Health, TEXT("HealthRegen"));
		StatRegenFunctions.Add(EStats::Mana, TEXT("ManaRegen"));
		StatRegenFunctions.Add(EStats::Stamina, TEXT("StaminaRegen"));
		StatRegenFunctions.Add(EStats::Poise, TEXT("PoiseRegen"));
		StatRegenFunctions.Add(EStats::Balance, TEXT("BalanceRegen"));
		StatRegenFunctions.Add(EStats::BurningResist, TEXT("BurningResistRegen"));
		StatRegenFunctions.Add(EStats::PoisonResist, TEXT("PoisonResistRegen"));

		StatRegenExist.AddUnique(EStats::Health);
		StatRegenExist.AddUnique(EStats::Mana);
		StatRegenExist.AddUnique(EStats::Stamina);
		StatRegenExist.AddUnique(EStats::Poise);
		StatRegenExist.AddUnique(EStats::Balance);
		StatRegenExist.AddUnique(EStats::BurningResist);
		StatRegenExist.AddUnique(EStats::PoisonResist);

		//스텟 리젠 CoolDown
		for (EStats RegenExist : StatRegenExist)
		{
			StatTimerStruct.StatRegenCoolDown.Add(RegenExist, 1.0f);
			StatTimerStruct.StatRegenRate.Add(RegenExist, 1.0f);
			StatTimerStruct.StatTimerTickInterval.Add(RegenExist, 0.1f);
		}
	}


	//Status Effects
	{
		StatusEffectRegenFunctions.Add(EStatusEffects::Burning,TEXT("BurningStatTimer"));
		StatusEffectRegenFunctions.Add(EStatusEffects::Poison,TEXT("PoisonStatTimer"));
		StatusEffectRegenFunctions.Add(EStatusEffects::Healing,TEXT("HealingMedicineTimer"));

		// StatusEffectParticles.Add 파티클을 추가해야합니다
	}


	//LeveSystem 초기화
	{
		RequiredSoulsForNextLevel = 100;
	}


	for (int i = 0; i < static_cast<int>(EAttackTypes::MAX); i++)
	{
		EAttackTypes L_AttackType = static_cast<EAttackTypes>(i);
		AttackTypeStatMultipliers.Add(L_AttackType, 1.0f);
	}

	//Attributes 기본 값 세팅
	{
		for (int32 i = 0; i < static_cast<int32>(EAttributes::MAX); i++)
		{
			EAttributes L_Attributes = static_cast<EAttributes>(i);
			if (L_Attributes == EAttributes::None) continue;
			Attributes.Add(L_Attributes, 1);
		}
	}
}

void UCStatsManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	ACharacter* CastCharacter = Cast<ACharacter>(GetOwner());
	if (CastCharacter != nullptr)
		CharacterRef = CastCharacter;
}

void UCStatsManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	UpdateTickMove(DeltaTime);
}

void UCStatsManagerComponent::InitializeStats()
{
	//스텟의 열거형의 갯수만큼 반복문을 돌린다
	for (int32 i = 1; i < static_cast<int32>(EStats::MAX); i++)
	{
		const EStats L_Stat = static_cast<EStats>(i); //스텟 타입을 설정합니다
		if (L_Stat == EStats::None) continue; //None타입이라면 컨티뉴

		if (Stats.Contains(L_Stat))
		{
			const FUserEditableStats L_StatStruct = *Stats.Find(L_Stat); //해당 스텟구조 변수값을 설정

			UpdateStatsMap(L_Stat, L_StatStruct.BaseValue, L_StatStruct.MaxValue); //Internal Stat 초기화
			UpdateMaxStatValue(L_Stat, L_StatStruct.MaxValue);
			UpdateBaseStatValue(L_Stat, L_StatStruct.BaseValue);
			UpdateCurrentStatValue(L_Stat, L_StatStruct.BaseValue);

			//스텟 재생값들을 설정합니다
			if (StatTimerStruct.StatRegenCoolDown.Contains(L_Stat))
				UpdateBaseStatRegenCoolDown(L_Stat, *StatTimerStruct.StatRegenCoolDown.Find(L_Stat));
			if (StatTimerStruct.StatRegenRate.Contains(L_Stat))
				UpdateBaseStatRegenRate(L_Stat, *StatTimerStruct.StatRegenRate.Find(L_Stat));
			if (StatTimerStruct.StatTimerTickInterval.Contains(L_Stat))
				UpdateBaseStatRegenTickInterval(L_Stat, *StatTimerStruct.StatTimerTickInterval.Find(L_Stat));

			//스텟 유형 타입에 맞게 초기화
			OnStatInitialized(L_Stat, L_StatStruct.BaseValue);
		}
	}
}

void UCStatsManagerComponent::InitializeStatsComponent()
{
	SetGait(StartingGait); //스피드 모드를 스타팅모드로 설정한다

	InitializeStats(); //스텟들을 초기화한다

	//오너캐릭터의 장비컴포넌트에 델리게이트를 연동한다
	UCEquipmentManagerComponent* EquipmentManagerComponent = Cast<UCEquipmentManagerComponent>(
		GetOwner()->GetComponentByClass(UCEquipmentManagerComponent::StaticClass()));

	CHECK(EquipmentManagerComponent != nullptr);
	EquipmentManagerComponent->OnSlotDisabledUpdated.AddDynamic(this, &UCStatsManagerComponent::OnSlotDisabledUpdated);

	UpdateSoulsAmount(StartingSoulsAmount, false); //영혼의 갯수를 업데이트 한다
	SetRequiredSoulsForNextLevel(RequiredSoulsForNextLevel); //다음 레벨업에 필요한 영혼값을 설정한다
}

void UCStatsManagerComponent::OnStatInitialized(EStats InStat, float InValue)
{
	switch (InStat)
	{
	case EStats::Health:
		StatRegen(EStats::Health);
		break;

	case EStats::Mana:
		StatRegen(EStats::Mana);
		break;

	case EStats::Stamina:
		StatRegen(EStats::Stamina);
		break;

	case EStats::Weight:
		UpdateEncumbrance();
		break;

	case EStats::MovementSpeed:
		UpdateMovementSpeed(EGait::Run, InValue);
		break;

	default: break;
	}
}

void UCStatsManagerComponent::InitializeAttributes()
{
	//레벨 업 스텟 배열의 갯수가 0개보다 크다면
	if (LevelingUpStatsValues.Num() > 0)
	{
		for (auto LevelUpStat : LevelingUpStatsValues)
		{
			TArray<EStats> L_AllStatKeys;
			LevelUpStat.StatsToModify.GetKeys(L_AllStatKeys);

			for (const auto StatKey : L_AllStatKeys)
			{
				const UCurveFloat* CurveFloat = *LevelUpStat.StatsToModify.Find(StatKey);
				if (IsValid(CurveFloat))
				{
					const float L_CurveTime = GetCurrentAttributeLevel(LevelUpStat.Attribute);
					const float L_CurveValue = CurveFloat->GetFloatValue(L_CurveTime);
					SetMaxAndBaseStatValue(StatKey, L_CurveValue);
				}
			}
		}
	}
}

void UCStatsManagerComponent::SetGait(EGait NewGait)
{
	//현재 스피드모드가 새로운 스피드모드가 아니면
	if (Gait != NewGait)
	{
		OnMovementSpeedModeEnd(Gait);
		Gait = NewGait;
		OnMovementSpeedModeEnd(Gait);
	}
}

void UCStatsManagerComponent::OnMovementSpeedModeBegin(EGait InMovementSpeedMode)
{
	CHECK(GaitSpeed.Contains(InMovementSpeedMode));
	DesiredSpeed = *GaitSpeed.Find(InMovementSpeedMode);
	bShouldUpdateSpeed = true;
}

void UCStatsManagerComponent::OnMovementSpeedModeEnd(EGait InMovementSpeedMode)
{
	CHECK(GaitSpeed.Contains(InMovementSpeedMode));
	DesiredSpeed = *GaitSpeed.Find(InMovementSpeedMode);
	bShouldUpdateSpeed = true;
}

void UCStatsManagerComponent::UpdateMovementSpeed(EGait InGait, float InMovementSpeed)
{
	GaitSpeed.Add(InGait, InMovementSpeed);
	CHECK(GaitSpeed.Contains(GetGait()));
	DesiredSpeed = *GaitSpeed.Find(GetGait());
	bShouldUpdateSpeed = true;
}

void UCStatsManagerComponent::UpdateTickMove(float InDeltaTime)
{
	//스피드를 업데이트 해야하지 않는다면 종료합니다
	if (!bShouldUpdateSpeed) return;

	//소유자 캐릭터의 걷기 스피드를 설정한다
	CharacterRef->GetCharacterMovement()->MaxWalkSpeed =
		UKismetMathLibrary::FInterpTo(CharacterRef->GetCharacterMovement()->MaxWalkSpeed, DesiredSpeed,
		                              UGameplayStatics::GetWorldDeltaSeconds(this), MovementSpeedInterpSpeed);

	//원하는 스피드와 같아진다면
	if (UKismetMathLibrary::NearlyEqual_FloatFloat(CharacterRef->GetCharacterMovement()->MaxWalkSpeed, DesiredSpeed, 1.0f))
	{
		bShouldUpdateSpeed = false;
		CharacterRef->GetCharacterMovement()->MaxWalkSpeed = DesiredSpeed;
	}
}

float UCStatsManagerComponent::ModifyCurrentStatValue(EStats StatToModify, float InValue, bool StopRegen, bool IncludeModifiers)
{
	//변경 될 값이 0.0f면 함수를 종료합니다
	if (InValue == 0.0f) return 0.0f;

	//재생을 멈출 것이라면
	if (StopRegen)
		ClearRegenTimers(StatToModify);

	const float L_Value = GetCurrentValueForStat(StatToModify) + InValue;
	SetCurrentStatValue(StatToModify, L_Value);
	return L_Value;
}

float UCStatsManagerComponent::ModifyMaxStatValue(EStats StatToModify, float InValue)
{
	//수정 할 값이 0이라면 return
	if (InValue == 0.0f) return 0.0f;

	const float L_Value = GetMaxValueForStat(StatToModify) + InValue;
	UpdateMaxStatValue(StatToModify, L_Value);
	return L_Value;
}

void UCStatsManagerComponent::SetCurrentStatValue(EStats StatToModify, float InValue)
{
	const float L_ClampMax = GetValueForStat(StatToModify, EStatValueType::MaxValue); //최대값으로 최대범위를 지정합니다
	const float L_Value = UKismetMathLibrary::FClamp(InValue, 0.0f, L_ClampMax); //현재 값의 범위를 제한합니다

	UpdateCurrentStatValue(StatToModify, L_Value);
	OnCurrentStatValueUpdated(StatToModify, L_Value);
}

void UCStatsManagerComponent::SetBaseStatValue(EStats StatToModify, float InValue)
{
	const float L_Value = UKismetMathLibrary::FClamp(InValue, 0.0f, GetMaxValueForStat(StatToModify));
	UpdateBaseStatValue(StatToModify, L_Value);
	SetCurrentStatValue(StatToModify, GetValueForStat(StatToModify, EStatValueType::ModifierValue));
}

void UCStatsManagerComponent::SetMaxStatValue(EStats StatToModify, float InValue)
{
	//Stats.MaxValue Update
	if (Stats.Contains(StatToModify))
		Stats.Find(StatToModify)->MaxValue = InValue;

	LocalStatMaxValues.Add(StatToModify, InValue);
	UpdateMaxStatValue(StatToModify, InValue);
	OnMaxValueSet(StatToModify, InValue);
}

void UCStatsManagerComponent::SetMaxAndBaseStatValue(EStats StatToModify, float InValue)
{
	SetMaxStatValue(StatToModify, InValue);
	SetBaseStatValue(StatToModify, InValue);
}

void UCStatsManagerComponent::UpdateStat(EStats StatToUpdate)
{
	UpdateMaxStatValue(StatToUpdate, GetMaxValueForStat(StatToUpdate));
	SetCurrentStatValue(StatToUpdate, GetBaseValueForStat(StatToUpdate));
}

void UCStatsManagerComponent::UpdateBaseStats(EStats InStat, FUserEditableStats InValue)
{
	Stats.Add(InStat, InValue);
}

void UCStatsManagerComponent::UpdateStatsMap(EStats InStat, float InBaseValue, float InMaxValue)
{
	FFloatAttribute AddAttribute;
	AddAttribute.BaseValue = InBaseValue;
	AddAttribute.MaxValue = InMaxValue;
	InternalStats.Add(InStat, AddAttribute);
}

void UCStatsManagerComponent::UpdateCurrentStatValue(EStats StatToUpdate, float InValue)
{
	if (InternalStats.Contains(StatToUpdate))
		InternalStats.Find(StatToUpdate)->CurrentValue = InValue; //InternalStat CurrentValue Update

	LocalStatCurrentValues.Add(StatToUpdate, InValue); //LocalStatCurrentValue Update

	//OnCurrentValueUpdated 호출
	if (OnCurrentValueUpdated.IsBound())
		OnCurrentValueUpdated.Broadcast(StatToUpdate, InValue, GetMaxValueForStat(StatToUpdate));
}

void UCStatsManagerComponent::UpdateBaseStatValue(EStats StatToUpdate, float InValue)
{
	//InternalStat BaseValue Update
	if (InternalStats.Contains(StatToUpdate))
		InternalStats.Find(StatToUpdate)->BaseValue = InValue;

	//Stats BaseValue Update
	if (Stats.Contains(StatToUpdate))
	{
		FUserEditableStats NewUserEditableStatsStruct = *Stats.Find(StatToUpdate);
		NewUserEditableStatsStruct.BaseValue = InValue;
		Stats.Add(StatToUpdate, NewUserEditableStatsStruct);
	}
	else
	{
		FUserEditableStats AddStat;
		AddStat.BaseValue = InValue;
		Stats.Add(StatToUpdate, AddStat);
	}

	UpdateCurrentStatValue(StatToUpdate, InValue);
	OnBaseStatValueUpdated(StatToUpdate, InValue);
}

void UCStatsManagerComponent::UpdateMaxStatValue(EStats StatToUpdate, float InValue)
{
	//InternalStat MaxValue Update
	if (InternalStats.Contains(StatToUpdate))
		InternalStats.Find(StatToUpdate)->MaxValue = InValue;

	LocalStatMaxValues.Add(StatToUpdate, InValue); //LocalStatMaxValue Update

	WhenMaxStatValueUpdated(StatToUpdate, InValue); //해당 스텟의 최대값이 업데이트 됬다면 변하는  로직 실행

	//OnMaxStatValueUpdated 호출
	if (OnMaxStatValueUpdated.IsBound())
		OnMaxStatValueUpdated.Broadcast(StatToUpdate, InValue);
}

void UCStatsManagerComponent::GainSouls(int32 InSoulsToAdd)
{
	//ResetRecentlyReceivedSouls 함수를 클리어합니다
	const FString Function_ResetRecentlyReceivedSouls = TEXT("ResetRecentlyReceivedSouls");
	UKismetSystemLibrary::K2_ClearTimer(this, Function_ResetRecentlyReceivedSouls);

	//최근에 받은 영혼 + 추가 될 영혼
	RecentlyReceivedSouls = InSoulsToAdd + RecentlyReceivedSouls;

	//ResetRecentlyRecievedSouls 타이머를 시작한다
	UKismetSystemLibrary::K2_SetTimer(this, Function_ResetRecentlyReceivedSouls, 1.5f, false);

	ModifySoulsAmount(InSoulsToAdd); //소울의 갯수를 수정한다

	//OnSoulsGained 델리게이트 호출
	if (OnSoulsGained.IsBound())
		OnSoulsGained.Broadcast(RecentlyReceivedSouls);
}

void UCStatsManagerComponent::ResetRecentlyReceivedSouls()
{
	RecentlyReceivedSouls = 0;
	const FString ClearFunction_ResetRecentlyReceivedSouls = TEXT("ResetRecentlyReceivedSouls");
	UKismetSystemLibrary::K2_ClearTimer(this, ClearFunction_ResetRecentlyReceivedSouls);
}

int32 UCStatsManagerComponent::ModifySoulsAmount(int32 InSouls)
{
	if (InSouls != 0)
	{
		const int32 L_ModifiedSoulsAmount = Souls + InSouls; //현재 소울 + 수정될 소울 값을 더한다
		UpdateSoulsAmount(L_ModifiedSoulsAmount, true); //소울의 갯수를 업데이트한다
		return L_ModifiedSoulsAmount;
	}
	return 0;
}

void UCStatsManagerComponent::UpdateSoulsAmount(int32 SoulAmount, bool PlayUpdateAnimation)
{
	//현재 영혼의 갯수를 설정
	Souls = SoulAmount;

	if (OnSoulsAmountUpdated.IsBound())
		OnSoulsAmountUpdated.Broadcast(Souls, PlayUpdateAnimation);
}

void UCStatsManagerComponent::SetRequiredSoulsForNextLevel(int32 InSouls)
{
	RequiredSoulsForNextLevel = InSouls;
	if (OnSoulsRequiredForNextLevelUpdated.IsBound())
		OnSoulsRequiredForNextLevelUpdated.Broadcast(RequiredSoulsForNextLevel);
}

void UCStatsManagerComponent::IncrementCharacterLevel()
{
	CharacterLevel++;
}

int32 UCStatsManagerComponent::GetCurrentAttributeLevel(EAttributes InAttribute)
{
	if (Attributes.Contains(InAttribute))
		return *Attributes.Find(InAttribute);
	return NOTFIND_STATVALUE;
}

int32 UCStatsManagerComponent::GetAttributeLevelUpIndex(EAttributes InAttribute)
{
	for (int32 i = 0; i < LevelingUpStatsValues.Num(); i++)
	{
		if (LevelingUpStatsValues[i].Attribute == InAttribute)
			return i;
	}
	return -1;
}

void UCStatsManagerComponent::IncrementAttributeLevel(EAttributes InAttribute)
{
	CHECK(Attributes.Contains(InAttribute));
	const int32 L_AttributeLevel = *Attributes.Find(InAttribute);
	const int32 L_InValue = UKismetMathLibrary::Clamp(L_AttributeLevel + 1, 0, 200);
	Attributes.Add(InAttribute, L_InValue);
}

void UCStatsManagerComponent::LevelUpAttribute(EAttributes InAttributeToLevel)
{
	IncrementCharacterLevel(); //캐릭터를 레벨 업 +1 합니다

	//지정한 속성 유형의 구조체를 가져옵니다
	FAssociatedStats L_LevelingUpStatValues = LevelingUpStatsValues[GetAttributeLevelUpIndex(InAttributeToLevel)];

	//현재 속성타입과 새로들어온 속성 타입이 일치하면
	if (L_LevelingUpStatValues.Attribute == InAttributeToLevel)
	{
		IncrementAttributeLevel(InAttributeToLevel); //속성 값을 클램프1~100으로 제한하고 레벨업을 한다

		TArray<EStats> OutStatsKeys;
		L_LevelingUpStatValues.StatsToModify.GetKeys(OutStatsKeys);

		for (const auto StatKey : OutStatsKeys)
		{
			//스텟과 연결된 플롯커브를 찾습니다
			const UCurveFloat* L_StatCurveFloat = *L_LevelingUpStatValues.StatsToModify.Find(StatKey);

			//스텟타입의 커브값이 유효하다면
			if (IsValid(L_StatCurveFloat))
			{
				const float L_CurveTime = GetCurrentAttributeLevel(InAttributeToLevel);
				const float L_CurveValue = L_StatCurveFloat->GetFloatValue(L_CurveTime);
				SetMaxAndBaseStatValue(StatKey, L_CurveValue);
			}
		}
	}
}

float UCStatsManagerComponent::GetCurrentValueForStat(EStats InStat)
{
	if (LocalStatCurrentValues.Contains(InStat))
		return *LocalStatCurrentValues.Find(InStat);
	return NOTFIND_STATVALUE;
}

float UCStatsManagerComponent::GetMaxValueForStat(EStats InStat)
{
	if (LocalStatMaxValues.Contains(InStat))
		return *LocalStatMaxValues.Find(InStat);

	return NOTFIND_STATVALUE;
}

float UCStatsManagerComponent::GetBaseValueForStat(EStats InStat)
{
	if (Stats.Contains(InStat))
		return Stats.Find(InStat)->BaseValue;

	return NOTFIND_STATVALUE;
}

float UCStatsManagerComponent::GetMultiplierValueForStat(EStats InStat)
{
	if (LocalStatMultiplierValues.Contains(InStat))
		return *LocalStatMultiplierValues.Find(InStat);

	return NOTFIND_STATVALUE;
}

float UCStatsManagerComponent::GetCurrentStatValue_Clamped(EStats InStat, float Min, float Max)
{
	if (LocalStatCurrentValues.Contains(InStat))
		return UKismetMathLibrary::FClamp(*LocalStatCurrentValues.Find(InStat), Min, Max);

	return NOTFIND_STATVALUE;
}

float UCStatsManagerComponent::GetValueForStat(EStats InStat, EStatValueType ValueType)
{
	if (!InternalStats.Contains(InStat))
	{
		const FString StatString = CRPGLibrary::GetStatTypeAsString(InStat);
		LOG_ERROR(TEXT("찾지 못한 스텟 타입은 : %s 입니다"), *StatString);
	}
	else
	{
		const FFloatAttribute FindAttribute = *InternalStats.Find(InStat);

		switch (ValueType)
		{
		case EStatValueType::BaseValue:
			return FindAttribute.BaseValue;

		case EStatValueType::MaxValue:
			return FindAttribute.MaxValue;

		case EStatValueType::CurrentValue:
			return FindAttribute.CurrentValue;

		case EStatValueType::ModifierValue:
			return CalculateModifiedValue(InStat);

		case EStatValueType::IncrementValue:
			return FindAttribute.IncrementValue;

		case EStatValueType::MultiplierValue:
			return FindAttribute.MultiplierValue;
		}
	}

	return NOTFIND_STATVALUE;
}

float UCStatsManagerComponent::GetValuePercentage(EStats InStat)
{
	const float L_CurrentValue = GetCurrentValueForStat(InStat) * 100.0f;
	const float L_MaxValue = GetMaxValueForStat(InStat);
	return L_CurrentValue / L_MaxValue;
}

void UCStatsManagerComponent::OnBaseStatValueUpdated(EStats InStat, float InValue)
{
	switch (InStat)
	{
	case EStats::MovementSpeed:
		UpdateMovementSpeed(EGait::Run, InValue);
		break;

	case EStats::Weight:
		UpdateEncumbrance();
		break;

	default: break;
	}
}

void UCStatsManagerComponent::OnCurrentStatValueUpdated(EStats StatModified, float InValue)
{
	//수정해야할 타입
	switch (StatModified)
	{
	case EStats::Health:
		StatRegen(EStats::Health);
		break;

	case EStats::Mana:
		StatRegen(EStats::Mana);
		break;

	case EStats::Stamina:
		StatRegen(EStats::Stamina);
		break;

	case EStats::Poise:
		StatRegen(EStats::Poise);
		break;

	case EStats::Balance:
		StatRegen(EStats::Balance);
		break;

	case EStats::Weight:
		UpdateEncumbrance();
		break;

	case EStats::MovementSpeed:
		UpdateMovementSpeed(EGait::Run, InValue);
		break;

	case EStats::PoisonResist:
		StatRegen(EStats::PoisonResist);
		break;

	case EStats::BurningResist:
		StatRegen(EStats::BurningResist);
		break;

	default: break;
	}
}

void UCStatsManagerComponent::OnMaxValueSet(EStats InStat, float InValue)
{
	UpdateCurrentStatValue(InStat, GetCurrentValueForStat(InStat));

	switch (InStat)
	{
	case EStats::MovementSpeed:
		UpdateMovementSpeed(EGait::Sprint, InValue);
		break;

	default: break;
	}
}

void UCStatsManagerComponent::WhenMaxStatValueUpdated(EStats InStat, float InValue)
{
	switch (InStat)
	{
	case EStats::MovementSpeed:
		UpdateMovementSpeed(EGait::Sprint, InValue);
		break;

	case EStats::Weight:
		UpdateEncumbrance();
		break;

	default: break;
	}
}

void UCStatsManagerComponent::HealthRegen()
{
	const float L_HealthRegenRate = GetCurrentValueForStat(EStats::Health) + GetCurrentStatRegenRate(EStats::Health); //현재 스텟 값 + 재생될 양
	UpdateCurrentStatValue(EStats::Health, L_HealthRegenRate); //현재 체력 값을 업데이트한다

	//현재 체력 값이 최대 체력 값 보다 크거나 같다면 
	if (GetCurrentValueForStat(EStats::Health) >= GetMaxValueForStat(EStats::Health))
	{
		PauseStatRegenTimer(EStats::Health); //회복재생 함수를 일시정지 한다
		UpdateCurrentStatValue(EStats::Health, GetMaxValueForStat(EStats::Health)); //현재 체력 값을 최대 체력 값으로 업데이트 한다
	}
	else
	{
		//현재 체력 값이 0.0f보다 작다면 현재 값을 0.0f로 설정한다
		if (GetCurrentValueForStat(EStats::Health) <= 0.0f)
			UpdateCurrentStatValue(EStats::Health, 0.0f);
	}
}

void UCStatsManagerComponent::ManaRegen()
{
	const float L_ManaRegenRate = GetCurrentValueForStat(EStats::Mana) + GetCurrentStatRegenRate(EStats::Mana);
	UpdateCurrentStatValue(EStats::Mana, L_ManaRegenRate);

	//현재 마나 값이 최대 마나 값 보다 크거나 같다면 
	if (GetCurrentValueForStat(EStats::Mana) >= GetMaxValueForStat(EStats::Mana))
	{
		PauseStatRegenTimer(EStats::Mana); //마나 재생 함수를 일시정지 한다
		UpdateCurrentStatValue(EStats::Mana, GetMaxValueForStat(EStats::Mana)); //현재 마나 값을 최대 마나 값으로 업데이트 한다
	}
	else
	{
		if (GetCurrentValueForStat(EStats::Mana) <= 0.0f)
			UpdateCurrentStatValue(EStats::Mana, 0.0f); //현재 마나 값이 0.0f보다 작다면 현재 값을 0.0f로 설정한다
	}
}

void UCStatsManagerComponent::StaminaRegen()
{
	const float L_StaminaRegenRate = GetCurrentValueForStat(EStats::Stamina) + GetCurrentStatRegenRate(EStats::Stamina);
	UpdateCurrentStatValue(EStats::Stamina, L_StaminaRegenRate); //현재 스테미나 값을 업데이트한다

	//현재 스테미나 값이 최대 스테미나 값 보다 크거나 같다면 
	if (GetCurrentValueForStat(EStats::Stamina) >= GetMaxValueForStat(EStats::Stamina))
	{
		PauseStatRegenTimer(EStats::Stamina); //스테미나 재생 함수를 일시정지 한다
		UpdateCurrentStatValue(EStats::Stamina, GetMaxValueForStat(EStats::Stamina)); //현재 스테미나 값을 최대 스테미나 값으로 업데이트 한다
	}
	else
	{
		//현재 스테미나 값이 0.0f보다 작다면 현재 값을 0.0f로 설정한다
		if (GetCurrentValueForStat(EStats::Stamina) <= 0.0f)
			UpdateCurrentStatValue(EStats::Stamina, 0.0f);
	}
}

void UCStatsManagerComponent::PoiseRegen()
{
	const float L_PoiseRate = GetCurrentValueForStat(EStats::Poise) + GetCurrentStatRegenRate(EStats::Poise);
	const float L_PoiseRegenRate = UKismetMathLibrary::FClamp(L_PoiseRate, 0.0f, GetMaxValueForStat(EStats::Poise));
	UpdateCurrentStatValue(EStats::Poise, L_PoiseRegenRate); //현재 평정 값을 업데이트한다

	//현재 평정 값이 쵀대 평정 값보다 크다면
	if (GetCurrentValueForStat(EStats::Poise) >= GetMaxValueForStat(EStats::Poise))
		PauseStatRegenTimer(EStats::Poise); //평정 함수를 일시정지한다
}

void UCStatsManagerComponent::BalanceRegen()
{
	const float L_BalanceRate = GetCurrentValueForStat(EStats::Balance) + GetCurrentStatRegenRate(EStats::Balance);
	const float L_BalanceRegenRate = UKismetMathLibrary::FClamp(L_BalanceRate, 0.0f, GetMaxValueForStat(EStats::Balance));
	UpdateCurrentStatValue(EStats::Balance, L_BalanceRegenRate);

	//현재 형 값이 최대균형 값보다 크거나 같다면
	if (GetCurrentValueForStat(EStats::Balance) >= GetMaxValueForStat(EStats::Balance))
		PauseStatRegenTimer(EStats::Balance); //균형 함수를 일시정지한다
	else
	{
		//현재 균형값이 0.0f보다 작거나 같다면
		if (GetCurrentValueForStat(EStats::Balance) <= 0.0f)
			PauseStatRegenTimer(EStats::Balance); //균형 함수를 일시정지한다
	}
}

void UCStatsManagerComponent::BurningStatTimer()
{
	const float L_BurningRate = GetCurrentValueForStat(EStats::Health) + GetStatusEffectModificationRate(EStatusEffects::Burning);
	UpdateCurrentStatValue(EStats::Health, L_BurningRate);

	if (OnDamageReceived.IsBound())
	{
		const float L_ABSBurningRate = UKismetMathLibrary::Abs(GetStatusEffectModificationRate(EStatusEffects::Burning));
		OnDamageReceived.Broadcast(EDamageType::BurningDamage, L_ABSBurningRate);
	}

	//현재 체력 값이 최대 체력 값보다 크거나 같다면
	if (GetCurrentValueForStat(EStats::Health) >= GetMaxValueForStat(EStats::Health))
	{
		ClearStatusEffectTimer(EStatusEffects::Burning); //버닝 상태효과 함수를 클리어한다
		UpdateCurrentStatValue(EStats::Health, GetMaxValueForStat(EStats::Health)); //현재 체력 값을 최대 체력값으로 업데이트한다
	}
	else //현재 체력 값이 최대 체력 값보다 크지 않거나 같지 않다면
	{
		if (GetCurrentValueForStat(EStats::Health) <= 0.0f) //현재 체력 값이 0.0f보다 작거나 같다면
		{
			ClearStatusEffectTimer(EStatusEffects::Burning); //버닝 상태효과 함수를 클리어한다

			UpdateCurrentStatValue(EStats::Health, 0.0f); //현재 체력 값을 0.0f로 업데이트한다

			if (OnDamageReceived.IsBound())
				OnDamageReceived.Broadcast(EDamageType::BurningDamage, GetMaxValueForStat(EStats::Health));
		}
	}
}

void UCStatsManagerComponent::HealingMedicineTimer()
{
	const float L_HealingRate = GetCurrentValueForStat(EStats::Health) + GetStatusEffectModificationRate(EStatusEffects::Healing);
	UpdateCurrentStatValue(EStats::Health, L_HealingRate); //현재 체력값을 업데이트한다

	//현재 체력 값이 최대 체력 값보다 크거나 같다면
	if (GetCurrentValueForStat(EStats::Health) >= GetMaxValueForStat(EStats::Health))
	{
		ClearStatusEffectTimer(EStatusEffects::Healing);
		UpdateCurrentStatValue(EStats::Health, GetMaxValueForStat(EStats::Health)); //현재 체력 값을 최대 체력값으로 업데이트한다
	}
	else //현재 체력 값이 최대 체력 값보다 크지않거나 같지 않다면
	{
		//현재 체력 값이 0.0f보다 작거나 같다면
		if (GetCurrentValueForStat(EStats::Health) <= 0.0f)
		{
			ClearStatusEffectTimer(EStatusEffects::Healing);
			UpdateCurrentStatValue(EStats::Health, 0.0f); //현재 체력 값을 0.0f로 업데이트한다
		}
	}
}

void UCStatsManagerComponent::PoisonStatTimer()
{
	const float L_PoisonRate = GetCurrentValueForStat(EStats::Health) + GetStatusEffectModificationRate(EStatusEffects::Poison);
	UpdateCurrentStatValue(EStats::Health, L_PoisonRate); //현재 체력값을 업데이트한다

	if (OnDamageReceived.IsBound())
	{
		const float L_ABSPoisonRate = UKismetMathLibrary::Abs(GetStatusEffectModificationRate(EStatusEffects::Poison));
		OnDamageReceived.Broadcast(EDamageType::PoisonDamage, L_ABSPoisonRate);
	}

	//현재 체력 값이 최대 체력 값보다 크거나 같다면
	if (GetCurrentValueForStat(EStats::Health) >= GetMaxValueForStat(EStats::Health))
	{
		ClearStatusEffectTimer(EStatusEffects::Poison); //독 상태효과 함수를 클리어한다
		UpdateCurrentStatValue(EStats::Health, GetMaxValueForStat(EStats::Health)); //현재 체력 값을 최대 체력값으로 업데이트한다
	}
	else //현제 체력 값이 최대체력보다 크거나 같지 않다면
	{
		//현재 체력 값이 0.0f보다 작거나 같다면
		if (GetCurrentValueForStat(EStats::Health) <= 0.0f)
		{
			ClearStatusEffectTimer(EStatusEffects::Poison); //독 상태효과 함수를 클리어한다
			UpdateCurrentStatValue(EStats::Health, 0.0f); //현재 체력 값을 0.0f로 업데이트한다
			if (OnDamageReceived.IsBound())
				OnDamageReceived.Broadcast(EDamageType::PoisonDamage, GetMaxValueForStat(EStats::Health));
		}
	}
}

void UCStatsManagerComponent::SprintStaminaCost()
{
	//Speed가 0이 아니라면
	if (CharacterRef->GetVelocity().Size() != 0.0f)
	{
		UpdateCurrentStatValue(EStats::Stamina, GetCurrentValueForStat(EStats::Stamina) - 2.0f); //현재 스테미나 값을 설정한다
		StatRegen(EStats::Stamina); //스테미나를 리젠한다

		//스테미나가 10.0f 보다 작다면
		if (GetCurrentValueForStat(EStats::Stamina) < 10.0f)
		{
			//현재 스프린트 질주 함수를 중단한다.
			UKismetSystemLibrary::K2_PauseTimer(this, TEXT("SprintStaminaCost"));
			IInterface_GeneralCharacter* CastGeneralCharacter_Interface = Cast<IInterface_GeneralCharacter>(CharacterRef);
			CHECK(CastGeneralCharacter_Interface!=nullptr);
			CastGeneralCharacter_Interface->I_StopSprinting();
		}
	}
	else //Speed가 0이라면
	{
		IInterface_GeneralCharacter* CastGeneralCharacter_Interface = Cast<IInterface_GeneralCharacter>(CharacterRef);
		CHECK(CastGeneralCharacter_Interface!=nullptr);
		CastGeneralCharacter_Interface->I_StopSprinting();
		UKismetSystemLibrary::K2_PauseTimer(this, TEXT("SprintStaminaCost"));
	}
}

void UCStatsManagerComponent::BurningResistRegen()
{
	const float L_BurningResistRate = GetCurrentValueForStat(EStats::BurningResist) - GetCurrentStatRegenRate(EStats::BurningResist);
	UpdateCurrentStatValue(EStats::BurningResist, L_BurningResistRate); //현재 화영저항 값을 업데이트한다

	//현재 화염저항 값이 0.0f보다 작거나 같다면
	if (GetCurrentValueForStat(EStats::BurningResist) <= 0.0f)
	{
		PauseStatRegenTimer(EStats::BurningResist); //타이머를 일시정지한다
		UpdateCurrentStatValue(EStats::BurningResist, 0.0f); //현재 화영저항 값을 업데이트한다
	}
}

void UCStatsManagerComponent::PoisonResistRegen()
{
	const float L_PoisonResistRate = GetCurrentValueForStat(EStats::PoisonResist) - GetCurrentStatRegenRate(EStats::PoisonResist);
	UpdateCurrentStatValue(EStats::PoisonResist, L_PoisonResistRate); //현재 화영저항 값을 업데이트한다

	if (GetCurrentValueForStat(EStats::PoisonResist) <= 0.0f)
	{
		PauseStatRegenTimer(EStats::PoisonResist); //타이머를 일시정지한다
		UpdateCurrentStatValue(EStats::PoisonResist, 0.0f); //현재 화영저항 값을 업데이트한다
	}
}

float UCStatsManagerComponent::AttackStatCost(EAttackTypes InAttackType)
{
	const float L_ModifierMeleeAttackStatCost = GetValueForStat(EStats::MeleeAttackStatCost, EStatValueType::ModifierValue);
	return L_ModifierMeleeAttackStatCost * GetAttackTypeMultiplier(InAttackType);
}

float UCStatsManagerComponent::GetCombatActionStatCost(ECharacterAction CombatAction)
{
	if (CombatActionStatCost.Contains(CombatAction))
		return *CombatActionStatCost.Find(CombatAction);

	return 1.0f;
}

float UCStatsManagerComponent::GetAttackTypeMultiplier(EAttackTypes InAttackType)
{
	if (AttackTypeStatMultipliers.Contains(InAttackType))
	{
		const float FoundMultiplier = *AttackTypeStatMultipliers.Find(InAttackType);
		return UKismetMathLibrary::FClamp(FoundMultiplier, 1.0f, 100.f);
	}

	return 1.0f;
}

void UCStatsManagerComponent::AddStatModifier(FStatModifier InStatModifier, bool IsReplace)
{
	//변경되어야 한다면
	if (IsReplace)
	{
		for (int32 i = 0; i < AppliedStatModifiers.Num(); i++)
		{
			//적용 된 스텟 변경 값의  스텟 타입과 변경ID가 같다면 
			if (AppliedStatModifiers[i].ModifierType == InStatModifier.ModifierType &&
				AppliedStatModifiers[i].ModifierID == InStatModifier.ModifierID)
				AppliedStatModifiers[i] = InStatModifier;
		}
	}

	AppliedStatModifiers.Add(InStatModifier); //적용된 수정 스텟 값에 추가한다

	const EStats L_StatToModify = InStatModifier.ModifierType; //변경 될 스텟 타입
	const float L_IncrementValue = GetValueForStat(L_StatToModify, EStatValueType::IncrementValue) + InStatModifier.Increment; //추가 될 스텟 값 설정
	const float L_Multiplier = GetValueForStat(L_StatToModify, EStatValueType::MultiplierValue) + InStatModifier.BaseMultiplier; //추가 될 배율 값 설정

	UpdateStatIncrementValue(L_StatToModify, L_IncrementValue); //증가 값을 업데이트 한다
	UpdateMultiplierValue(L_StatToModify, L_Multiplier); //배율 값을 업데이트 한다

	//변경된 값이 최대값 보다 크다면 그 값으로 바꾸자아아아아앙아아아앙아앙
	if (GetValueForStat(L_StatToModify, EStatValueType::ModifierValue) > GetValueForStat(L_StatToModify, EStatValueType::MaxValue))
		UpdateMaxStatValue(L_StatToModify, GetValueForStat(L_StatToModify, EStatValueType::ModifierValue));

	//OnModifierAdded 델리게이트 호출
	if (OnModifierAdded.IsBound())
		OnModifierAdded.Broadcast(L_StatToModify);

	SetCurrentStatValue(L_StatToModify, GetValueForStat(L_StatToModify, EStatValueType::CurrentValue));
	OnCurrentStatValueUpdated(L_StatToModify, GetValueForStat(L_StatToModify, EStatValueType::CurrentValue));
}

void UCStatsManagerComponent::RemoveStatModifier(FStatModifier InStatModifier)
{
	//동일 한 수정 값 이라면 배열에서 제거한다
	for (int32 i = 0; i < AppliedStatModifiers.Num(); i++)
	{
		//적용 된 스텟 수정값의 식별자와 입력받은 스텟식별자가 같다면
		if (AppliedStatModifiers[i].ModifierID == InStatModifier.ModifierID)
		{
			//적용 된 스텟 수정 값들 에서 제거합니다
			AppliedStatModifiers.RemoveAt(i);

			const EStats L_StatToModify = InStatModifier.ModifierType; //변경 될 스텟 유형 설정

			const float L_IncrementValue = GetValueForStat(L_StatToModify, EStatValueType::IncrementValue) - InStatModifier.Increment; //원래 값 - 빼야할 값
			const float L_Multiplier = GetValueForStat(L_StatToModify, EStatValueType::MultiplierValue) - InStatModifier.BaseMultiplier;
			//원래 갑 - 빼야할 값

			//내부 스텟 값을 업데이트 합니다
			UpdateStatIncrementValue(L_StatToModify, L_IncrementValue);
			UpdateMultiplierValue(L_StatToModify, L_Multiplier);

			//현재 적용 값이 최대 값보다 크다면 현재 적용 값으로 맞춰라
			if (GetValueForStat(L_StatToModify, EStatValueType::ModifierValue) > GetValueForStat(L_StatToModify, EStatValueType::MaxValue))
				UpdateMaxStatValue(L_StatToModify, GetValueForStat(L_StatToModify, EStatValueType::ModifierValue));

			//델리게이트 호출
			if (OnModifierRemoved.IsBound())
				OnModifierRemoved.Broadcast(L_StatToModify);

			//스텟 업데이트
			SetCurrentStatValue(L_StatToModify, GetValueForStat(L_StatToModify, EStatValueType::ModifierValue));
			OnCurrentStatValueUpdated(L_StatToModify, GetValueForStat(L_StatToModify, EStatValueType::ModifierValue));
		}
	}
}

void UCStatsManagerComponent::AddMultipleModifiers(const TArray<FStatModifier>& InModifiers)
{
	for (const auto AddModifyStat : InModifiers)
		AddStatModifier(AddModifyStat, true);
}

void UCStatsManagerComponent::RemoveMultipleModifiers(const TArray<FStatModifier>& InModifiers)
{
	for (const auto removeModifyStat : InModifiers)
		RemoveStatModifier(removeModifyStat);
}

float UCStatsManagerComponent::CalculateModifiedValue(EStats InStat)
{
	if (InternalStats.Contains(InStat))
	{
		const FFloatAttribute FindAttribute = *InternalStats.Find(InStat);

		const float L_Increment = FindAttribute.BaseValue + FindAttribute.IncrementValue; //내부 스텟의 기본 값 + 내부 스텟의 증가 값
		const float L_MultiplierValue = FindAttribute.BaseValue * FindAttribute.MultiplierValue; //내부 스텟의 기본 값 * 내부 스텟의 배율 값
		return L_Increment + L_MultiplierValue;
	}
	return NOTFIND_STATVALUE;
}

void UCStatsManagerComponent::UpdateStatIncrementValue(EStats StatToUpdate, float InValue)
{
	//내부 스텟에 업데이트해야 할 스텟이 있다면 증가 값을 InValue로 설정한다
	if (InternalStats.Contains(StatToUpdate))
		InternalStats.Find(StatToUpdate)->IncrementValue = InValue;
}

void UCStatsManagerComponent::UpdateMultiplierValue(EStats StatToUpdate, float InValue)
{
	if (InternalStats.Contains(StatToUpdate))
		InternalStats.Find(StatToUpdate)->MultiplierValue = InValue;
	LocalStatMultiplierValues.Add(StatToUpdate, InValue);
}

void UCStatsManagerComponent::UpdateCurrentStatRegenCoolDown(EStats InStat, float InValue)
{
	if (StatRegenCoolDown.Contains(InStat))
		StatRegenCoolDown.Find(InStat)->CurrentValue = InValue;
	CurrentStatRegenCoolDown.Add(InStat, InValue);
}

void UCStatsManagerComponent::UpdateBaseStatRegenCoolDown(EStats InStat, float InValue)
{
	if (StatRegenCoolDown.Contains(InStat))
		StatRegenCoolDown.Find(InStat)->BaseValue = InValue;
	UpdateCurrentStatRegenCoolDown(InStat, InValue);
}

void UCStatsManagerComponent::UpdateCurrentStatRegenRate(EStats InStat, float InValue)
{
	if (StatRegenRate.Contains(InStat))
		StatRegenRate.Find(InStat)->CurrentValue = InValue;
	CurrentStatRegenRate.Add(InStat, InValue);
}

void UCStatsManagerComponent::UpdateBaseStatRegenRate(EStats InStat, float InValue)
{
	if (StatRegenRate.Contains(InStat))
		StatRegenRate.Find(InStat)->BaseValue = InValue;
	UpdateCurrentStatRegenRate(InStat, InValue);
}

void UCStatsManagerComponent::UpdateCurrentStatRegenTickInterval(EStats InStat, float InValue)
{
	if (StatRegenTickInterval.Contains(InStat))
		StatRegenTickInterval.Find(InStat)->CurrentValue = InValue;
	CurrentStatRegenTickInterval.Add(InStat, InValue);
}

void UCStatsManagerComponent::UpdateBaseStatRegenTickInterval(EStats InStat, float InValue)
{
	if (StatRegenTickInterval.Contains(InStat))
		StatRegenTickInterval.Find(InStat)->BaseValue = InValue;
	UpdateCurrentStatRegenTickInterval(InStat, InValue);
}

void UCStatsManagerComponent::PauseStatRegenTimer(EStats StatToPause)
{
	CHECK(StatRegenTimerHandels.Contains(StatToPause));
	if (UKismetSystemLibrary::K2_IsTimerActiveHandle(this, *StatRegenTimerHandels.Find(StatToPause)))
		UKismetSystemLibrary::K2_PauseTimerHandle(this, *StatRegenTimerHandels.Find(StatToPause));
}

void UCStatsManagerComponent::ClearRegenTimers(EStats StatToClearRegen)
{
	UKismetSystemLibrary::K2_ClearTimer(this, *StatRegenFunctions.Find(StatToClearRegen));
}

void UCStatsManagerComponent::StatRegen(EStats StatToRegen)
{
	switch (StatToRegen)
	{
	case EStats::Health:
		StartStatRegen(EStats::Health);
		break;

	case EStats::Stamina:
		StartStatRegen(EStats::Stamina);
		break;

	case EStats::Mana:
		StartStatRegen(EStats::Mana);
		break;

	case EStats::Poise:
		StartStatRegen(EStats::Poise);
		break;

	case EStats::Balance:
		StartStatRegen(EStats::Balance);
		break;

	case EStats::PoisonResist:
		StartStatRegen(EStats::PoisonResist);
		break;

	case EStats::BurningResist:
		StartStatRegen(EStats::BurningResist);
		break;

	default: break;
	}
}

float UCStatsManagerComponent::GetCurrentStatRegenRate(EStats InStat)
{
	CHECK(CurrentStatRegenRate.Contains(InStat), NOTFIND_STATVALUE);
	return *CurrentStatRegenRate.Find(InStat);
}

float UCStatsManagerComponent::GetStatRegenRate(EStats InStat, EStatValueTypeSimplified StatValueType)
{
	if (StatRegenRate.Contains(InStat))
	{
		const FFloatStatValue FindStatRegenRate = *StatRegenRate.Find(InStat);
		switch (StatValueType)
		{
		case EStatValueTypeSimplified::BaseValue: return FindStatRegenRate.BaseValue;
		case EStatValueTypeSimplified::CurrentValue: return FindStatRegenRate.CurrentValue;
		case EStatValueTypeSimplified::None: return 0.0f;
		}
	}
	return 0.0f;
}

float UCStatsManagerComponent::GetStatRegenTimerCoolDown(EStats InStat, EStatValueTypeSimplified StatValueType)
{
	if (StatRegenCoolDown.Contains(InStat))
	{
		const FFloatStatValue FindStatRegenCoolDown = *StatRegenCoolDown.Find(InStat);
		switch (StatValueType)
		{
		case EStatValueTypeSimplified::BaseValue:
			return FindStatRegenCoolDown.BaseValue;

		case EStatValueTypeSimplified::CurrentValue:
			return FindStatRegenCoolDown.CurrentValue;

		case EStatValueTypeSimplified::None:
			return 0.0f;
		}
	}
	return 0.0f;
}

float UCStatsManagerComponent::GetStatRegenTickInterval(EStats InStat, EStatValueTypeSimplified StatValueType)
{
	if (StatRegenTickInterval.Contains(InStat))
	{
		const FFloatStatValue FindStatRegenTickInterval = *StatRegenTickInterval.Find(InStat);

		switch (StatValueType)
		{
		case EStatValueTypeSimplified::BaseValue: return FindStatRegenTickInterval.BaseValue;
		case EStatValueTypeSimplified::CurrentValue: return FindStatRegenTickInterval.CurrentValue;
		case EStatValueTypeSimplified::None: return 0.0f;
		}
	}
	return 0.0f;
}

void UCStatsManagerComponent::ModifyRegenPercentForStat(EStats InStat, int32 InPercentage)
{
	const float L_Value = CCombatLibrary::GetMultiplierForPercentage(InPercentage) * GetStatRegenRate(InStat, EStatValueTypeSimplified::BaseValue);
	UpdateCurrentStatRegenRate(InStat, L_Value);
}

EEncumbrance UCStatsManagerComponent::CalculateEncumbrance(float InValue)
{
	const float L_Weight = InValue; //계산 할 무게
	const float L_MaxEquipLoad = GetMaxValueForStat(EStats::Weight); //최대 무게

	if (UKismetMathLibrary::InRange_FloatFloat(L_Weight, 0.0f, L_MaxEquipLoad * 0.25f))
		return EEncumbrance::Light;
	if (UKismetMathLibrary::InRange_FloatFloat(L_Weight, L_MaxEquipLoad * 0.25f, L_MaxEquipLoad * 0.5f))
		return EEncumbrance::Medium;
	if (UKismetMathLibrary::InRange_FloatFloat(L_Weight, L_MaxEquipLoad * 0.5f, L_MaxEquipLoad))
		return EEncumbrance::Heavy;
	return EEncumbrance::Medium;
}

void UCStatsManagerComponent::UpdateEncumbrance()
{
	const float L_EquipLoad = GetCurrentValueForStat(EStats::Weight);
	CurrentEncumbrance = CalculateEncumbrance(L_EquipLoad);
}

void UCStatsManagerComponent::ApplyStatusEffect(const FStatusEffect& InStatusEffect, float InEffectDuration)
{
	StatusEffectDuration.Add(InStatusEffect.StatusEffect, InEffectDuration); //현재 상태효과의 지속시간을 새로운 지속시간으로 설정
	UpdateAppliedStatusEffects(InStatusEffect); //적용된 상태 효과 업데이트

	//상태효과 타이머를 시작한다
	StartStatusEffectTimer(InStatusEffect.StatusEffect);

	//지정 된 타이머가 존재하고 활성화 상태인라면
	if (IsStatusEffectApplied(InStatusEffect))
	{
		Event_OnStatusEffectApplied(InStatusEffect.StatusEffect); //상태 효과 적용 이벤트 델리게이트 호출	


		auto L_StatusEffect = InStatusEffect.StatusEffect;
		float L_RetirggerableDelayWait = *StatusEffectDuration.Find(L_StatusEffect);
		//스탑 타이머 핸들이 존재한다면
		if (StatusEffectStopTimerHandles.Contains(L_StatusEffect))
		{
			//타이머 핸들이 존재하고 활성화 되어 있다면
			if (UKismetSystemLibrary::K2_IsTimerActiveHandle(this, *StatusEffectStopTimerHandles.Find(L_StatusEffect)))
			{
				UKismetSystemLibrary::K2_ClearTimerHandle(this, *StatusEffectStopTimerHandles.Find(L_StatusEffect));
				FTimerDelegate TimerDel;
				TimerDel.BindUFunction(this, FName("StopStatusEffect"), InStatusEffect);
				GetWorld()->GetTimerManager().SetTimer(*StatusEffectStopTimerHandles.Find(L_StatusEffect), TimerDel, L_RetirggerableDelayWait, false);
			}
			else //타이머 핸들이 존재하나 활성화되어 있지 않다면
			{
				FTimerDelegate TimerDel;
				TimerDel.BindUFunction(this, FName("StopStatusEffect"), InStatusEffect);
				GetWorld()->GetTimerManager().SetTimer(*StatusEffectStopTimerHandles.Find(L_StatusEffect), TimerDel, L_RetirggerableDelayWait, false);
			}
		}
		else //스탑 타이머 핸들이 존재하지 않는다면
		{
			FTimerDelegate TimerDel;
			TimerDel.BindUFunction(this, FName("StopStatusEffect"), InStatusEffect);
			GetWorld()->GetTimerManager().SetTimer(*StatusEffectStopTimerHandles.Find(L_StatusEffect), TimerDel, L_RetirggerableDelayWait, false);
		}
	}
}

bool UCStatsManagerComponent::IsStatusEffectApplied(FStatusEffect InStatusEffect)
{
	CHECK(StatusEffectRegenFunctions.Contains(InStatusEffect.StatusEffect), false);
	const FString FunctionName = *StatusEffectRegenFunctions.Find(InStatusEffect.StatusEffect);
	return UKismetSystemLibrary::K2_IsTimerActive(this, FunctionName);
}

float UCStatsManagerComponent::GetStatusEffectModificationRate(EStatusEffects InStatusEffect)
{
	CHECK(StatusEffectModificationRate.Contains(InStatusEffect), NOTFIND_STATVALUE);
	return *StatusEffectModificationRate.Find(InStatusEffect);
}

UParticleSystem* UCStatsManagerComponent::GetStatusEffectParticle(EStatusEffects InStatusEffectType)
{
	CHECK(StatusEffectParticles.Contains(InStatusEffectType), nullptr);
	return *StatusEffectParticles.Find(InStatusEffectType);
}

UParticleSystemComponent* UCStatsManagerComponent::GetAppliedStatusParticleComponent(EStatusEffects InStatusType)
{
	CHECK(AppliedStatusParticleComponents.Contains(InStatusType), nullptr);
	return *AppliedStatusParticleComponents.Find(InStatusType);
}

void UCStatsManagerComponent::ClearStatusEffectTimer(EStatusEffects StatusEffectTimerToClear)
{
	CHECK(StatusEffectRegenFunctions.Contains(StatusEffectTimerToClear));

	const FString ClearFunctionName = *StatusEffectRegenFunctions.Find(StatusEffectTimerToClear); //클리어할 타이머의 함수이름을 가지고와서 지운다
	UKismetSystemLibrary::K2_ClearTimer(this, ClearFunctionName); //함수를 클리어한다

	StatusEffectModificationRate.Remove(StatusEffectTimerToClear); //상태효과 타입의 양 제거
	StatusEffectTickInterval.Remove(StatusEffectTimerToClear); //상태효과 타입의 틱 인터벌 제거
	StatusEffectDuration.Remove(StatusEffectTimerToClear); //상태효과 타입의 지속시간 제거

	//상태효과가 끝난 델리게이트를 호출한다.
	if (OnStatusEffectRemoved.IsBound())
		OnStatusEffectRemoved.Broadcast(StatusEffectTimerToClear);
}

void UCStatsManagerComponent::RemoveStatusEffect(const FStatusEffect& InStatusEffect)
{
	const EStatusEffects L_AuxiliaryEffectToRemove = InStatusEffect.StatusEffect; //상태효과의 타입설정
	AppliedStatusEffects.Remove(InStatusEffect); //적용된 상태효과들에서 제거한다

	ClearStatusEffectTimer(L_AuxiliaryEffectToRemove);

	StatusEffectModificationRate.Remove(L_AuxiliaryEffectToRemove);
	StatusEffectTickInterval.Remove(L_AuxiliaryEffectToRemove);
	StatusEffectDuration.Remove(L_AuxiliaryEffectToRemove);

	if (OnStatusEffectRemoved.IsBound())
		OnStatusEffectRemoved.Broadcast(L_AuxiliaryEffectToRemove);
}

void UCStatsManagerComponent::UpdateAppliedStatusEffects(const FStatusEffect& InStatusEffect)
{
	AppliedStatusEffects.AddUnique(InStatusEffect); //적용된 상태효과에 고유타입을 추가한다

	StatusEffectModificationRate.Add(InStatusEffect.StatusEffect, InStatusEffect.StatusEffectIncrement); //상태효과 수정 값 설정
	StatusEffectTickInterval.Add(InStatusEffect.StatusEffect, InStatusEffect.TimerTickInterval); //상태효과 틱인터벌 값 설정
}

void UCStatsManagerComponent::UpdateAppliedStatusParticleComponent(EStatusEffects InStatusType, UParticleSystemComponent* InParticleComponent)
{
	AppliedStatusParticleComponents.Add(InStatusType, InParticleComponent);
}

bool UCStatsManagerComponent::RemoveAppliedStatusParticleComponent(EStatusEffects InStatusType)
{
	//파티클 컴포넌트가 유효하다면
	if (IsValid(GetAppliedStatusParticleComponent(InStatusType)))
	{
		//파티클컴포넌트를 Destroy 한다.
		GetAppliedStatusParticleComponent(InStatusType)->DestroyComponent();
		const auto RemoveCount = AppliedStatusParticleComponents.Remove(InStatusType);

		if (RemoveCount > 0)
			return true;
	}
	return false;
}

void UCStatsManagerComponent::Event_OnStatusEffectApplied(EStatusEffects Selection)
{
	if (OnStatusEffectApplied.IsBound())
		OnStatusEffectApplied.Broadcast(Selection);
}

float UCStatsManagerComponent::StatPercentageIncreaseAmount(EStats InStatToIncrease, int32 InPercentageAmount)
{
	const float L_MaxStatValue = GetMaxValueForStat(InStatToIncrease);
	return L_MaxStatValue * CCombatLibrary::GetMultiplierForPercentage(InPercentageAmount);
}

void UCStatsManagerComponent::ConsumeItem(EStats InStatToModify, FStatusEffect InStatusEffect, int32 InModificationPercentage)
{
	//새로운 상태효과의 상태효과 타입이 None 이 아니라면 상태효과를 적용한다.
	if (InStatusEffect.StatusEffect != EStatusEffects::None)
		ApplyStatusEffect(InStatusEffect, InStatusEffect.EffectDuration);
	else //새로운 상태효과의 상태효과 타입이 None 이라면 
	{
		const float L_Value = StatPercentageIncreaseAmount(InStatToModify, InModificationPercentage);
		ModifyCurrentStatValue(InStatToModify, L_Value, true, true);
	}
}

void UCStatsManagerComponent::AddItemModifiers(UCRPGItemAsset* InItem)
{
	//아이템이 유효하다면
	if (IsValid(InItem))
	{
		TArray<EStats> ItemStatsKey;
		InItem->ItemStats.GetKeys(ItemStatsKey);

		for (int32 i = 0; i < ItemStatsKey.Num(); i++)
		{
			const EStats L_ModifierStatType = ItemStatsKey[i];

			//아이템 스텟 수정 구조체
			FStatModifier L_FindStatModifier;
			L_FindStatModifier.ModifierType = ItemStatsKey[i];
			L_FindStatModifier.BaseMultiplier = 0.0f;
			L_FindStatModifier.Increment = InItem->ItemStats.Find(L_ModifierStatType)->Value;
			L_FindStatModifier.ModifierID = InItem->ItemStats.Find(L_ModifierStatType)->ModifierID;

			if (AppliedStatModifiers.IsValidIndex(i)) //적용된 스텟 수정 값의 인덱스가 유효하다면
				if (AppliedStatModifiers.Contains(L_FindStatModifier))return; //적용 된 스텟 수정값들에서 찾았다면 함수를 종료합니다

			AddStatModifier(L_FindStatModifier, true);
		}
	}
}

void UCStatsManagerComponent::RemoveItemModifiers(UCRPGItemAsset* InItem)
{
	//아이템이 유효하다면
	if (IsValid(InItem))
	{
		TArray<EStats> ItemStatsKey;
		InItem->ItemStats.GetKeys(ItemStatsKey);

		for (auto StatsKey : ItemStatsKey)
		{
			FStatModifier L_RemoveStatModifer;
			L_RemoveStatModifer.ModifierType = StatsKey;
			L_RemoveStatModifer.Increment = InItem->ItemStats.Find(StatsKey)->Value;
			L_RemoveStatModifer.BaseMultiplier = 0.0f;
			L_RemoveStatModifer.ModifierID = InItem->ItemStats.Find(StatsKey)->ModifierID;
			RemoveStatModifier(L_RemoveStatModifer);
		}
	}
}

void UCStatsManagerComponent::OnSlotDisabledUpdated(EEquipSlots InEquipSlots, FItemSlot InSlotItem, bool IsDisabled)
{
	//비활성화 되어야 한다면
	if (IsDisabled)
		RemoveItemModifiers(InSlotItem.Item);
	else //활성화 되어야 한다면
		AddItemModifiers(InSlotItem.Item);
}

// void UCStatsManagerComponent::TakePhysicalDamage(float InDamage, EImpactDecision InImpactDecision, EStats InDamageResistType,
//                                                  EDamageType InDamageTypeReceived)
// {
// 	RecentlyReceivedDamage = InDamage + RecentlyReceivedDamage; //최근 받은 데미지 설정
// 	RecentlyReceivedHitCount++; // 받은 히트 카운트 증가
// 	const FString ClearFunctionName = TEXT("ResetRecentlyReceivedDamage");
// 	UKismetSystemLibrary::K2_SetTimer(this, ClearFunctionName, 4.0f, false); //ResetRecentlyReceivedDamage 함수를 실행한다
//
//
// 	//데미지 배율 = 데미지 / (데미지 + 갑옷)	//감소된 데미지 = 데미지 * 데미지 승수	//ex) 100 * (100 / 100 + 100	//데미지 / 데미지 +
// 	float L_CalculateDamage = InDamage * (InDamage / (InDamage + GetCurrentValueForStat(InDamageResistType)));
// 	float L_PhysicalDamage = UKismetMathLibrary::FClamp(L_CalculateDamage, 0.0f, L_CalculateDamage);
//
//
// 	//스위치문에서 쓸 변수
// 	float L_StabilityResistance;
// 	float L_StaminaDamage;
//
//
// 	//임팩트 결정타입에 따라
// 	switch (InImpactDecision)
// 	{
// 	case EImpactDecision::NotBlocked:
// 		bDefenseBroken = false;
// 		ModifyCurrentStatValue(EStats::Health, L_PhysicalDamage * -1.0f, true, true);
// 		if (OnDamageReceived.IsBound())
// 			OnDamageReceived.Broadcast(InDamageTypeReceived, L_PhysicalDamage);
// 		break;
//
//
// 	case EImpactDecision::Blocked:
// 	case EImpactDecision::DefenseBroken:
// 		L_StabilityResistance = UCombatLibrary::GetMultiplierForPercentage(
// 			UKismetMathLibrary::FTrunc(GetCurrentStatValue_Clamped(EStats::Stability, 0.0f, 100.0f))) * InDamage;
// 		L_StaminaDamage = InDamage - L_StabilityResistance;
// 		ModifyCurrentStatValue(EStats::Stamina, L_StaminaDamage * -1.0f);
//
// 	//임팩트 유형이 디펜스브로큰이거나 (또는) 현재스테미나가 최소 방어 디펜스스테미나 값보다 이하라면
// 		bDefenseBroken = (InImpactDecision == EImpactDecision::DefenseBroken || GetCurrentValueForStat(EStats::Stamina) <=
// 			MinimumStaminaDefenseBreak);
// 		if (OnDamageReceived.IsBound())
// 			OnDamageReceived.Broadcast(InDamageTypeReceived, L_StaminaDamage);
//
// 	//디펜스브로큰이 false라면 깨지지 않았다면
// 		if (!bDefenseBroken)
// 		{
// 			//블로킹으로 감소된 물리 데미지 계산
// 			L_CalculateDamage = UCombatLibrary::GetMultiplierForPercentage(
// 				UKismetMathLibrary::FTrunc(GetCurrentStatValue_Clamped(EStats::BlockPhysicalDamageReduction, 0.0f, 100.0f))) * L_PhysicalDamage;
// 			L_PhysicalDamage = L_PhysicalDamage - L_CalculateDamage;
// 		}
//
// 		ModifyCurrentStatValue(EStats::Health, L_PhysicalDamage * -1.0f, true, true);
// 		if (OnDamageReceived.IsBound())
// 			OnDamageReceived.Broadcast(InDamageTypeReceived, L_PhysicalDamage);
//
// 		break;
//
//
// 	case EImpactDecision::PerfectBlocked:
// 		L_StabilityResistance = UCombatLibrary::GetMultiplierForPercentage(
// 			UKismetMathLibrary::FTrunc(GetCurrentStatValue_Clamped(EStats::Stability, 0.0f, 100.0f))) * InDamage;
// 		L_StaminaDamage = InDamage - L_StabilityResistance;
// 		L_StaminaDamage *= 0.5f; //퍼팩트 블록은 스테미나 데미지를 반으로 줄여준다
// 		ModifyCurrentStatValue(EStats::Stamina, L_StaminaDamage * -1.0f);
//
//
// 		bDefenseBroken = false; //퍼펙트 방어는 방어가 깨지지 않ㄴ느다
// 		if (OnDamageReceived.IsBound())
// 			OnDamageReceived.Broadcast(InDamageTypeReceived, L_StaminaDamage);
//
// 	//디펜스브로큰이 false라면 깨지지 않았다면
// 		if (!bDefenseBroken)
// 		{
// 			//블로킹으로 감소된 물리 데미지 계산
// 			L_CalculateDamage = UCombatLibrary::GetMultiplierForPercentage(
// 				UKismetMathLibrary::FTrunc(GetCurrentStatValue_Clamped(EStats::BlockPhysicalDamageReduction, 0.0f, 100.0f))) * L_PhysicalDamage;
// 			L_PhysicalDamage = L_PhysicalDamage - L_CalculateDamage;
// 			L_PhysicalDamage *= 0.5f; //퍼펙트 방어는 물리데미지를 반으로 줄인다
// 		}
//
// 		ModifyCurrentStatValue(EStats::Health, L_PhysicalDamage * -1.0f, true, true);
// 		if (OnDamageReceived.IsBound())
// 			OnDamageReceived.Broadcast(InDamageTypeReceived, L_PhysicalDamage);
//
// 		break;
//
//
// 	default: break;
// 	}
// }
//
// float UCStatsManagerComponent::DamageResist(EStats ResistStat, float InDamage)
// {
// 	float L_Resist = UCombatLibrary::GetMultiplierForPercentage(UKismetMathLibrary::FTrunc(GetCurrentStatValue_Clamped(ResistStat, 0.0f, 100.0f))) *
// 		InDamage;
// 	float L_DamageValue = InDamage - L_Resist;
// 	return L_DamageValue;
// }
//
// void UCStatsManagerComponent::ResetRecentlyReceivedDamage()
// {
// 	RecentlyReceivedDamage = 0.0f;
// 	RecentlyReceivedHitCount = 0;
// 	bDefenseBroken = false;
// }
//
// void UCStatsManagerComponent::DamagePoise(float InPoiseDamage)
// {
// 	float LStaggerResistance = GetCurrentStatValue_Clamped(EStats::StaggerResistance, 0.0f, 100.0f);
//
// 	float L_StaggerResistance = UCombatLibrary::GetMultiplierForPercentage(UKismetMathLibrary::FTrunc(LStaggerResistance)); //비틀거림저항력
// 	L_StaggerResistance *= InPoiseDamage;
//
// 	float L_PoiseDamage = InPoiseDamage - L_StaggerResistance;
//
// 	ModifyCurrentStatValue(EStats::Poise, L_PoiseDamage * -1, true, true); //현재 평정 값을 수정한다.
//
// 	if (OnDamageReceived.IsBound())
// 		OnDamageReceived.Broadcast(EDamageType::PoiseDamage, L_PoiseDamage);
// }
//
// bool UCStatsManagerComponent::DamageBalance(float InBalanceDamage)
// {
// 	//반동저항력
// 	float L_RecoilResist = UCombatLibrary::GetMultiplierForPercentage(
// 		UKismetMathLibrary::FTrunc(GetCurrentStatValue_Clamped(EStats::RecoilResist, 0.0f, 100.0f)));
// 	L_RecoilResist *= InBalanceDamage;
// 	float L_BalanceDamage = InBalanceDamage - L_RecoilResist;
//
// 	//현재 균형값을 수정한다.
// 	ModifyCurrentStatValue(EStats::Balance, L_BalanceDamage * -1.0f, true, true);
//
// 	return GetCurrentValueForStat(EStats::Balance) <= 0.0f;
// }
//
// float UCStatsManagerComponent::GetPhysicalAttackDamage(EAttackTypes InAttackType, EStats DamageStat, bool PunisherMode, float StrongChargeAmount)
// {
// 	float RegularDamage = StatsLibrary::RegularDamageByAttackType(GetCurrentValueForStat(DamageStat), InAttackType, PunisherMode);
// 	float LerpDamage = UKismetMathLibrary::Lerp(RegularDamage, RegularDamage * StrongChargeAttackMultiplier, StrongChargeAmount);
// 	return LerpDamage * PhysicalDamageMultiplier;
// }
//
// float UCStatsManagerComponent::GetPoiseDamage(EAttackTypes InAttackType, bool PunisherMode, float StrongChargeAmount)
// {
// 	float RegularDamage = StatsLibrary::RegularDamageByAttackType(GetCurrentValueForStat(EStats::PoiseDamage), InAttackType, PunisherMode);
// 	float LerpDamage = UKismetMathLibrary::Lerp(RegularDamage, RegularDamage * StrongChargeAttackMultiplier, StrongChargeAmount);
// 	return LerpDamage * PoiseDamageMultiplier;
// }
//
void UCStatsManagerComponent::OnDeath()
{
	//체력 타이머 클리어
	ClearRegenTimers(EStats::Health);

	//상태 효과들을 전부 클리어 합니다
	for (const auto AppliedStatusEffect : AppliedStatusEffects)
		ClearStatusEffectTimer(AppliedStatusEffect.StatusEffect);
}

void UCStatsManagerComponent::OnBonfireRested()
{
	TArray<EStats> ResetStats;
	ResetStats.Add(EStats::Health);
	ResetStats.Add(EStats::Stamina);
	ResetStats.Add(EStats::Mana);
	ResetStats.Add(EStats::Poise);
	ResetStats.Add(EStats::Balance);

	//커런트 벨류를 최대값으로 설정
	for (const auto ReStat : ResetStats)
		UpdateCurrentStatValue(ReStat, GetMaxValueForStat(ReStat));


	TArray<EStats> ResetStatResist;
	ResetStatResist.Add(EStats::PoisonResist);
	ResetStatResist.Add(EStats::BurningResist);

	//커런트 벨류를 0.0f로 설정
	for (const auto Resist : ResetStatResist)
		UpdateCurrentStatValue(Resist, 0.0f);


	//상태효과를 클리어한다
	for (int32 i = 0; i < static_cast<int32>(EStatusEffects::MAX); i++)
	{
		const EStatusEffects L_StatusEffect = static_cast<EStatusEffects>(i);
		ClearStatusEffectTimer(L_StatusEffect);
	}
}

void UCStatsManagerComponent::StartStatRegen(EStats StatToRegen)
{
	CHECK(CurrentStatRegenCoolDown.Contains(StatToRegen));
	CHECK(CurrentStatRegenTickInterval.Contains(StatToRegen));
	CHECK(StatRegenFunctions.Contains(StatToRegen));

	const float L_RegenCoolDown = *CurrentStatRegenCoolDown.Find(StatToRegen);

	//타이머 핸들이 존재한다면
	if (StatRegenTimerHandels.Contains(StatToRegen))
	{
		//타이머 핸들이 존재하고 현재 활성화 중이라면
		if (UKismetSystemLibrary::K2_IsTimerActiveHandle(this, *StatRegenTimerHandels.Find(StatToRegen)))
		{
			UKismetSystemLibrary::K2_ClearTimerHandle(this, *StatRegenTimerHandels.Find(StatToRegen));
			FTimerHandle L_RegenHandle;
			FTimerDelegate TimerDel;
			TimerDel.BindUFunction(this, FName("StartStatRegen_Logic"), StatToRegen);
			GetWorld()->GetTimerManager().SetTimer(L_RegenHandle, TimerDel, L_RegenCoolDown, false);
			StatRegenTimerHandels.Add(StatToRegen, L_RegenHandle);
		}
		else //타이머 핸들이 존재하나 현재 활성화 상태가 아니라면
		{
			FTimerHandle L_RegenHandle;
			FTimerDelegate TimerDel;
			TimerDel.BindUFunction(this, FName("StartStatRegen_Logic"), StatToRegen);
			GetWorld()->GetTimerManager().SetTimer(L_RegenHandle, TimerDel, L_RegenCoolDown, false);
			StatRegenTimerHandels.Add(StatToRegen, L_RegenHandle);
		}
	}
	else //타이머 핸들이 존재하지 않으면
	{
		FTimerHandle L_RegenHandle;
		FTimerDelegate TimerDel;
		TimerDel.BindUFunction(this, FName("StartStatRegen_Logic"), StatToRegen);
		GetWorld()->GetTimerManager().SetTimer(L_RegenHandle, TimerDel, L_RegenCoolDown, false);
		StatRegenTimerHandels.Add(StatToRegen, L_RegenHandle);
	}
}

void UCStatsManagerComponent::StartStatRegen_Logic(EStats StatToRegen)
{
	const FString L_RegenFunction = *StatRegenFunctions.Find(StatToRegen);
	const float L_RegenTickInterval = *CurrentStatRegenTickInterval.Find(StatToRegen);
	UKismetSystemLibrary::K2_SetTimer(this, L_RegenFunction, L_RegenTickInterval, true);
}

void UCStatsManagerComponent::StartStatusEffectTimer(EStatusEffects InStatusEffect)
{
	CHECK(StatusEffectRegenFunctions.Contains(InStatusEffect));

	FString L_StatusEffectFunction = *StatusEffectRegenFunctions.Find(InStatusEffect);
	float L_TickInterval = *StatusEffectTickInterval.Find(InStatusEffect);

	//현재 활성화되어있지 않다면
	if (!UKismetSystemLibrary::K2_IsTimerActive(this, L_StatusEffectFunction))
		UKismetSystemLibrary::K2_SetTimer(this, L_StatusEffectFunction, L_TickInterval, true);
}

void UCStatsManagerComponent::StopStatusEffect(EStatusEffects InStatusEffect)
{
	ClearStatusEffectTimer(InStatusEffect);
}
