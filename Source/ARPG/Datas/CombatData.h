#pragma once

#include "CoreMinimal.h"
#include "CharacterData.h"
#include "Engine/DataTable.h"
#include "CombatData.generated.h"


class UCBaseHitReaction;
/**전투 유형*/
UENUM(BlueprintType)
enum class ECombatType : uint8
{
	//None
	None UMETA(DisplayName = "None") ,
	/**맨손*/
	Unarmed UMETA(DisplayName = "Unarmed") ,
	/**한손검*/
	LightSword UMETA(DisplayName = "LightSword") ,
	/**대검*/
	GreatSword UMETA(DisplayName = "Greatsword") ,
	/**검과 방패*/
	LightSwordAndShield UMETA(DisplayName = "LightSwordAndShield") ,
	/**도끼*/
	Axe UMETA(DisplayName = "Axe") ,
	/**활*/
	Bow UMETA(DisplayName = "Bow") ,
	/**창*/
	Spear UMETA(DisplayName = "Spear") ,
	/**카타나*/
	Katana UMETA(DisplayName = "Katana") ,
	/**쌍검*/
	DualSwords UMETA(DisplayName = "DualSwords") ,
	/**트윈 블레이드(상하 검날로 된)*/
	Twinblades UMETA(DisplayName = "Twinblades") ,
	/**맨손과 방패*/
	UnarmedCombatAndShield UMETA(DisplayName = "UnarmedCombatAndShield") ,
	/**도끼와 검*/
	AxeAndShield UMETA(DisplayName = "AxeAndShield") ,
	/**크로스보우*/
	Crossbow UMETA(DisplayName = "Crossbow")
};


/**데미지 유형*/
UENUM(BlueprintType)
enum class EDamageType : uint8
{
	//아무것도 아닌
	None UMETA(DisplayName = "None") ,
	//물리 피해
	PhysicalDamage UMETA(DisplayName = "Physical Damage"),
	//평정 피해
	PoiseDamage UMETA(DisplayName = "Poise Damage"),
	//독 피해
	PoisonDamage UMETA(DisplayName = "Poison Damage"),
	//불타는 피해
	BurningDamage UMETA(DisplayName = "Burning Damage"),
	//발사체 피해
	ProjectileDamage UMETA(DisplayName = "Projectile Damage"),
	//균형 피해
	MAX UMETA(DisplayName = "Balance Damage")
};


/**무게 유형*/
UENUM(BlueprintType)
enum class EEncumbrance : uint8
{
	//가벼운
	Light UMETA(DisplayName = "Light") ,
	//중간
	Medium UMETA(DisplayName = "Medium"),
	//무거운
	Heavy UMETA(DisplayName = "Heavy"),
};


/**공격 유형*/
UENUM(BlueprintType)
enum class EAttackTypes : uint8
{
	/**아무것도 아닌*/
	None UMETA(DisplayName = "None") ,
	/**약한 공격*/
	LightAttack UMETA(DisplayName = "Light Attack") ,
	/**강한 공격*/
	StrongAttack UMETA(DisplayName = "StrongAttack") ,
	/**대쉬 공격*/
	DashAttack UMETA(DisplayName = "Dash Attack") ,
	/**약한 차지 공격*/
	LightChargeAttack UMETA(DisplayName = "Light Charge Attack") ,
	/**무기 기술 공격*/
	WeaponArtAttack UMETA(DisplayName = "Weapon Art Attack") ,
	/**공중 공격*/
	AirAttack UMETA(DisplayName = "Air Attack") ,
	/**치명타 처형*/
	FatalityExecution UMETA(DisplayName = "Fatality Execution") ,
	/**암살 공격*/
	Assassination UMETA(DisplayName = "Assassination") ,
	/**피니셔 모드 공격 진입*/
	EnterFinisherModeAttack UMETA(DisplayName = "Enter Finisher  Mode Attack") ,
	/**피니셔 모드 공격 종료*/
	ExitFinisherModeAttack UMETA(DisplayName = "Exit Finisher Mode Attack") ,
	/**활 발사체 공격*/
	ShootArcheryProjectile UMETA(DisplayName = "Shoot Archery Projectile") ,
	/**연쇄 공격*/
	ChainAttack UMETA(DisplayName = "Chain Attack") ,
	/**반격*/
	Riposte UMETA(DisplayName = "Riposte"),
	/**스텝 회피 공격*/
	StepDodgeAttack UMETA(DisplayName = "Step Dodge Attack"),
	/**구르기 회피 공격*/
	RollDodgeAttack UMETA(DisplayName = "Roll Dodge Attack"),
	/**Max*/
	MAX UMETA(DisplayName = "MAX")
};


