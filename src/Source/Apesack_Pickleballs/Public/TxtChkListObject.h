// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TxtListObject.h"
#include "TxtChkListObject.generated.h"

class UCheckBox;
/**
 * 
 */
UCLASS()
class APESACK_PICKLEBALLS_API UTxtChkListObject : public UTxtListObject
{
	GENERATED_BODY()

	protected:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	
	private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCheckBox> State;
	
};
