#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Datas/CombatData.h"
#include "GlobalFunctions/Gate.h"
#include "Kismet/KismetSystemLibrary.h"
#include "CustomCollisionComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCollisionEnabled, ECollisionTraceTarget, InTraceTarget);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHit, FHitResult, InHitResult, ECollisionChannel, InHitCollisionChannel);


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ARPG_API UCustomCollisionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCustomCollisionComponent();

protected:
	virtual void BeginPlay() override;

public:
	/** 컴포넌트를 초기화한다 * OwningCharacter 를 설정한다 */
	void Event_InitializeCollision(AActor* InOwningActor);

	/**충돌 컴포넌트와 추적 소켓들을 설정합니다*/
	void SetCollisionMesh(UPrimitiveComponent* InMesh, TArray<FName> InSockets);

	/**콜리전을 활성화 합니다*/
	void EnableCollision(ECollisionTraceTarget InTraceTarget);

	/**Radial형으로 추적 활성화*/
	void EnableRadialTrace(FName InTargetSocket);

	/**HitActors 와 HitComponents 를 클리어한다	*/
	void ClearHitActors();

	/**	액터가 히트가 가능한지 아닌지의 결과를 가져옵니다*/
	bool CanHitActor(bool WasHit, AActor* InHitActor, const UPrimitiveComponent* InHitComponent) const;

private:
	/**충돌 추적을 시작합니다*/
	void CollisionTrace_Start();

	/**충돌 추적 실행 로직입니다*/
	UFUNCTION()
	void CollisionTrace_Logic();

	/**충돌 추적에 필요한 라인트레이스*/
	void TraceSocketsLineTrace();

protected:
	/**공격에 맞은 액터를 추가해준다
	 * Pawn으로 캐스팅됬다면 true Pawn으로 캐스팅 안됬다면 false */
	bool AddHitActor(AActor* InHitActor);

	/**TraceSockets의 FName값으로 Collision의 소켓위치를 얻어와서 TracesStartLocations의 값을 업데이트 한다*/
	void UpdateSocketTraceLocations();

public:
	void SetCollisionProfileNamesToIgnore(FName InIgnoreProfileName);
	/***********************************************************************************************************************************************/
	/************************************************************************FIELD MEMBERS**********************************************************/
	/***********************************************************************************************************************************************/

	FGate CollisionTraceGate;
	FLatentActionInfo TraceLogicLatentInfo;
	int32 LatentAction;
#pragma region               Collision Members
private:
	UPROPERTY(BlueprintReadOnly, Category=" Collision Component | Component", meta = (AllowPrivateAccess = "true"))
	UPrimitiveComponent* CollisionComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Collision Component | Component", meta = (AllowPrivateAccess = "true"))
	UParticleSystem* ParticleSystem;
#pragma endregion            Collision Members


#pragma region               Initialize Member
private:
	/**이 컴포넌트를 소유한 캐릭터*/
	UPROPERTY()
	ACharacter* OwningCharacter;


	/**최대 추적 갯수*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Collision Component | Initialize", meta = (AllowPrivateAccess = "true"))
	int32 MaxTrace;

	/**충돌 할 타입*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Collision Component | Initialize", meta = (AllowPrivateAccess = "true"))
	TArray<TEnumAsByte<EObjectTypeQuery>> CollisionObjectTypes;

	/**추적할 반경*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Collision Component | Initialize", meta = (AllowPrivateAccess = "true"))
	float TraceRadius;

	/**충돌에서 제외 할 프로파일 네임*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Collision Component | Initialize", meta = (AllowPrivateAccess = "true"))
	TArray<FName> CollisionProfileNamesToIgnore;

	/**충돌에서 제외 할 액터 클래스*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Collision Component | Initialize", meta = (AllowPrivateAccess = "true"))
	TArray<TSubclassOf<AActor>> ActorClassesToIgnore;

	/**추적 반경 수정 값*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Collision Component | Initialize", meta = (AllowPrivateAccess = "true"))
	float TraceRadiusModifier;

	/**최대 추적 가능 수정 값*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Collision Component | Initialize", meta = (AllowPrivateAccess = "true"))
	int32 MaxTracesModifier;

	/**디버그 타입*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Collision Component | Initialize", meta = (AllowPrivateAccess = "true"))
	TEnumAsByte<EDrawDebugTrace::Type> DrawDebugType;

#pragma endregion region            Initialize Member


#pragma region               Collision Member
private:
	/**충돌 추적 활성화 상태*/
	bool bEnableCollisionTrace;

	/**히트 된 액터*/
	UPROPERTY()
	AActor* HitActor;

	/**히트된 액터들*/
	UPROPERTY()
	TArray<AActor*> HitActors;

	/**마지막 히트 정보*/
	FHitResult LastHit;

	/**트레이스가 시작될 위치들*/
	TArray<FVector> TracesStartLocations;

	/**충돌 소켓들*/
	TArray<FName> TraceSockets;

	/**추적 부위*/
	ECollisionTraceTarget TraceTarget;

	/**히트된 컴포넌트들*/
	UPROPERTY()
	TArray<UPrimitiveComponent*> HitComponents;
#pragma endregion            Collision Member


#pragma region               Delegate Members
public:
	/** 아이템과 캐릭터에서 함수를 바인딩한다
		 *  콜리전충돌을 활성화 시킬 때 콜한다*/
	UPROPERTY(BlueprintAssignable)
	FOnCollisionEnabled OnCollisionEnabled;

	/** 아이템과 캐릭터에서 함수를 바인딩한다
	 *  히트를 발생시킬 때 콜한다*/
	UPROPERTY(BlueprintAssignable)
	FOnHit OnHit;
#pragma endregion            Delegate Members


#pragma region              FORCEINLINE METHODS
public:
	/**히트 된 액터(들을) 가지고 온다*/
	FORCEINLINE TArray<AActor*> GetHitActors() const { return HitActors; }
	/**마지막 히트결과를 가지고 온다*/
	FORCEINLINE FHitResult GetLastHit() const { return LastHit; }
	/**콜리전을 비활성화 한다*/
	FORCEINLINE void DisableCollision() { bEnableCollisionTrace = false; }
	/**추적 반경을 설정합니다*/
	FORCEINLINE void SetTraceRadius(float InRadius) { TraceRadius = InRadius; }
	/**추적 반경 수정 값을 설정합니다*/
	FORCEINLINE void SetTraceRadiusModifier(float InTraceRadiusModifier) { TraceRadiusModifier = InTraceRadiusModifier; }
	/**최대 트레이스 수정 값을 설정합니다*/
	FORCEINLINE void SetMaxTracesModifier(float InMaxTracesModifier) { MaxTracesModifier = InMaxTracesModifier; }
#pragma endregion                  FORCEINLINE METHODS
};
