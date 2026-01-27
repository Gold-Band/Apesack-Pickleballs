#pragma once

#include "CoreMinimal.h"
#include "NpcName.generated.h"

USTRUCT(BlueprintType)
struct APESACK_PICKLEBALLS_API FNpcName: public FTableRowBase
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString SampleName;
	
};
