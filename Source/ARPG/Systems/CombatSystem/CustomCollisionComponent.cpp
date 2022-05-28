#include "CustomCollisionComponent.h"

#include "Global.h"
#include "Datas/CombatData.h"
#include "GameFramework/Character.h"


UCustomCollisionComponent::UCustomCollisionComponent()
{
	MaxTrace = 5;
	TraceRadius = 0.1f;
	CollisionObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic)); //WorldStatic
	CollisionObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn)); //Pawn

	DrawDebugType = EDrawDebugTrace::ForDuration;
}

void UCustomCollisionComponent::Event_InitializeCollision(AActor* InOwningActor)
{
	const auto L_CastCharacter = Cast<ACharacter>(InOwningActor);
	if (L_CastCharacter != nullptr)
		OwningCharacter = Cast<ACharacter>(InOwningActor);
}

void UCustomCollisionComponent::BeginPlay()
{
	Super::BeginPlay();
	OwningCharacter = Cast<ACharacter>(GetOwner());;
	CHECK(OwningCharacter!=nullptr);


	//함수들을 위한 기본준비
	TraceLogicLatentInfo.CallbackTarget = this;
	TraceLogicLatentInfo.ExecutionFunction = TEXT("CollisionTrace_Logic");
	TraceLogicLatentInfo.Linkage = 1;
	TraceLogicLatentInfo.UUID = LatentAction++;
}

void UCustomCollisionComponent::SetCollisionMesh(UPrimitiveComponent* InMesh, const TArray<FName> InSockets)
{
	CollisionComponent = InMesh;
	TraceSockets = InSockets;
}

void UCustomCollisionComponent::EnableCollision(ECollisionTraceTarget InTraceTarget)
{
	//델리게이트 호출
	if (OnCollisionEnabled.IsBound())
		OnCollisionEnabled.Broadcast(InTraceTarget);

	bEnableCollisionTrace = true; //콜리전충돌을 활성화

	ClearHitActors(); //히트된 배열을 클리어한다

	UpdateSocketTraceLocations(); //소켓의 위치를 업데이트 한다

	CollisionTrace_Start(); //콜리전 추적
}

bool UCustomCollisionComponent::CanHitActor(const bool WasHit, AActor* InHitActor, const UPrimitiveComponent* InHitComponent) const
{
	//히트 될 액터를 설정
	AActor* L_HitActor = InHitActor;

	//히트 된 액터가 Owner캐릭터와 같지 않아야 한다
	const bool L_DifferentOwners = (GetOwner() != L_HitActor);
	//히트 된 액터들에 없어야 한다  한명의 적을 한번만 때려야 한다
	const bool L_OnlyOncHit = !HitActors.Contains(L_HitActor);
	//HitComponent의 프로파일 네임이 제외된 프로파일 네임이 아니어야 한다
	const bool L_IgnoreProfile = !CollisionProfileNamesToIgnore.Contains(InHitComponent->GetCollisionProfileName());
	//히트 된 액터가 히트공격에서 제외되어야 할 액터클래스가 아니어야 한다
	const bool IgnoreActor = !ActorClassesToIgnore.Contains(L_HitActor->GetClass());
	return WasHit && L_DifferentOwners && L_OnlyOncHit && L_IgnoreProfile && IgnoreActor;
}

void UCustomCollisionComponent::EnableRadialTrace(FName InTargetSocket)
{
	//소유한 액터의 메쉬컴포넌트를 가지고 온다
	CollisionComponent = OwningCharacter->GetMesh();

	//TraceSockets 설정
	TArray<FName> L_TraceSockets;
	L_TraceSockets.Add(InTargetSocket);

	TraceSockets = L_TraceSockets;

	bEnableCollisionTrace = true; //충돌을 활성화

	ClearHitActors(); ///Hit 배열을 클리어한다.

	UpdateSocketTraceLocations(); //소켓위치를 업데이트 한다

	CollisionTrace_Start(); //콜리전 추적
}

