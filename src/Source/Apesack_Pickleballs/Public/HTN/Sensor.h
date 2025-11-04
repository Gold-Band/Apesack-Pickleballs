#pragma once

#include "CoreMinimal.h"
#include "WorldState.h"
#include "Sensor.generated.h"

class UHTNComponent;
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

	virtual void Initialize(UHTNComponent* OwnerDomain);
	
	virtual void Tick();

	bool ShouldTick() const;

	float TickInterval;

protected:
	double LastTick;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UHTNComponent> Owner;
	
	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="Tick"))
	void ReceiveTick();

	FWorldState WorldState;
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