/**임팩트 결정 유형*/
UENUM(BlueprintType)
enum class EImpactDecision : uint8
{
	/**아무것도 아닌*/
	None UMETA(DisplayName = "None") ,
	/**블록하지 않음*/
	NotBlocked UMETA(DisplayName = "Not Blocked") ,
	/**블록한*/
	Blocked UMETA(DisplayName = "Blocked") ,
	/**완벽 차단*/
	PerfectBlocked UMETA(DisplayName = "PerfectBlocked") ,
	/**패리한*/
	Parried UMETA(DisplayName = "Parried") ,
	/**방어가 부서진 */
	DefenseBroken UMETA(DisplayName = "DefenseBroken")
};


/**구르기 유형 */
UENUM(BlueprintType)
enum class EDodgeType : uint8
{
	/**구르기 회피*/
	RollDodge UMETA(DisplayName = "Roll Dodge") ,
	/**스텝 회피*/
	StepDodge UMETA(DisplayName = "Step Dodge") ,
	/**구르기와 스텝 둘다 가능*/
	Both UMETA(DisplayName = "Both") ,
};


/**구르기 방향 */
UENUM(BlueprintType)
enum class EDodgeDirection : uint8
{
	/**None*/
	None UMETA(DisplayName = "None") ,
	/**정면*/
	Forwards UMETA(DisplayName = "Forwards") ,
	/**우측*/
	Right UMETA(DisplayName = "Right") ,
	/**좌측*/
	Left UMETA(DisplayName = "Left") ,
	/**뒤로*/
	Backwards UMETA(DisplayName = "Backwards") ,
	/**왼쪽 뒤*/
	BackLeft UMETA(DisplayName = "Back Left") ,
	/**오른쪽 뒤*/
	BackRight UMETA(DisplayName = "Back Right") ,
	/**정면 왼쪽*/
	FrontLeft UMETA(DisplayName = "Front Left") ,
	/**정면 오른쪽*/
	FrontRight UMETA(DisplayName = "Front Right") ,
	/**뒷걸음*/
	StepBack UMETA(DisplayName = "StepBack")
};


/**공격의 강도*/
UENUM(BlueprintType)
enum class EAttackPower : uint8
{
	None UMETA(DisplayName = "None"),
	Light UMETA(DisplayName = "Light"),
	Medium UMETA(DisplayName = "Medium"),
	Heavy UMETA(DisplayName = "Heavy"),
	VeryHeavy UMETA(DisplayName = "Very Heavy")
};


/**임팩트 이미터 스폰 유형*/
UENUM(BlueprintType)
enum class EImpactEmitterSpawnType : uint8
{
	/**위치에서*/
	AtLocation UMETA(DisplayName = "At Location"),
	/**붙인다*/
	Attached UMETA(DisplayName = "Attached")
};


