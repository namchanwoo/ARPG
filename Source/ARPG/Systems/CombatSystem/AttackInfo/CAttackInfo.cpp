#include "CAttackInfo.h"


void UCAttackInfo::StoreAttackInfo(EMeleeSwingDirection InMeleeSwingDirection, EAttackPower InAttackPower,
                                   ECustomAttackTypes InCustomAttackTypes, bool InCauseKnockBack,
                                   float InKnockBackTime, float InKnockBackSpeed, bool InIsUnblockableAttack,
                                   EAttackCategory InAttackCategory, ECombatType InCombatType, int32 InAttackIndex)
{
	MeleeSwingDirection = InMeleeSwingDirection;
	AttackPower = InAttackPower;
	CustomAttackTypes = InCustomAttackTypes;
	bCauseKnockBack = InCauseKnockBack;
	KnockBackTime = InKnockBackTime;
	KnockBackSpeed = InKnockBackSpeed;
	bIsUnBlockableAttack = InIsUnblockableAttack;
	AttackCategory = InAttackCategory;
	CombatType = InCombatType;
	AttackIndex = InAttackIndex;
}

EHitReactionType UCAttackInfo::GetCustomAttackType(EImpactDecision InImpactDecision)
{
	if (TestCustomAttackType.Contains(InImpactDecision))
		return *TestCustomAttackType.Find(InImpactDecision);
	return EHitReactionType::None;
}
