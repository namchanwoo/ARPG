// /**스텟 수정 값 구조체*/
// USTRUCT(BlueprintType, meta=(HasNativeMake="ActionBattle.ABStatDataClass.MakeResourceModifer"))
// struct FResourceModifier
// {
// 	GENERATED_BODY()
// public:
// 	//식별자
// 	UPROPERTY(BlueprintReadOnly, EditAnywhere)
// 	FGuid Identification;
// 	//값의 이름
// 	UPROPERTY(BlueprintReadOnly, EditAnywhere)
// 	EModifiableValues ValueName;
// 	//값의 사양
// 	UPROPERTY(BlueprintReadOnly, EditAnywhere)
// 	uint8 ValueSpecification;
// 	//값
// 	UPROPERTY(BlueprintReadOnly, EditAnywhere)
// 	float Value;
// 	//산술 연산
// 	UPROPERTY(BlueprintReadOnly, EditAnywhere)
// 	EArithmeticOperations ArithmeticOperation;
// 	//기준 값을 계산기준으로 사용할 것인가?
// 	UPROPERTY(BlueprintReadOnly, EditAnywhere)
// 	bool bUseBaseValueAsCalculationBase;
//
// public:
// 	FResourceModifier(): ValueName(), ValueSpecification(0), Value(0), ArithmeticOperation(), bUseBaseValueAsCalculationBase(false)
// 	{
// 	}
// };
//
//
// UCLASS()
// class ACTIONBATTLE_API UABStatDataClass : public UBlueprintFunctionLibrary
// {
// 	GENERATED_BODY()
//
// public:
// 	// UFUNCTION(BlueprintPure, Category = "AB_Custom", meta = (NativeMakeFunc, AdvancedDisplay="2", Normal="0,0,1", ImpactNormal="0,0,1"))
// 	UFUNCTION(BlueprintPure, Category = "AB_Custom", DisplayName="FResourceModifer 만들기", meta = (NativeMakeFunc, AdvancedDisplay="2"))
// 	static FResourceModifier MakeResourceModifer(FGuid Identification, EModifiableValues ValueName, uint8 ValueSpecification, float Value,
// 	                                             EArithmeticOperations ArithmeticOperation, bool bUseBaseValueAsCalculationBase)
// 	{
// 		FResourceModifier MakeResourceModifier;
// 		MakeResourceModifier.Identification = Identification;
// 		MakeResourceModifier.ValueName = ValueName;
// 		MakeResourceModifier.ValueSpecification = ValueSpecification;
// 		MakeResourceModifier.Value = Value;
// 		MakeResourceModifier.ArithmeticOperation = ArithmeticOperation;
// 		MakeResourceModifier.bUseBaseValueAsCalculationBase = bUseBaseValueAsCalculationBase;
// 		return MakeResourceModifier;
// 	}
// };
