#include "AN_ToggleCombat.h"

#include "Systems/CombatSystem/CEquipmentManagerComponent.h"


FString UAN_ToggleCombat::GetNotifyName_Implementation() const
{
	return TEXT("C_AN_ToggleCombat");
}

void UAN_ToggleCombat::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	if (MeshComp == nullptr) return;
	if (MeshComp->GetOwner() == nullptr) return;

	UCEquipmentManagerComponent* EquipmentManagerComponent =
		Cast<UCEquipmentManagerComponent>(MeshComp->GetOwner()->GetComponentByClass(UCEquipmentManagerComponent::StaticClass()));
	if (EquipmentManagerComponent != nullptr)
		EquipmentManagerComponent->ToggleCombat();
}
