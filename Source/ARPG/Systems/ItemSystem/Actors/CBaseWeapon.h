#pragma once

#include "CoreMinimal.h"
#include "CBaseItem.h"
#include "CBaseWeapon.generated.h"


class UCustomCollisionComponent;
class UArrowComponent;
class UCapsuleComponent;


UCLASS(Blueprintable)
class ARPG_API ACBaseWeapon : public ACBaseItem
{
	GENERATED_BODY()

public:
	ACBaseWeapon();

protected:
	virtual void BeginPlay() override;


public:
	virtual void OnHit(FHitResult InHitResult, ECollisionChannel InCollisionChannel);

	UFUNCTION()
	void OnCollisionEnabled_WeaponCollision(ECollisionTraceTarget TraceTarget);

	UFUNCTION()
	void OnCollisionEnabled_SecondWeaponCollision(ECollisionTraceTarget TraceTarget);

	UFUNCTION()
	void OnHit_WeaponCollision(FHitResult InHitResult, ECollisionChannel InCollisionChannel);

	UFUNCTION()
	void OnHit_SecondWeaponCollision(FHitResult InHitResult, ECollisionChannel InCollisionChannel);

	/**트레일을 시작한다*/
	void StartTrail(UParticleSystem* InBaseWeaponTrail, bool UseNotifyTrail);

	/**트레일을 끝낸다*/
	void EndTrail();

public:
	/**두번 째 무기의 메쉬를 가져옵니다*/
	UPrimitiveComponent* GetSecondWeaponMesh();

	/****************************************************************************************************************************FIELD MEMBERS********/
	/****************************************************************************************************************************FIELD MEMBERS********/
	/****************************************************************************************************************************FIELD MEMBERS********/
#pragma region               Initialize Members
private:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "BaseWeapon | Initialize", meta=(AllowPrivateAccess="true"))
	FName WeaponStartSocket;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "BaseWeapon | Initialize", meta=(AllowPrivateAccess="true"))
	FName WeaponEndSocket;

#pragma endregion            Initialize Members


#pragma region               Default Members
private:
	UPROPERTY()
	ACharacter* OwnerCharacter;

	/**무기 충돌 컴포넌트*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "BaseWeapon | Default",meta=(AllowPrivateAccess="true"))
	UCustomCollisionComponent* WeaponCollisionComponent;

	/**두 번째 무기 충돌 컴포넌트*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "BaseWeapon | Default",meta=(AllowPrivateAccess="true"))
	UCustomCollisionComponent* SecondWeaponCollisionComponent;

	/**웨폰의 방향을 대변하는 에로우 컴포넌트*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "BaseWeapon | Default",meta=(AllowPrivateAccess="true"))
	UArrowComponent* MeleeWeaponGuideArrow;

	/**웨폰 트레일 파티클 컴포넌트*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "BaseWeapon | Default",meta=(AllowPrivateAccess="true"))
	UParticleSystemComponent* ParticleTrail;

	/**웨폰의 루트 컴포넌트*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "BaseWeapon | Default",meta=(AllowPrivateAccess="true"))
	USceneComponent* WeaponRoot;

	/**웨폰의 충돌체 콜리전*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "BaseWeapon | Default",meta=(AllowPrivateAccess="true"))
	UCapsuleComponent* FocusCollision;
#pragma endregion            Default Members


#pragma region               Component Members
private:
	/**파티클 시스템 컴포넌트*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "BaseWeapon | Components", meta=(AllowPrivateAccess="true"))
	UParticleSystemComponent* ParticleSystem;

	/**오너캐릭터의 충돌 콜리전컴포넌트*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "BaseWeapon | Components", meta=(AllowPrivateAccess="true"))
	UCustomCollisionComponent* OwnerCollisionComponent;

	/**두번째 무기 스테틱 메쉬*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "BaseWeapon | Components", meta=(AllowPrivateAccess="true"))
	UStaticMeshComponent* SecondWeaponStaticMesh;

	/**두 번째 무기 스켈레탈 메시 컴포넌트*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "BaseWeapon | Components", meta=(AllowPrivateAccess="true"))
	USkeletalMeshComponent* SecondWeaponSkeletalMeshComponent;
#pragma endregion            Component Members


#pragma region               Applying Damage Members
private:
	/**마지막 히트결과*/
	FHitResult LastHit;
#pragma endregion            Applying Damage Members


#pragma region               Effect Members
private:
	/**무기트레일 파티클*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BaseWeapon | Effect", meta = (AllowPrivateAccess = "true"))
	UParticleSystem* WeaponTrail;
#pragma endregion            Effect Members

public:
	FORCEINLINE UCapsuleComponent* GetFocusCollision() { return FocusCollision; }
	FORCEINLINE UArrowComponent* GetMeleeWeaponGuideArrow() { return MeleeWeaponGuideArrow; }
};


