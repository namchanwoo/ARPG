

#include "CBaseHitReaction.h"




UCBaseHitReaction::UCBaseHitReaction()
{
	//기본 반응 값 초기화들
	bAutoResetCharacterState = true;
	HitReactionCharacterState = ECharacterState::HitStunned;
	HitReactionCategory = EHitReactionCategory::GeneralHitReaction;

	HitReactIndexType = EHitReactIndexType::CustomIndex;

	AttackPowerIndex.Add(EAttackPower::Light, 0);
	AttackPowerIndex.Add(EAttackPower::Medium, 0);
	AttackPowerIndex.Add(EAttackPower::Heavy, 0);
	AttackPowerIndex.Add(EAttackPower::VeryHeavy, 0);

	PoiseWeightIndex.Add(EAttackPower::Light, 0);
	PoiseWeightIndex.Add(EAttackPower::Medium, 0);
	PoiseWeightIndex.Add(EAttackPower::Heavy, 0);
	PoiseWeightIndex.Add(EAttackPower::VeryHeavy, 0);

	PoiseWeightMaxPercentage.Add(EAttackPower::Light, 25);
	PoiseWeightMaxPercentage.Add(EAttackPower::Medium, 50);
	PoiseWeightMaxPercentage.Add(EAttackPower::Heavy, 100);
	PoiseWeightMaxPercentage.Add(EAttackPower::VeryHeavy, 125);

	SwingDirection_Front.Add(EMeleeSwingDirection::Default, EHitReactionDirection::Front);
	SwingDirection_Front.Add(EMeleeSwingDirection::HorizontalLeft, EHitReactionDirection::Right);
	SwingDirection_Front.Add(EMeleeSwingDirection::HorizontalRight, EHitReactionDirection::Left);
	SwingDirection_Front.Add(EMeleeSwingDirection::VerticalDown, EHitReactionDirection::Front);
	SwingDirection_Front.Add(EMeleeSwingDirection::VerticalUp, EHitReactionDirection::Front);
	SwingDirection_Front.Add(EMeleeSwingDirection::ForwardThrust, EHitReactionDirection::Front);


	SwingDirection_Back.Add(EMeleeSwingDirection::Default, EHitReactionDirection::Back);
	SwingDirection_Back.Add(EMeleeSwingDirection::HorizontalLeft, EHitReactionDirection::Left);
	SwingDirection_Back.Add(EMeleeSwingDirection::HorizontalRight, EHitReactionDirection::Right);
	SwingDirection_Back.Add(EMeleeSwingDirection::VerticalDown, EHitReactionDirection::Back);
	SwingDirection_Back.Add(EMeleeSwingDirection::VerticalUp, EHitReactionDirection::Back);
	SwingDirection_Back.Add(EMeleeSwingDirection::ForwardThrust, EHitReactionDirection::Back);


	SwingDirection_Right.Add(EMeleeSwingDirection::Default, EHitReactionDirection::Right);
	SwingDirection_Right.Add(EMeleeSwingDirection::HorizontalLeft, EHitReactionDirection::Back);
	SwingDirection_Right.Add(EMeleeSwingDirection::HorizontalRight, EHitReactionDirection::Front);
	SwingDirection_Right.Add(EMeleeSwingDirection::VerticalDown, EHitReactionDirection::Right);
	SwingDirection_Right.Add(EMeleeSwingDirection::VerticalUp, EHitReactionDirection::Right);
	SwingDirection_Right.Add(EMeleeSwingDirection::ForwardThrust, EHitReactionDirection::Right);


	SwingDirection_Left.Add(EMeleeSwingDirection::Default, EHitReactionDirection::Left);
	SwingDirection_Left.Add(EMeleeSwingDirection::HorizontalLeft, EHitReactionDirection::Front);
	SwingDirection_Left.Add(EMeleeSwingDirection::HorizontalRight, EHitReactionDirection::Back);
	SwingDirection_Left.Add(EMeleeSwingDirection::VerticalDown, EHitReactionDirection::Left);
	SwingDirection_Left.Add(EMeleeSwingDirection::VerticalUp, EHitReactionDirection::Left);
	SwingDirection_Left.Add(EMeleeSwingDirection::ForwardThrust, EHitReactionDirection::Left);

	MaxRotation = 180.0f;
	RotationPerSecond = 720.0f;
	
	ReactionIn = 0.2f;
	ReactionOut = 0.1f;
	LegBlendWeightMultiplier = 5.0f;
}
