// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TxtListObject.h"
#include "BtnListObject.generated.h"

class UButton;
/**
 * 
 */
UCLASS()
class APESACK_PICKLEBALLS_API UBtnListObject : public UTxtListObject
{
	GENERATED_BODY()

	protected:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Action;
	
};
