#include "ANS_RotateCharacter.h"


#include "GameFramework/Character.h"
#include "Interfaces/Interface_Rotation.h"


FString UANS_RotateCharacter::GetNotifyName_Implementation() const
{
	return TEXT("C_ANS_RotateCharacter");
}

void UANS_RotateCharacter::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);
	if (MeshComp == nullptr) return;
	if (MeshComp->GetOwner() == nullptr) return;

	ACharacter* OwnerCharacter = Cast<ACharacter>(MeshComp->GetOwner());
	if (OwnerCharacter != nullptr)
	{
		IInterface_Rotation* IRotation = Cast<IInterface_Rotation>(OwnerCharacter->GetMesh()->GetAnimInstance());
		if (IRotation != nullptr)
			IRotation->I_StartRotating(RotationInterpSpeed);
	}
}

void UANS_RotateCharacter::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::NotifyEnd(MeshComp, Animation);
	if (MeshComp == nullptr) return;
	if (MeshComp->GetOwner() == nullptr) return;

	ACharacter* OwnerCharacter = Cast<ACharacter>(MeshComp->GetOwner());
	if (OwnerCharacter != nullptr)
	{
		IInterface_Rotation* IRotation = Cast<IInterface_Rotation>(OwnerCharacter->GetMesh()->GetAnimInstance());
		if (IRotation != nullptr)
			IRotation->I_StopRotating();
	}
}
