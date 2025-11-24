// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PoolableActor.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnPooledActorSelfDisabled, AActor*);
// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPoolableActor : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */

class APESACK_PICKLEBALLS_API IPoolableActor
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
	
public:
	
	virtual FOnPooledActorSelfDisabled& GetOnActorDisabled() = 0;
	virtual void Disable() = 0;
	virtual void Enable() = 0;
};