/**버퍼 키 유형*/
UENUM(BlueprintType)
enum class EBufferKey : uint8
{
	/**아무것도 아닌*/
	None UMETA(DisplayName = "None") ,
	/**공격*/
	Attack UMETA(DisplayName = "Attack") ,
	/**스텝 회피*/
	StepDodge UMETA(DisplayName = "StepDodge") ,
	/**구르기 회피*/
	RollDodge UMETA(DisplayName = "RollDodge") ,
	/**방어*/
	Block UMETA(DisplayName = "Block") ,
	/**점프*/
	Jump UMETA(DisplayName = "Jump") ,
	/**패리*/
	Parry UMETA(DisplayName = "Parry") ,
	/**도구사용*/
	UseTool UMETA(DisplayName = "Use Tool") ,
	/**상호작용*/
	Interact UMETA(DisplayName = "Interact") ,
	/**포커스모드 토글(전환)*/
	ToggleFocusMode UMETA(DisplayName = "ToggleFocusMode") ,
	/**웅크리기*/
	Crouch UMETA(DisplayName = "Crouch") ,
	/**피니시모드(토글)*/
	ToggleFinisherMode UMETA(DisplayName = "Toggle Finisher Mode") ,
	/**차지공격*/
	ChargeAttack UMETA(DisplayName = "Charge Attack") ,
	/**전투모드(토글)*/
	ToggleCombatMode UMETA(DisplayName = "ToggleCombatMode") ,
	/**일어서다*/
	GetUpRoll UMETA(DisplayName = "GetUpRoll") ,
	/**활성화된 아이템 스위칭*/
	SwitchActiveItem UMETA(DisplayName = "Switch Active Item") ,
	/**확대(줌)*/
	Zoom UMETA(DisplayName = "Zoom") ,
	/**조준*/
	Aim UMETA(DisplayName = "Aim") ,
	/**마지막*/
	MAX UMETA(DisplayName = "MAX")
};

/**선택적 전투 능력 유형 */
UENUM(BlueprintType)
enum class EOptionalCombatAbilities : uint8
{
	/***/
	None UMETA(DisplayName = "None") ,
	/**집중(초점) 상태*/
	FocusState UMETA(DisplayName = "Focus State") ,
	/**피니셔 모드*/
	FinisherMode UMETA(DisplayName = "Finisher Mode") ,
	/**패리*/
	Parry UMETA(DisplayName = "Parry") ,
	/**완벽방어*/
	PerfectBlock UMETA(DisplayName = "Perfect Block") ,
	/**회피공격*/
	DodgeAttack UMETA(DisplayName = "Dodge Attack") ,
	/**연쇄공격*/
	ChainAttack UMETA(DisplayName = "Chain Attack")
};

/**충돌 추적 대상 유형*/
UENUM(BlueprintType)
enum class ECollisionTraceTarget : uint8
{
	/***/
	None UMETA(DisplayName = "None") ,
	/**오른손무기*/
	RightWeapon UMETA(DisplayName = "Right Weapon") ,
	/**왼손혹은(추가무기)*/
	Off_HandWeapon UMETA(DisplayName = "Off-Hand Weapon") ,
	/**방패*/
	Shield UMETA(DisplayName = "Shield") ,
	/**오른손*/
	RightHand UMETA(DisplayName = "Right Hand") ,
	/**왼손*/
	LeftHand UMETA(DisplayName = "Left Hand") ,
	/**오른 발*/
	RightFoot UMETA(DisplayName = "Right Foot") ,
	/**왼 발*/
	LeftFoot UMETA(DisplayName = "Left Foot")
};

/**근접 공격 방향*/
UENUM(BlueprintType)
enum class EMeleeSwingDirection : uint8
{
	/*아무것도 아닌*/
	None UMETA(DisplayName = "None"),
	/*기본*/
	Default UMETA(DisplayName = "Default"),
	/*수평왼쪽*/
	HorizontalLeft UMETA(DisplayName = "Horizonta lLeft"),
	/*수평오른쪽*/
	HorizontalRight UMETA(DisplayName = "Horizontal Right"),
	/*수직위*/
	VerticalDown UMETA(DisplayName = "Vertical Down"),
	/*수직아래*/
	VerticalUp UMETA(DisplayName = "Vertical Up"),
	/*전방*/
	ForwardThrust UMETA(DisplayName = "Forward Thrust")
};


