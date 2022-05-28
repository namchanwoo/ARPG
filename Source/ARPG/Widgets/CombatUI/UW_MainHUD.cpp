#include "UW_MainHUD.h"

#include "UW_StatBar.h"
#include "GlobalFunctions/CInventoryEquipmentLibrary.h"
#include "GlobalFunctions/CRPGLibrary.h"
#include "Systems/CombatSystem/CStatsManagerComponent.h"


void UUW_MainHUD::Initialize_StatBars(UCStatsManagerComponent* OwnerStatsComponent)
{
	// WBP_PointsLabel->InitializeSoulsLabel(OwnerStatsComponent);

	AssignStatsEvents(OwnerStatsComponent);

	HealthStatBar->InitializeStatBar(OwnerStatsComponent);
	ManaStatBar->InitializeStatBar(OwnerStatsComponent);
	StaminaStatBar->InitializeStatBar(OwnerStatsComponent);
	BurningResist->InitializeStatBar(OwnerStatsComponent);
	PoisonResist->InitializeStatBar(OwnerStatsComponent);
}

void UUW_MainHUD::AssignStatsEvents(UCStatsManagerComponent* InStatsManagerComponent)
{
	InStatsManagerComponent->OnStatusEffectApplied.AddDynamic(this, &UUW_MainHUD::OnStatusEffectApplied);
	InStatsManagerComponent->OnStatusEffectRemoved.AddDynamic(this, &UUW_MainHUD::OnStatusEffectRemoved);
}

void UUW_MainHUD::OnStatusEffectApplied(EStatusEffects InStatusEffects)
{
}

void UUW_MainHUD::OnStatusEffectRemoved(EStatusEffects InStatusEffects)
{
	SetStatusEffectBarEnabled(InStatusEffects, false);
}

UUW_StatBar* UUW_MainHUD::GetStatBar(EStats InStat)
{
	switch (InStat)
	{
	case EStats::Health: return HealthStatBar;

	case EStats::Stamina: return StaminaStatBar;

	case EStats::Mana: return ManaStatBar;

	case EStats::PoisonResist: return PoisonResist;

	case EStats::BurningResist: return BurningResist;

	default: return nullptr;
	}
}

UUW_StatBar* UUW_MainHUD::GetStatusBar(EStatusEffects InStatusEffect)
{
	switch (InStatusEffect)
	{
	case EStatusEffects::Burning: return BurningResist;

	case EStatusEffects::Poison: return PoisonResist;

	default: return nullptr;
	}
}

void UUW_MainHUD::SetStatusEffectBarEnabled(EStatusEffects InStatusEffect, bool InEnabled)
{
	if (IsValid(GetStatusBar(InStatusEffect)))
	{
		ESlateVisibility L_Visiblity;
		if (InEnabled)
			L_Visiblity = ESlateVisibility::SelfHitTestInvisible;
		else
			L_Visiblity = ESlateVisibility::Hidden;
		
		GetStatusBar(InStatusEffect)->SetVisibility(L_Visiblity);

		CInventoryEquipmentLibrary::SetWidgetActive(GetStatusBar(InStatusEffect), InEnabled);
	}
}
