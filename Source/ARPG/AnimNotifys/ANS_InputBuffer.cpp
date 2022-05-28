#include "ANS_InputBuffer.h"
#include "Systems/CombatSystem/CCombatComponent.h"

FString UANS_InputBuffer::GetNotifyName_Implementation() const
{
	return TEXT("C_ANS_InputBuffer");
}

void UANS_InputBuffer::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);

	if (MeshComp == nullptr) return;
	if (MeshComp->GetOwner() == nullptr) return;

	UCCombatComponent* CombatComponent = Cast<UCCombatComponent>(MeshComp->GetOwner()->GetComponentByClass(UCCombatComponent::StaticClass()));
	if (CombatComponent != nullptr)
		CombatComponent->OpenInputBuffer();
}

void UANS_InputBuffer::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::NotifyEnd(MeshComp, Animation);
	if (MeshComp == nullptr) return;
	if (MeshComp->GetOwner() == nullptr) return;

	UCCombatComponent* CombatComponent = Cast<UCCombatComponent>(MeshComp->GetOwner()->GetComponentByClass(UCCombatComponent::StaticClass()));
	if (CombatComponent != nullptr)
		CombatComponent->CloseInputBuffer();
}
