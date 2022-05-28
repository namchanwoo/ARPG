#include "ANS_SetInvulnerable.h"

#include "GlobalFunctions/CLog.h"
#include "Systems/CombatSystem/CCharacterStateManagerComponent.h"


FString UANS_SetInvulnerable::GetNotifyName_Implementation() const
{
	return TEXT("C_ANS_SetInvulnerable");
}

void UANS_SetInvulnerable::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);
	if (MeshComp == nullptr) return;
	if (MeshComp->GetOwner() == nullptr) return;
	UCCharacterStateManagerComponent* StateManagerComponent = Cast<UCCharacterStateManagerComponent>(
		MeshComp->GetOwner()->GetComponentByClass(UCCharacterStateManagerComponent::StaticClass()));

	CHECK(StateManagerComponent != nullptr);
	StateManagerComponent->SetInvulnerable(true);
}

void UANS_SetInvulnerable::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::NotifyEnd(MeshComp, Animation);
	if (MeshComp == nullptr) return;
	if (MeshComp->GetOwner() == nullptr) return;
	UCCharacterStateManagerComponent* StateManagerComponent = Cast<UCCharacterStateManagerComponent>(
		MeshComp->GetOwner()->GetComponentByClass(UCCharacterStateManagerComponent::StaticClass()));

	CHECK(StateManagerComponent != nullptr);
	StateManagerComponent->SetInvulnerable(false);
}
