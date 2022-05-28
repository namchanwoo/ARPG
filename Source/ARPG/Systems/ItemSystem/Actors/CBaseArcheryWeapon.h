
#pragma once

#include "CoreMinimal.h"
#include "CBaseWeapon.h"
#include "CBaseArcheryWeapon.generated.h"


class UTimelineComponent;
class ACBaseArcherProjectile;

UCLASS(Blueprintable)
class ARPG_API ACBaseArcheryWeapon : public ACBaseWeapon
{
	GENERATED_BODY()

public:
	ACBaseArcheryWeapon();

protected:
	virtual void BeginPlay() override;

public:
	virtual void OnConstruction(const FTransform& Transform) override;
	
	virtual void Tick(float DeltaSeconds) override;
	
	void Reload();

	UFUNCTION()
	void StartDrawStringTimeLine(bool Reverse,float TL_PlayRate);

	UFUNCTION()
	void StopDrawStringTimeLine();

public:
	/**탄약을 어태치 합니다*/
	bool AttachAmmo(FName InSocketName, EWeaponProjectileState NewArrowState,UPrimitiveComponent* InAttachParent);

	/**탄약이 무기에 어태치 되었다면 호출됩니다 자식클래스에서 재정의합니다*/
	virtual void OnAmmoAttached(bool AttachedToWeapon);

	/**액터를 어태치 합니다*/
	virtual bool AttachActor(FName InSocketName, bool InUseExtraMesh) override;

	/**화살의 가시성을 업데이트 한다*/
	void UpdateArrowVisibility(bool InVisible);

	/**화살을 어태치합니다*/
	void AttachArrow(bool IsAttachToCharacter);

	/**화살 메쉬를 업데이트한다*/
	void UpdateArrowMesh();

	/**화살 메쉬의 가시성을 체크한다*/
	bool IsArrowMeshVisible();

	/**아이템을 장착합니다*/
	virtual void OnItemEquipped() override;

private:
	UFUNCTION()
	void DrawString_Update(float InAlpha);

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////FIELD MEMBERS//////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
	UPROPERTY(BlueprintReadWrite,EditAnywhere, Category="ArcheryWeapon | Default",meta=(AllowPrivateAccess="true"))
	UStaticMeshComponent* ArrowMesh;
	
	UPROPERTY(BlueprintReadWrite,EditAnywhere, Category="ArcheryWeapon | Initialize",meta=(AllowPrivateAccess="true"))
	TArray<TSubclassOf<ACBaseItem>> AmmoUseClass;

	UPROPERTY(BlueprintReadWrite, Category="ArcheryWeapon | Default",meta=(AllowPrivateAccess="true"))
	float DrawStringAlpha;

	UPROPERTY(BlueprintReadWrite,meta=(AllowPrivateAccess="true"))
	ACBaseArcherProjectile* ArcheryProjectile;

	UPROPERTY(BlueprintReadOnly,meta=(AllowPrivateAccess="true"))
	UTimelineComponent* TimeLine_DrawString;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,meta=(AllowPrivateAccess="true"))
	UCurveFloat* FC_DrawString;
};
