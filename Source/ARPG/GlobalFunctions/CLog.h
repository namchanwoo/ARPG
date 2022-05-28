#pragma once

#include "CoreMinimal.h"


DECLARE_LOG_CATEGORY_EXTERN(ActionRPG_LOG, Log, All); //추가


#define LOG_CALLINFO (FString(__FUNCTION__) + TEXT("(") + FString::FromInt(__LINE__) + TEXT(")"))

#define LOG_S(Verbosity) UE_LOG(ActionRPG_LOG, Verbosity, TEXT("%s"), *LOG_CALLINFO)

#define LOG(Verbosity, Format, ...) UE_LOG(ActionRPG_LOG, Verbosity, TEXT("%s %s"), *LOG_CALLINFO, *FString::Printf(Format, ##__VA_ARGS__))

#define LOG_WARNING(Format, ...) UE_LOG(ActionRPG_LOG, Warning, TEXT("%s %s"), *LOG_CALLINFO, *FString::Printf(Format, ##__VA_ARGS__))
#define LOG_ERROR(Format, ...) UE_LOG(ActionRPG_LOG, Error, TEXT("%s %s"), *LOG_CALLINFO, *FString::Printf(Format, ##__VA_ARGS__))


#define	LOG_SCREEN(Format, ...) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(Format, ##__VA_ARGS__))
#define	LOG_SCREEN_T(Time, Format, ...) GEngine->AddOnScreenDebugMessage(-1, Time, FColor::Red, FString::Printf(TEXT(Format), ##__VA_ARGS__))
 

// assert()와 거의 비슷한 용도로 사용가능한 매크로. ...에는 retval이 들어감.
#define CHECK(Expr, ...) {if(!(Expr)) {LOG(Error, TEXT("ASSERTION : %s"), TEXT("'"#Expr"'")); return __VA_ARGS__;}}