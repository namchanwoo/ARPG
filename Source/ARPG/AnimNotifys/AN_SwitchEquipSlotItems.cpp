#include "AN_SwitchEquipSlotItems.h"

#include "Systems/CombatSystem/CEquipmentManagerComponent.h"


FString UAN_SwitchEquipSlotItems::GetNotifyName_Implementation() const
{
	return TEXT("C_AN_SwitchEquipSlotItems");
}

void UAN_SwitchEquipSlotItems::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);
	if (MeshComp == nullptr) return;
	if (MeshComp->GetOwner() == nullptr) return;


	// UCharacterStateManagerComponent* CharacterStateManagerComponent = Cast<UCharacterStateManagerComponent>(MeshComp->GetOwner()->GetComponentByClass(UCharacterStateManagerComponent::StaticClass()));
	UCEquipmentManagerComponent* EquipmentManagerComponent =
		Cast<UCEquipmentManagerComponent>(MeshComp->GetOwner()->GetComponentByClass(UCEquipmentManagerComponent::StaticClass()));

	if (EquipmentManagerComponent != nullptr)
	{
		EquipmentManagerComponent->SetCombatEnabled(true);
		EquipmentManagerComponent->SwitchEquipSlotActiveIndex(EquipSlots);
	}
}
