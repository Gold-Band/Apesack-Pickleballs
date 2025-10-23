#pragma once

#include "CoreMinimal.h"
#include "WorldState.h"
#include "Sensor.generated.h"

typedef TFunction<void(const FWorldState&)> FOnSenseCallback;

// A sensor for a WorldState. It directly controls the value of its target WorldState
UCLASS(BlueprintType, Blueprintable)
class APESACK_PICKLEBALLS_API USensor: public UObject
{
	GENERATED_BODY()
	
public:

#if WITH_EDITOR
	UFUNCTION()
	TArray<FName> GetOptions() const
	{
		if (OptionsDataTable)
		{
			return OptionsDataTable->GetRowNames();
		}
		return {};
	}
#endif
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UDataTable> OptionsDataTable;
	
	UPROPERTY(EditAnywhere, meta=(EditCondition="OptionsDataTable!=nullptr", DisplayName="World State"))
	FWorldStateMaker WorldStateMaker;

	virtual void Initialize(AActor* OwnerActor, const FOnSenseCallback& OnSenseCallback);
	
	virtual void Tick();

	bool ShouldTick() const;

	FWorldState WorldState;

	FOnSenseCallback OnSensed;

	float TickInterval;

protected:
	double LastTick;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AActor> Owner;
	
	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="Tick"))
	void ReceiveTick();

	// whatever is needed to do any check
};

USTRUCT(BlueprintType)
struct FSensorInitializer
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TSubclassOf<USensor> SensorClass;
	UPROPERTY(EditAnywhere, meta=(Units="s"))
	float TickInterval = 1.0f;
};