void UCustomCollisionComponent::ClearHitActors()
{
	HitActors.Empty();
	HitComponents.Empty();
}

void UCustomCollisionComponent::CollisionTrace_Start()
{
	CollisionTraceGate.Open();
	CollisionTrace_Logic();
}

void UCustomCollisionComponent::CollisionTrace_Logic()
{
	if (CollisionTraceGate.IsOpen())
	{
		//콜리전 추적이 활성화라면
		if (bEnableCollisionTrace)
			TraceSocketsLineTrace();
		else //콜리전 추적이 비활성화라면
			CollisionTraceGate.Close();

		UKismetSystemLibrary::Delay(this, 0.01f, TraceLogicLatentInfo);
	}
}

void UCustomCollisionComponent::TraceSocketsLineTrace()
{
	for (int32 i = 0; i < TraceSockets.Num(); i++)
	{
		const FVector L_StartTrace = CollisionComponent->GetSocketLocation(TraceSockets[i]); //콜리전 충돌 시작위치는 소켓부터
		const FVector L_EndTrace = TracesStartLocations[i]; //콜리전 스타트 지점부터
		const float L_Radius = TraceRadius + TraceRadiusModifier;

		TArray<AActor*> IgnoreActors;
		IgnoreActors.Add(GetOwner());
		IgnoreActors.Add(OwningCharacter);

		TArray<FHitResult> L_OutHits;

		const bool L_IsHit = UKismetSystemLibrary::SphereTraceMultiForObjects(this, L_StartTrace, L_EndTrace, L_Radius,
		                                                                      CollisionObjectTypes, false, IgnoreActors,
		                                                                      DrawDebugType, L_OutHits, true);

		for (const auto OutHit : L_OutHits)
		{
			LastHit = OutHit; //마지막 히트설정

			//히트가 가능하다면
			if (CanHitActor(L_IsHit, LastHit.GetActor(), LastHit.GetComponent()))
			{
				HitActor = LastHit.GetActor(); //히트된 액터를 설정

				//폰으로 캐스팅된다면
				if (AddHitActor(HitActor))
				{
					//폰 히트 호출
					if (OnHit.IsBound())
						OnHit.Broadcast(LastHit, ECC_Pawn);
				}
				else //폰으로 캐스팅이 안된다면
				{
					//히트된 컴포넌트에 마지막히트컴포넌트가 포함되지 않는다면
					if (!HitComponents.Contains(LastHit.GetComponent()))
					{
						HitComponents.Add(LastHit.GetComponent());
						if (OnHit.IsBound())
							OnHit.Broadcast(LastHit, ECC_WorldStatic);
					}
				}
			}
		} //for Break
		TracesStartLocations[i] = CollisionComponent->GetSocketLocation(TraceSockets[i]);
	}
}

bool UCustomCollisionComponent::AddHitActor(AActor* InHitActor)
{
	const APawn* L_CastPawn = Cast<APawn>(InHitActor);

	if (L_CastPawn != nullptr)
	{
		HitActors.Add(InHitActor); //히트액터들에 추가한다.
		return true;
	}
	return false;
}

void UCustomCollisionComponent::UpdateSocketTraceLocations()
{
	TracesStartLocations.Empty(); //TracesStartLocations 배열을 비운다.
	const float L_TraceSocketResize = UKismetMathLibrary::Clamp(MaxTrace + MaxTracesModifier, 1, TraceSockets.Num());
	TraceSockets.SetNum(L_TraceSocketResize); //TraceSocket의 사이즈를 재설정 한다

	for (const auto Socket : TraceSockets)
		TracesStartLocations.Add(CollisionComponent->GetSocketLocation(Socket));
}

void UCustomCollisionComponent::SetCollisionProfileNamesToIgnore(FName InIgnoreProfileName)
{
	CollisionProfileNamesToIgnore.Add(InIgnoreProfileName);
}
