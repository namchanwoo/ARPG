#include "CBaseArcheryWeapon.h"
#include "Global.h"
#include "Components/ArrowComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Character.h"
#include "Interfaces/Interface_AnimInstance.h"
#include "Interfaces/Interface_Combat.h"
#include "Projectiles/CBaseArcherProjectile.h"
#include "Systems/CombatSystem/CEquipmentManagerComponent.h"
#include "Systems/ItemSystem/CRPGItemAsset.h"


ACBaseArcheryWeapon::ACBaseArcheryWeapon()
{
	ArrowMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ArrowMesh"));
	ArrowMesh->SetupAttachment(ItemSkeletalMesh);
	ArrowMesh->SetVisibility(false);

	GetFocusCollision()->SetVisibility(false);
	GetFocusCollision()->SetCollisionProfileName(TEXT("Custom"));
	GetFocusCollision()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetFocusCollision()->SetCollisionObjectType(ECC_WorldDynamic);
	GetFocusCollision()->SetCollisionResponseToAllChannels(ECR_Overlap);
	GetFocusCollision()->SetCollisionResponseToChannel(ECC_Interactable, ECR_Block);
	GetMeleeWeaponGuideArrow()->SetVisibility(false);

	ConstructorHelpers::FObjectFinder<UCurveFloat> FC_DrawString_Src(TEXT("CurveFloat'/Game/Game_/Datas/Curves/FC_DrawString.FC_DrawString'"));
	if (FC_DrawString_Src.Succeeded())
		FC_DrawString = FC_DrawString_Src.Object;
}

void ACBaseArcheryWeapon::BeginPlay()
{
	Super::BeginPlay();


	//Timeline_ClimbStart 설정
	{
		//플롯커브가 존재한다면
		CHECK(FC_DrawString!=nullptr);

		TimeLine_DrawString = NewObject<UTimelineComponent>(this, UTimelineComponent::StaticClass(), FName(TEXT("TimeLine_DrawString")));

		FOnTimelineFloat DrawString_UpdateCallback;
		DrawString_UpdateCallback.BindUFunction(this, FName(TEXT("DrawString_Update")));
		TimeLine_DrawString->AddInterpFloat(FC_DrawString, DrawString_UpdateCallback);

		// FOnTimelineEvent DrawString_UpdateCallback;
		// FOnTimelineEventStatic DrawString_FinishedCallback;

		// DrawString_UpdateCallback.BindUFunction(this, FName(TEXT("TimelineEvent_ClimbStart_Update")));
		// DrawString_FinishedCallback.BindUFunction(this, FName(TEXT("TimelineEvent_ClimbStart_Finished")));

		// TimeLine_DrawString->SetTimelinePostUpdateFunc(DrawString_UpdateCallback); //타임라인 업데이트 함수 연결
		// TimeLine_DrawString->SetTimelineFinishedFunc(DrawString_FinishedCallback); //타임라인 피니쉬일때 함수연결

		TimeLine_DrawString->SetTimelineLength(5.0f);
		TimeLine_DrawString->SetTimelineLengthMode(TL_TimelineLength);
		TimeLine_DrawString->RegisterComponent();
	}
}

void ACBaseArcheryWeapon::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

void ACBaseArcheryWeapon::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	TimeLine_DrawString->TickComponent(DeltaSeconds, LEVELTICK_TimeOnly, nullptr); //타임라인 컴포넌트 틱설정
}

void ACBaseArcheryWeapon::Reload()
{
	//궁수 발사체가 유효하지 않다면
	if (!IsValid(ArcheryProjectile))
	{
	}
	else //궁수 발체가 유효하다면
	{
	}
}

void ACBaseArcheryWeapon::StartDrawStringTimeLine(bool Reverse, float TL_PlayRate)
{
	TimeLine_DrawString->SetPlayRate(TL_PlayRate);

	//리버스가 아니라면
	if (!Reverse)
		TimeLine_DrawString->Play();
	else
		TimeLine_DrawString->Reverse();
}

void ACBaseArcheryWeapon::StopDrawStringTimeLine()
{
	TimeLine_DrawString->Stop();
}

