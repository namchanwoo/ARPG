#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "CharacterData.generated.h"


/**보행 유형*/
UENUM(BlueprintType)
enum class EGait : uint8
{
	//아무것도 아닌
	None UMETA(DisplayName = "None") ,
	//걷기
	Walk UMETA(DisplayName = "Walk"),
	//달리기
	Run UMETA(DisplayName = "Run"),
	//질주
	Sprint UMETA(DisplayName = "Sprint"),
	//방어중인
	Blocking UMETA(DisplayName = "Blocking"),
};


/**캐릭터 행동 유형*/
UENUM(BlueprintType)
enum class ECharacterAction : uint8
{
	/**아무것도 아닌*/
	None UMETA(DisplayName = "None") ,
	/**공격*/
	Attack UMETA(DisplayName = "Attack") ,
	/**구르기 회피*/
	RollDodge UMETA(DisplayName = "Roll Dodge") ,
	/**스텝 회피*/
	StepDodge UMETA(DisplayName = "Step Dodge") ,
	/**상호작용*/
	Interacting UMETA(DisplayName = "Interacting") ,
	/**패리(받아넘기기)*/
	Parry UMETA(DisplayName = "Parry") ,
	/**패리한*/
	Parried UMETA(DisplayName = "Parried") ,
	/**도구 사용*/
	UseTool UMETA(DisplayName = "Use Tool") ,
	/**히트 리액션*/
	HitReaction UMETA(DisplayName = "Hit Reaction") ,
	/**초점 상태 전환*/
	ToggleFocusState UMETA(DisplayName = "Toggle Focus State") ,
	/**전투 진입*/
	EnterCombat UMETA(DisplayName = "Enter Combat") ,
	/**전투 종료*/
	ExitCombat UMETA(DisplayName = "Exit Combat") ,
	/**피니셔 모드 진입*/
	EnterFinisherMode UMETA(DisplayName = "Enter Finisher Mode") ,
	/**피니셔 모드 종료*/
	ExitFinisherMode UMETA(DisplayName = "Exit Finisher Mode") ,
	/**앉다(휴식)*/
	SitDown_Bonfire UMETA(DisplayName = "Sit down (Bonfire)") ,
	/**일어서다(휴식)*/
	StandUp_Bonfire UMETA(DisplayName = "Standup (Bonfire)") ,
	/**웨폰 기술 충전*/
	WeaponArtCharge UMETA(DisplayName = "Weapon Art Charge") ,
	/**확대 시작*/
	StartZooming UMETA(DisplayName = "StartZooming") ,
	/**확대 중지*/
	StopZooming UMETA(DisplayName = "StopZooming") ,
	/**활을 장전*/
	ReloadBow UMETA(DisplayName = "Reload Bow") ,
	/**조준*/
	Aim UMETA(DisplayName = "Aim") ,
	/**원거리 공격*/
	RangedAttack UMETA(DisplayName = "Ranged Attack") ,
	/***/
	GetupFacingUp UMETA(DisplayName = "Getup Facing Up") ,
	/***/
	GetupFacingDown UMETA(DisplayName = "Getup Facing Down") ,
	/***/
	Counter UMETA(DisplayName = "Counter") ,
	/**일반액션*/
	GeneralAction UMETA(DisplayName = "GeneralAction") ,
	/**장비 장착*/
	Equip UMETA(DisplayName = "Equip") ,
	/**장비 해제*/
	UnEquip UMETA(DisplayName = "UnEquip") ,
	/**방패 변경*/
	SwitchShield UMETA(DisplayName = "Switch Shield") ,
	/**화살 변경*/
	SwitchArrow UMETA(DisplayName = "Switch Arrow") ,
	/**도구 변경*/
	SwitchTool UMETA(DisplayName = "Switch Tool"),
	/**MAX*/
	MAX UMETA(DisplayName = "MAX"),
};


/**캐릭터 상태 유형*/
UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	/**대기*/
	Idle UMETA(DisplayName = "Idle") ,
	/**공격 중*/
	Attacking UMETA(DisplayName = "Attacking") ,
	/**방어 중*/
	Blocking UMETA(DisplayName = "Blocking") ,
	/**집중 상태*/
	FocusedState UMETA(DisplayName = "FocusedState") ,
	/**히트 후 잠깐 기절*/
	HitStunned UMETA(DisplayName = "Hit Stunned") ,
	/**기절*/
	Stunned UMETA(DisplayName = "Stunned") ,
	/**상호작용 중*/
	Interacting UMETA(DisplayName = "Interacting") ,
	/**회피중*/
	Dodging UMETA(DisplayName = "Dodging") ,
	/**스텝 회피중*/
	StepDodging UMETA(DisplayName = "Step Dodging") ,
	/**눕혀지다*/
	KnockDown UMETA(DisplayName = "Knock Down") ,
	/**처형(치명)*/
	Executed_Fatality UMETA(DisplayName = "Executed (Fatality)") ,
	/**방어가 깨진*/
	DefenseBroken UMETA(DisplayName = "Defense Broken") ,
	/**방어 기절*/
	BlockStunned UMETA(DisplayName = "Block Stunned") ,
	/**일어나다*/
	GettingUp UMETA(DisplayName = "GettingUp") ,
	/**도구룰 사용중인*/
	UsingTool UMETA(DisplayName = "UsingTool") ,
	/**조준*/
	Aiming UMETA(DisplayName = "Aiming") ,
	/**죽음*/
	Dead UMETA(DisplayName = "Dead") ,
	/**비활성화(장애)*/
	Disabled UMETA(DisplayName = "Disabled") ,
	/**일반적인 행동*/
	GeneralAction UMETA(DisplayName = "General Action")
};

/**전투 유형*/
UENUM(BlueprintType)
enum class EGeneralCombatType : uint8
{
	//아무것도 아닌
	None UMETA(DisplayName = "None"),
	//근접 전투
	MeleeCombat UMETA(DisplayName = "Melee Combat"),
	//원거리 전투
	RangedCombat UMETA(DisplayName = "Ranged Combat"),
	//마법 전투
	MagicCombat UMETA(DisplayName = "Magic Combat")
};

/**일어나는 유형*/
UENUM(BlueprintType)
enum class EGetupType : uint8
{
	None UMETA(DisplayName = "None"),
	/**위로 향하게 일어나*/
	GetUpFacingUp UMETA(DisplayName = "GetupFacingUp"),
	/**아래를 향하여 일어나*/
	GetUpFacingDown UMETA(DisplayName = "GetupFacingDown")
};


/**애님몽테이지 구조체*/
USTRUCT(BlueprintType)
struct FAnimMontageStruct
{
	GENERATED_BODY()
public:
	/**애님 몽테이지*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	UAnimMontage* AnimMontage;
	/**플레이 재생속도*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float PlayRate;
	/**애니메이션 시작지점*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float StartMontageTime;
	/**재 설정 시간 비율*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float ResetTimeMultiplier;

	FAnimMontageStruct(): AnimMontage(nullptr), PlayRate(1.0f), StartMontageTime(0), ResetTimeMultiplier(0.8f)
	{
	}
};

/**액션 몽테이지 구조체*/
USTRUCT(BlueprintType)
struct FActionMontage
{
	GENERATED_BODY()
public:
	/**액션 몽테이지들*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TArray<FAnimMontageStruct> ActionMontages;
};
