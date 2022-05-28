#pragma once

#include "CoreMinimal.h"
#include "Systems/ItemSystem/Actors/CBaseItem.h"
#include "CBaseProjectile.generated.h"


class UCustomCollisionComponent;
class UArrowComponent;
class UCBaseImpactEffect;
class UCAttackInfo;
class UProjectileMovementComponent;


UCLASS()
class ARPG_API ACBaseProjectile : public ACBaseItem
{
	GENERATED_BODY()
public:
	ACBaseProjectile();

protected:
	virtual void BeginPlay() override;

	/**스폰 될 때 데이터 초기화*/
	void InitializeSpawnData(EWeaponProjectileState InSpawnArrowState, float InRegularDamage,
	                         float InPoiseDamage, UCAttackInfo* InAttackInfo, UCRPGItemAsset* InDataAsset);

public:
	UFUNCTION(Category="DelegateBinding")
	virtual void OnHit_ProjectileComponent(FHitResult HitResult, ECollisionChannel HitCollisionChannel);

	UFUNCTION(Category="DelegateBinding")
	void OnCollisionEnabled_ProjectileComponent(ECollisionTraceTarget TraceTarget);

	/**메테리얼에 따른 임팩트 이펙트*/
	virtual void MaterialImpactEffect(EPhysicalSurface InPhysicalSurface);

	/**발사체의 속도를 설정한다*/
	virtual void SetProjectileVelocity();

	/***/
	UFUNCTION()
	void ShotProjectileAfter();

	/**발사체 발사*/
	void FireProjectile(float InRegularDamage, float InPoiseDamage);

#pragma region                Weapon Projectile State Methods
public:
	/**무기의 발사체 상태를 설정합니다*/
	void SetWeaponProjectileState(EWeaponProjectileState NewArrowState);

	/**상태가 시작할 때 호출*/
	void OnStateBegin(EWeaponProjectileState NewArrowState);

	/**상태가 끝날 때 호출*/
	void OnStateEnd(EWeaponProjectileState PrevArrowState);

#pragma endregion             Weapon Projectile State Methods


public:
	/**방향 회전을 가져옵니다*/
	FRotator GetDirection();

	/**헤드샷이라면 헤드샷데미지를 가져옵니다*/
	float GetDamageWithMultipliers();

	/**죽음의 물리속도를 조정합니다1*/
	void AdjustDeathPhysicsVelocity();

	/**Lerp Damage Stat*/
	float LerpDamageStat(float InStat, float InChargeAmount);

	/**공격 정보를 저장합니다*/
	void StoreAttackInfo(UCAttackInfo* InAttackInfo);

	/**피해를 가한다*/
	void TryToApplyDamage(AActor* InHitActor);


#pragma region                IInterface_WorldItem
public:
	virtual EWeaponProjectileState I_GetProjectileState() override;

	virtual void I_ShootArrow(float InChargeAmount, float InRegularDamage, float InPoiseDamage) override;

	virtual void I_ShootProjectile(float InRegularDamage, float InPoiseDamage) override;

	virtual void I_InitializeQuiver(int32 Arrows, UStaticMesh* ArrowMesh) override;

	/**화살통을 업데이트 합니다*/
	virtual void I_UpdateQuiver(int32 InAmount) override;
#pragma endregion             IInterface_WorldItem


	/*****************************************************************************************************************FIELD MEMBERS************/
	/*****************************************************************************************************************FIELD MEMBERS************/
	/*****************************************************************************************************************FIELD MEMBERS************/

	int32 LatentID;
	FLatentActionInfo ProjectileLogicLatentInfo;
	

#pragma region               Default Members
protected:
	/**웨폰 Root 컴포넌트*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="BaseProjectile | Default", meta = (AllowPrivateAccess = "true"))
	USceneComponent* WeaponRoot;

	/**Projectile 액터의 방향을 알려주는 에로우 컴포넌트*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Initialize | Default", meta = (AllowPrivateAccess = "true"))
	UArrowComponent* ArrowComponent;

	/**발사체 충돌컴포넌트*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Base Projectile | Default", meta = (AllowPrivateAccess = "true"))
	UCustomCollisionComponent* ProjectileCollisionComponent;

	/**파티클 트레일 컴포넌트*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Base Projectile | Default", meta = (AllowPrivateAccess = "true"))
	UParticleSystemComponent* ParticleTrail;

	/**스폰할 카메라쉐이크 클래스*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Base Projectile | Initialize", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UMatineeCameraShake> CameraShake_Class;
#pragma endregion            Default Members


#pragma region               Component
protected:
	/**파티클 시스템 컴포넌트*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Base Projectile | Component", meta = (AllowPrivateAccess = "true"))
	UParticleSystemComponent* ParticleSystem;

	/**프로젝타이물 무브먼트 컴포넌트*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Base Projectile | Component", meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* ProjectileComponent;

#pragma endregion            Component


#pragma region               Projectile Parameters Members
protected:
	/**발사체 초기 스피드 */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Base Projectile | Projectile Parameters", meta = (AllowPrivateAccess = "true"))
	float ProjectileInitialSpeed;

