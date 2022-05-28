
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interface_Rotation.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInterface_Rotation : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ARPG_API IInterface_Rotation
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	/**
	 * @brief 캐릭터의 회전을 가져옵니다
	 * @return 캐릭터의 회전 값
	 */
	virtual FRotator I_GetCharacterRotation()=0;

	/**
	 * @brief 회전을 시작합니다
	 * @param InRotationInterpSpeed 회전 할때 인텁스피드 값 
	 */
	virtual void I_StartRotating(float InRotationInterpSpeed)=0;

	/**
	 * @brief 회전을 중지합니다
	 */
	virtual void I_StopRotating()=0;
};