bool ACBaseArcheryWeapon::AttachAmmo(FName InSocketName, EWeaponProjectileState NewArrowState, UPrimitiveComponent* InAttachParent)
{
	//아처 발사체가 유효하다면
	if (IsValid(ArcheryProjectile))
	{
		//어태치합니다
		ArcheryProjectile->AttachToComponent(InAttachParent, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), InSocketName);

		//아처 발사체의 발사상태를 설정합니다
		ArcheryProjectile->SetWeaponProjectileState(NewArrowState);

		OnAmmoAttached(NewArrowState == EWeaponProjectileState::InWeapon);
		return true;
	}

	return false;
}

void ACBaseArcheryWeapon::OnAmmoAttached(bool AttachedToWeapon)
{
}

bool ACBaseArcheryWeapon::AttachActor(FName InSocketName, bool InUseExtraMesh)
{
	IInterface_Combat* Combat_Interface = Cast<IInterface_Combat>(GetOwner());
	CHECK(Combat_Interface!=nullptr, false);

	//전투상태라면
	if (Combat_Interface->I_CombatEnabled())
	{
		UpdateArrowMesh();
		UpdateArrowVisibility(true);
		AttachArrow(false);
	}
	else //전투 상태가 아니라면
		UpdateArrowVisibility(false);

	return Super::AttachActor(InSocketName, false);
}

void ACBaseArcheryWeapon::UpdateArrowVisibility(bool InVisible)
{
	ArrowMesh->SetVisibility(InVisible, false);
}

void ACBaseArcheryWeapon::AttachArrow(bool IsAttachToCharacter)
{
	CHECK(IsValid(DataAsset)); //데이터 에셋이 유효해야합니다

	const FAttachmentTransformRules AttachmentTransformRules =
		FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true);

	//캐릭터에 어태치 해야한다면
	if (IsAttachToCharacter)
	{
		USkeletalMeshComponent* L_CharacterSkeletalMesh = Cast<ACharacter>(GetOwner())->GetMesh();
		ArrowMesh->AttachToComponent(L_CharacterSkeletalMesh, AttachmentTransformRules, DataAsset->PlayerHandStringSocketName);
	}
	else
		ArrowMesh->AttachToComponent(GetItemMeshComponent(), AttachmentTransformRules, DataAsset->ArcheryProjectileEquipSocket);
}

void ACBaseArcheryWeapon::UpdateArrowMesh()
{
	CHECK(IsValid(DataAsset));

	IInterface_Combat* Combat_Interface = Cast<IInterface_Combat>(GetOwner());
	CHECK(Combat_Interface!=nullptr);
	ArrowMesh->SetStaticMesh(Combat_Interface->I_GetArrowMesh());
}

bool ACBaseArcheryWeapon::IsArrowMeshVisible()
{
	return ArrowMesh->IsVisible();
}

void ACBaseArcheryWeapon::OnItemEquipped()
{
	Super::OnItemEquipped();

	CHECK(IsValid(DataAsset));

	const ACharacter* CastCharacter = Cast<ACharacter>(GetOwner());
	IInterface_AnimInstance* Anim_Interface = Cast<IInterface_AnimInstance>(CastCharacter->GetMesh()->GetAnimInstance());
	CHECK(Anim_Interface!=nullptr);


	Anim_Interface->I_SetBowDrawSpeed(DataAsset->BowDrawSpeedRate);

	const auto L_ItemAnimInstance = ItemSkeletalMesh->GetAnimInstance();
	CHECK(L_ItemAnimInstance!=nullptr);
	IInterface_AnimInstance* L_ItemAnim_Interface = Cast<IInterface_AnimInstance>(L_ItemAnimInstance);
	CHECK(L_ItemAnim_Interface!=nullptr);
	L_ItemAnim_Interface->I_SetPlayerHandStringSocketName(DataAsset->PlayerHandStringSocketName);
	
	if (IsValid(EquipmentComponent))
		EquipmentComponent->SetArcherWeapon(this);
}

void ACBaseArcheryWeapon::DrawString_Update(float InAlpha)
{
	LOG_SCREEN(TEXT("DrawString 타임라인 업데이트중"));
	DrawStringAlpha = InAlpha;
}