/**사용자 지정 공격 유형*/
UENUM(BlueprintType)
enum class ECustomAttackTypes : uint8
{
	/*아무것도 아닌*/
	None UMETA(DisplayName = "None"),
	/*슬램*/
	Slam UMETA(DisplayName = "Slam"),
	/*녹 업*/
	KnockUp UMETA(DisplayName = "Knockup"),
	/*발사체 뼈 타격 반응*/
	ProjectileBoneHitReaction UMETA(DisplayName = "Projectile BoneHit Reaction"),
	/*디펜스 브레이크 어택*/
	DefenseBreakAttack UMETA(DisplayName = "Defense Break Attack"),
	/*넉다운 어택*/
	KnockdownAttack UMETA(DisplayName = "Knockdown Attack"),
	/*잡아서 공격*/
	GrabAttack UMETA(DisplayName = "Grab Attack"),
	/*스탠드 오프 공격*/
	StandoffAttack UMETA(DisplayName = "Standoff Attack")
};


/**공격 카테고리*/
UENUM(BlueprintType)
enum class EAttackCategory : uint8
{
	/**근접*/
	Melee UMETA(DisplayName = "Melee"),
	/**원거리*/
	Ranged UMETA(DisplayName = "Ranged"),
	/*마법*/
	Magic UMETA(DisplayName = "Magic")
};


/**히트 반응 유형*/
UENUM(BlueprintType)
enum class EHitReactionType : uint8
{
	/**아무것도 아닌*/
	None UMETA(DisplayName = "None"),
	/**히트스턴*/
	HitStun UMETA(DisplayName = "HitStun"),
	/**페리를 했다*/
	Parried UMETA(DisplayName = "Parried"),
	/**반동반응*/
	RecoilReaction UMETA(DisplayName = "RecoilReaction"),
	/**완벽한 차단 반동 반응*/
	PerfectBlockedRecoilReaction UMETA(DisplayName = "PerfectBlockedRecoilReaction"),
	/**카운터를 한*/
	Countered UMETA(DisplayName = "Countered"),
	/**히트본*/
	HitBone UMETA(DisplayName = "HitBone"),
	/**기절*/
	Stunned UMETA(DisplayName = "Stunned"),
	/**처형*/
	Executed UMETA(DisplayName = "Executed"),
	/**암살*/
	Assassinated UMETA(DisplayName = "Assassinated"),
	/**차단반응*/
	BlockReaction UMETA(DisplayName = "BlockReaction"),
	/**완벽한 차단반응*/
	PerfectBlockReaction UMETA(DisplayName = "PerfectBlockReaction"),
	/**디펜스 브레이크*/
	DefenseBreak UMETA(DisplayName = "DefenseBreak"),
	/**죽음*/
	Death UMETA(DisplayName = "Death"),
	/**반격반응*/
	RiposteReaction UMETA(DisplayName = "RiposteReaction"),
	/**벡스텝*/
	BackStab UMETA(DisplayName = "BackStab"),
	/**IK 적중 반응*/
	IKHitReaction UMETA(DisplayName = "IKHitReaction")
};


/**히트 반응 인덱스 유형*/
UENUM(BlueprintType)
enum class EHitReactIndexType : uint8
{
	//아무것도 아닌
	None UMETA(DisplayName = "None") ,
	//사용자 정의 인덱스
	CustomIndex UMETA(DisplayName = "Custom Index"),
	//랜덤 인덱스
	RandomIndex UMETA(DisplayName = "Random Index"),
	//공격유형 인덱스
	AttackPowerIndex UMETA(DisplayName = "Attack Power Index"),
	//물리 무게 인덱스
	PoiseWeightIndex UMETA(DisplayName = "Poise Weight Index"),
};


/**마주치는 타격 방향*/
UENUM(BlueprintType)
enum class EHitFacingDirection : uint8
{
	/***/
	None UMETA(DisplayName = "None"),
	/**정면*/
	Front UMETA(DisplayName = "Front"),
	/**뒤*/
	Back UMETA(DisplayName = "Back"),
	/**오른쪽*/
	Right UMETA(DisplayName = "Right"),
	/**왼쪽*/
	Left UMETA(DisplayName = "Left")
};

