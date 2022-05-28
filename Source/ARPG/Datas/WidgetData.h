#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "WidgetData.generated.h"



/**위젯 타입*/
UENUM(BlueprintType)
enum class EUserInterface : uint8
{
	/**아무것도 아닌*/
	None UMETA(DisplayName = "None") ,
	/**일시정지 메뉴*/
	PauseMenu UMETA(DisplayName = "Pause Menu") ,
	/**메인메뉴*/
	MainMenu UMETA(DisplayName = "Main Menu") ,
	/**휴식(모닥불) 메뉴*/
	BonfireMenu UMETA(DisplayName = "Bonfire Menu") ,
	/**확인 메뉴*/
	ConfirmationMenu UMETA(DisplayName = "Confirmation Menu") ,
	/**옵션 메뉴*/
	OptionsMenu UMETA(DisplayName = "Options Menu") ,
	/**컨트롤 메뉴*/
	ControlsMenu UMETA(DisplayName = "Controls Menu") ,
	/**인벤토리 메뉴*/
	InventoryMenu UMETA(DisplayName = "Inventory Menu")
};





// /**에임 조준선 타입*/
// UENUM(BlueprintType)
// enum class EWidgetPopUp : uint8
// {
// 	None,
// 	//분할 스텍 팝업
// 	SplitStackPopUp,
// 	//확인 팝업
// 	ConfirmationPopUp,
// 	//텍스트문서 팝업
// 	TextDocumentPopUp	
// };

//
// /**에임 조준선 타입*/
// UENUM(BlueprintType)
// enum class EWidgetFilling : uint8
// {
// 	//원 형태
// 	Circle,
// 	//사각형태 형태
// 	Square
// };
//
//
// /**에임 조준선 타입*/
// UENUM(BlueprintType)
// enum class ECrossHair : uint8
// {
// 	//아무것도 아닌
// 	None UMETA(DisplayName = "None"),
// 	//원 형태
// 	Circle UMETA(DisplayName = "Circle"),
// 	//바 형태
// 	Bars UMETA(DisplayName = "Bars")	
// };


// /**오버레이 스테이트 버튼 파람*/
// USTRUCT(BlueprintType)
// struct FOverlayStateButtonParams
// {
// 	GENERATED_BODY()
// public:
// 	UPROPERTY(BlueprintReadOnly, EditAnywhere)
// 	class UUW_OverlayStateButton* Widget;
//
// 	UPROPERTY(BlueprintReadOnly, EditAnywhere)
// 	EALSOverlayState State;
// };
