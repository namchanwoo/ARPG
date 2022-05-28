#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "StatData.generated.h"


/**스텟의 유형*/
UENUM(BlueprintType)
enum class EStats : uint8
{
	/**아무것도 아닌*/
	None UMETA(DisplayName = "None") ,
	/**체력*/
	Health UMETA(DisplayName = "Health"),
	/**스테미나*/
	Stamina UMETA(DisplayName = "Stamina"),
	/**마나*/
	Mana UMETA(DisplayName = "Mana"),


	/**평정*/
	Poise UMETA(DisplayName = "Poise"),
	/**균형*/
	Balance UMETA(DisplayName = "Balance"),
	/**무게*/
	Weight UMETA(DisplayName = "Weight"),
	/**이동 스피드*/
	MovementSpeed UMETA(DisplayName = "Movement Speed"),
	/**안정*/
	Stability UMETA(DisplayName = "Stability"),


	/**물리적 데미지*/
	PhysicalDamage UMETA(DisplayName = "Physical Damage"),
	/**평정 데미지*/
	PoiseDamage UMETA(DisplayName = "Poise Damage"),
	/**균형 데미지*/
	BalanceDamage UMETA(DisplayName = "Balance Damage"),
	/**치명타 데미지*/
	CriticalDamage UMETA(DisplayName = "Critical Damage"),
	/**화염 데미지*/
	FireDamage UMETA(DisplayName = "Fire Damage"),
	/**출혈 데미지*/
	BleedingDamage UMETA(DisplayName = "Bleeding Damage"),
	/**독 데미지*/
	PoisonDamage UMETA(DisplayName = "Poison Damage"),


	/**물리피해 저항*/
	PhysicalDamageResist UMETA(DisplayName = "Physical Damage Resist"),
	/**독 저항*/
	PoisonResist UMETA(DisplayName = "Poison Resist"),
	/**화염 저항*/
	BurningResist UMETA(DisplayName = "Burning Resist"),
	/**발사체 저항*/
	ProjectileResistance UMETA(DisplayName = "Projectile Resistance"),
	/**비틀거림 저항*/
	StaggerResistance UMETA(DisplayName = "Stagger Resistance"),
	/**반동 저항*/
	RecoilResist UMETA(DisplayName = "Recoil Resist"),
	/**출혈 저항*/
	BleedingResist UMETA(DisplayName = "Bleeding Resist"),


	/**근전 공격 스텟 비용*/
	MeleeAttackStatCost UMETA(DisplayName = "Melee Attack Stat Cost"),
	/**마나 코스트*/
	ManaCost UMETA(DisplayName = "Mana Cost"),
	/**회피 스텟 비용*/
	DodgeStatCost UMETA(DisplayName = "Dodge Stat Cost"),


	/**마지막 인덱스*/
	MAX UMETA(DisplayName = "MAX"),
};


/**스텟의 값 유형*/
UENUM(BlueprintType)
enum class EStatValueType : uint8
{
	/**기본 값*/
	BaseValue UMETA(DisplayName = "Base Value") ,
	/**최대 값*/
	MaxValue UMETA(DisplayName = "Max Value"),
	/**현재 값*/
	CurrentValue UMETA(DisplayName = "Current Value"),
	/**수정(변경된) 값*/
	ModifierValue UMETA(DisplayName = "Modifier Value"),
	/**증가 값*/
	IncrementValue UMETA(DisplayName = "Increment Value"),
	/**배율값*/
	MultiplierValue UMETA(DisplayName = "Multiplier Value"),
};

/**스텟의 값 유형 단순화*/
UENUM(BlueprintType)
enum class EStatValueTypeSimplified : uint8
{
	/**아무것도 아닌*/
	None UMETA(DisplayName = "None") ,
	/**기본 값*/
	BaseValue UMETA(DisplayName = "Base Value") ,
	/**현재 값*/
	CurrentValue UMETA(DisplayName = "Current Value")
};

/**속성 유형*/
UENUM(BlueprintType)
enum class EAttributes : uint8
{
	/**아무것도 아닌*/
	None UMETA(DisplayName = "None") ,
	/**건강*/
	Vitality UMETA(DisplayName = "Vitality"),
	/**지능*/
	Intelligence UMETA(DisplayName = "Intelligence"),
	/**지구력*/
	Endurance UMETA(DisplayName = "Endurance"),
	/**힘*/
	Strength UMETA(DisplayName = "Strength"),
	/**손재주*/
	Dexterity UMETA(DisplayName = "Dexterity"),
	/**운*/
	Luck UMETA(DisplayName = "Luck"),
	/**소울 레벨*/
	SoulLevel UMETA(DisplayName = "SoulLevel"),
	/**마지막 인덱스*/
	MAX UMETA(DisplayName = "MAX"),
};