/**히트 반응 방향*/
UENUM(BlueprintType)
enum class EHitReactionDirection : uint8
{
	/**아무것도 아닌*/
	None UMETA(DisplayName = "None"),
	/**정면*/
	Front UMETA(DisplayName = "Front"),
	/**뒤*/
	Back UMETA(DisplayName = "Back"),
	/**오른쪽*/
	Right UMETA(DisplayName = "Right"),
	/**왼쪽*/
	Left UMETA(DisplayName = "Left"),
	/**오른쪽 위*/
	RightUp UMETA(DisplayName = "Right Up"),
	/**오른쪽 아래*/
	RightDown UMETA(DisplayName = "Right Down"),
	/**왼쪽 위*/
	LeftUp UMETA(DisplayName = "Left Up"),
	/**왼쪽 아래*/
	LeftDown UMETA(DisplayName = "Left Down")
};

/**히트 반응 카테고리*/
UENUM(BlueprintType)
enum class EHitReactionCategory : uint8
{
	/**아무것도 아닌*/
	None UMETA(DisplayName = "None"),
	/**일반 히트반응*/
	GeneralHitReaction UMETA(DisplayName = "GeneralHitReaction"),
	/**치명타 적중 반응*/
	FatalityHitReaction UMETA(DisplayName = "FatalityHitReaction"),
	/**히트 본 반응*/
	HitBoneReaction UMETA(DisplayName = "HitBoneReaction"),
	/**IK 히트 본 반응*/
	IKHitReaction UMETA(DisplayName = "IKHitReaction")
};


/**몸 타격 위치*/
UENUM(BlueprintType)
enum class EBodyHitLocations : uint8
{
	/**아무것도 아닌*/
	None UMETA(DisplayName = "None"),
	/**머리*/
	Head UMETA(DisplayName = "Head"),
	/**오른 팔*/
	RightArm UMETA(DisplayName = "RightArm"),
	/**왼 팔*/
	LeftArm UMETA(DisplayName = "LeftArm"),
	/**몸통*/
	Torso UMETA(DisplayName = "Torso"),
	/**오른쪽 다리*/
	RightLeg UMETA(DisplayName = "RightLeg"),
	/**왼쪽 다리*/
	LeftLeg UMETA(DisplayName = "LeftLeg")
};


/**몸 타격 위치*/
UENUM(BlueprintType)
enum class EProceduralHitReactAlphas : uint8
{
	None UMETA(DisplayName = "None"),
	/**팔*/
	Arm UMETA(DisplayName = "Arm"),
	/**척추*/
	Spine UMETA(DisplayName = "Spine"),
	/**목*/
	Neck UMETA(DisplayName = "Neck"),
	/**손*/
	Hand UMETA(DisplayName = "Hand"),
	/**팔*/
	ArmSet UMETA(DisplayName = "ArmSet"),
	/**척추*/
	SpineSet UMETA(DisplayName = "SpineSet"),
	/**오른쪽 다리*/
	RightLeg UMETA(DisplayName = "RightLeg"),
	/**왼쪽 다리*/
	LeftLeg UMETA(DisplayName = "LeftLeg")
};


/**임패트 이미터*/
USTRUCT(BlueprintType)
struct FImpactEmitter
{
	GENERATED_BODY()
public:
	//파티클
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UParticleSystem* ImpactEmitter;
	//회전
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FRotator Rotation;
	//크기
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector Scale;

	FImpactEmitter(): ImpactEmitter(nullptr)
	{
		Scale = FVector::OneVector;
	}
};