	/**회전이 속도를 따르는지의 여부*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Base Projectile | Projectile Parameters", meta = (AllowPrivateAccess = "true"))
	bool bRotationFollowsVelocity;
#pragma endregion            Projectile Parameters Members


#pragma region               Applying Damage Members
protected:
	/**마지막 히트 정보*/
	FHitResult LastHit;

	/**타격 위치(지점)*/
	FVector ImpactPoint;

	/**추가피해 유형 값*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Base Projectile | ApplyDamage")
	float AdditionalDamageTypeAmount;

	/**추가 피해 유형*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Base Projectile | ApplyDamage")
	TSubclassOf<UDamageType> AdditionalDamageType;

	/**일반 데미지*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Base Projectile | ApplyDamage",meta=(ExposeOnSpawn="true",AllowPrivateAccess="true"))
	float RegularDamage;

	/**평정 데미지*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Base Projectile | ApplyDamage",meta=(ExposeOnSpawn="true",AllowPrivateAccess="true"))
	float PoiseDamage;

#pragma endregion            Applying Damage Members


#pragma region               Sound Effects Members
protected:
	/**발사체 사운드*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Base Projectile | Sound Efeects", meta=(AllowPrivateAccess="true"))
	USoundBase* ShootProjectileSound;
#pragma endregion            Sound Effects Members


#pragma region               Hit Info Members
protected:
	UPROPERTY()
	ACharacter* HitCharacter;

	TEnumAsByte<EPhysicalSurface> SurfaceType;

	FVector HitLocation;

	FName HitBoneName;

	/**죽을 때 물리속도*/
	float DeathPhysicsVelocity;

	/**헤드샷 데미지 배율*/
	float HeadShotMultiplier;

	/**기본 물리속도*/
	float DefaultDeathPhysicsVelocity;

	FCombatHitData LastCombatHidData;

#pragma endregion            Hit Info Members


#pragma region               Projectile State
protected:
	/**발사체 상태*/
	EWeaponProjectileState ProjectileState;

	/**스폰된 Arrow 상태 */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Base Projectile | Projectile State", meta = (AllowPrivateAccess = "true"))
	EWeaponProjectileState SpawnArrowState;

#pragma endregion            Projectile State


#pragma region               Attack Info
protected:
	/** 공격 정보 */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Base Projectile | Attack info", meta = (AllowPrivateAccess = "true",ExposeOnSpawn="true"))
	UCAttackInfo* AttackInfo;

	/**사용자 정의 공격 타입*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Base Projectile | Attack info", meta = (AllowPrivateAccess = "true"))
	ECustomAttackTypes CustomAttackType;

	/**넉백의 원인?*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Base Projectile | Attack info", meta = (AllowPrivateAccess = "true"))
	bool CauseKnockBack;

	/**넉백 타임*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Base Projectile | Attack info", meta = (AllowPrivateAccess = "true"))
	float KnockBackTime;

	/**넉백 스피드*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Base Projectile | Attack info", meta = (AllowPrivateAccess = "true"))
	float KnockBackSpeed;

	/**차단 불가능 발사체인가?*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Base Projectile | Attack info", meta = (AllowPrivateAccess = "true"))
	bool bIsUnBlockAbleProjectile;

	/**공격 임팩트FX*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Base Projectile | Attack info", meta = (AllowPrivateAccess = "true"))
	UCBaseImpactEffect* AttackImpactFX;
#pragma endregion                           Attack Info


public:
	FORCEINLINE EWeaponProjectileState GetArrowState() const { return ProjectileState; }

	FORCEINLINE float GetDeathPhysicsVelocity() const { return DeathPhysicsVelocity; }
};
