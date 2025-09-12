#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractableLegacy.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInteractableLegacy : public UInterface {
	GENERATED_BODY()
};

class APESACK_PICKLEBALLS_API IInteractableLegacy {
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void Interact() = 0;
	virtual int GetCost();
};