/**임팩트 효과*/
USTRUCT(BlueprintType)
struct FImpactEffects
{
	GENERATED_BODY()
public:
	/**시각효과*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FImpactEmitter> VisualEffects;
	/**혼합반응 이미터*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FImpactEmitter> MixedReactionEmitter;
};


/**임팩트 사운드*/
USTRUCT(BlueprintType)
struct FImpactSound
{
	GENERATED_BODY()
public:
	/**임팩트 사운드*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USoundBase* ImpactSound;
};


/**닷지방향맵*/
USTRUCT(BlueprintType)
struct FDodgeDirectionMap
{
	GENERATED_BODY()
public:
	/**닷지 유형*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	EDodgeType DodgeType;
	/**구르기 회피의 방향에 매칭 된 몽테이지*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TMap<EDodgeDirection, FActionMontage> RollDodgeMontages;
	/**스텝 회피의 방향에 매칭 된 몽테이지*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TMap<EDodgeDirection, FActionMontage> StepDodgeMontages;
};


/**피니셔모드 몽테이지 구조체*/
USTRUCT(BlueprintType)
struct FFnisherModeMontage
{
	GENERATED_BODY()
public:
	/**일반 액션에 따른 액션몽테이지*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TMap<ECharacterAction, FActionMontage> GeneralActions;
	/**공격 유형에 따른 액션몽테이지*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TMap<EAttackTypes, FActionMontage> Attacks;
};


/**히트 반응 클래스(레퍼런스)*/
USTRUCT(BlueprintType)
struct FHitReactClass
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSubclassOf<UCBaseHitReaction> HitReactionClass;
};

/**방향 적중 반응 몽타주 맵*/
USTRUCT(BlueprintType)
struct FCustomHitReaction
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TMap<ECustomAttackTypes, FHitReactClass> CustomHitReaction;
};

/** 히트 방향에 따른 액션 몽테이지 맵*/
USTRUCT(BlueprintType)
struct FDirectionalHitReactMontageMap
{
	GENERATED_BODY()
public:
	/**방향에 따른 몽테이지*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TMap<EHitReactionDirection, FActionMontage> DirectionalMontage;
};

/**컴벳액션에 필요한 몽테이지 구조체*/
USTRUCT(BlueprintType)
struct FCombatMontages : public FTableRowBase
{
	GENERATED_BODY()
public:
	/**선택적전투능력*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TMap<EOptionalCombatAbilities, bool> OptionalCombatAbilities;
	/**일반 액션*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TMap<ECharacterAction, FActionMontage> GeneralActions;
	/**공격 애니메이션*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TMap<EAttackTypes, FActionMontage> Attacks;
	/**카운터 애니메이션*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TMap<EHitFacingDirection, FAnimMontageStruct> CounterAttacks;
	/**닷지방향 애니메이션*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FDodgeDirectionMap DodgeDirection;
	/**피니시모드 애니메이션*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FFnisherModeMontage FinisherModeMontages;
	/**히트 반응 */
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TMap<EHitReactionType, FHitReactClass> HitReactions;
	/**커스텀 히트 반응*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TMap<EImpactDecision, FCustomHitReaction> CustomHitReactions;
};


/**컴벳 히트 관련 구조체*/
USTRUCT(BlueprintType)
struct FCombatHitData
{
	GENERATED_BODY()
public:
	/**데미지를 받은 액터*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	AActor* DamagedActor;
	/**데미지를 주는 원인*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	AActor* DamageCauser;
	/**히트의 Yaw각도*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float HitYawAngle;
	/**히트 거리*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float HitDistance;
	/**방어가 가능 한지*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	bool bCanBeBlocked;
	/**패리가 가능 한지*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	bool bCanBeParried;
	/**임팩트 위치*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FVector ImpactLocation;
	/**임팩트 방향*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FVector ImpactDirection;
	/**히트 된 본 이름*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FName HitBoneName;
	/**마주치는 타격방향*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	EHitFacingDirection HitDirection;
	/**임팩트 결과 유형*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	EImpactDecision ImpactDecision;

public:
	FCombatHitData(): DamagedActor(nullptr), DamageCauser(nullptr), HitYawAngle(0), HitDistance(0), bCanBeBlocked(false),
	                  bCanBeParried(false), HitDirection(), ImpactDecision()
	{
	}

	FCombatHitData(AActor* InDamagedActor, AActor* InDamageCauser, float InHitYawAngle, float InHitDistance):
		bCanBeBlocked(false), bCanBeParried(false), HitDirection(), ImpactDecision()
	{
		DamagedActor = InDamagedActor;
		DamageCauser = InDamageCauser;
		HitYawAngle = InHitYawAngle;
		HitDistance = InHitDistance;
	}
};