/**상태 효과 유형*/
UENUM(BlueprintType)
enum class EStatusEffects : uint8
{
	/**아무것도 아닌*/
	None UMETA(DisplayName = "None") ,
	/**불타고 있는*/
	Burning UMETA(DisplayName = "Burning"),
	/**독 상태*/
	Poison UMETA(DisplayName = "Poison"),
	/**치유 상태*/
	Healing UMETA(DisplayName = "Healing"),
	/**마지막 인덱스*/
	MAX UMETA(DisplayName = "MAX"),
};


/**상태 효과 구조체*/
USTRUCT(BlueprintType)
struct FStatusEffect
{
	GENERATED_BODY()
public:
	/**상태효과 유형*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	EStatusEffects StatusEffect;
	/**상태효과증가량*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float StatusEffectIncrement;
	/**타이머 틱 간격*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float TimerTickInterval;
	/**효과 지속시간*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float EffectDuration;

public:
	FStatusEffect(): StatusEffect(), StatusEffectIncrement(0), TimerTickInterval(0.1f), EffectDuration(5.0)
	{
	}

	bool operator==(const FStatusEffect& Other) const
	{
		return StatusEffect == Other.StatusEffect;
	}
};


/**스텟 수정 구조체*/
USTRUCT(BlueprintType)
struct FStatModifier
{
	GENERATED_BODY()
public:
	/**수정할 스텟 유형*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	EStats ModifierType;
	/**증가량*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float Increment;
	/**기본 배율*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float BaseMultiplier;
	/**수정 ID*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FGuid ModifierID;

public:
	bool operator==(const FStatModifier& Other) const
	{
		return ModifierType == Other.ModifierType && ModifierID == Other.ModifierID;
	}
};


/**사용자 에디터 스텟 구조체*/
USTRUCT(BlueprintType)
struct FUserEditableStats
{
	GENERATED_BODY()
public:
	/**기본 값*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float BaseValue;
	/**최대 값*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float MaxValue;

	FUserEditableStats(): BaseValue(0), MaxValue(100.0f)
	{
	}

	FUserEditableStats(float InBaseValue, float InMaxValue): BaseValue(InBaseValue), MaxValue(InMaxValue)
	{
	}
};

/**플롯 스텟 구조체*/
USTRUCT(BlueprintType)
struct FFloatStatValue
{
	GENERATED_BODY()
public:
	/**기본 값*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float BaseValue;
	/**현재 값*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float CurrentValue;
};


/**속성 유형의 스텟 관련 구조체*/
USTRUCT(BlueprintType)
struct FAssociatedStats
{
	GENERATED_BODY()
public:
	/**속성 유형*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	EAttributes Attribute;
	/**수정 할 스텟*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TMap<EStats, UCurveFloat*> StatsToModify;
};


/**속성 구조체*/
USTRUCT(BlueprintType)
struct FFloatAttribute
{
	GENERATED_BODY()
public:
	/**기본 값*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float BaseValue;

	/**최대 값*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float MaxValue;

	/**현재 값*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float CurrentValue;

	/**증가 값*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float IncrementValue;

	/**배율 값*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float MultiplierValue;

	FFloatAttribute(): BaseValue(1.0f), MaxValue(100.0f), CurrentValue(0), IncrementValue(0), MultiplierValue(0)
	{
	}
};


/**스텟 타이머 구조체*/
USTRUCT(BlueprintType)
struct FStatTimer
{
	GENERATED_BODY()
public:
	/**스텟 재생 쿨 다운*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TMap<EStats, float> StatRegenCoolDown;

	/**스텟 재생 값*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TMap<EStats, float> StatRegenRate;

	/**스텟 타이머 틴 간격*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TMap<EStats, float> StatTimerTickInterval;

public:
	FStatTimer()
	{
		StatRegenCoolDown.Add(EStats::Health, 1.5f);
		StatRegenCoolDown.Add(EStats::Stamina, 1.5f);
		StatRegenCoolDown.Add(EStats::Mana, 1.5f);
		StatRegenCoolDown.Add(EStats::Poise, 2.0f);
		StatRegenCoolDown.Add(EStats::Balance, 2.0f);

		StatRegenRate.Add(EStats::Health, 5.0f);
		StatRegenRate.Add(EStats::Stamina, 5.5f);
		StatRegenRate.Add(EStats::Mana, 5.0f);
		StatRegenRate.Add(EStats::Poise, 3.0f);
		StatRegenRate.Add(EStats::Balance, 3.0f);

		StatRegenRate.Add(EStats::Health, 0.1f);
		StatRegenRate.Add(EStats::Stamina, 0.1f);
		StatRegenRate.Add(EStats::Mana, 0.1f);
		StatRegenRate.Add(EStats::Poise, 0.1f);
		StatRegenRate.Add(EStats::Balance, 0.1f);
	}
};
