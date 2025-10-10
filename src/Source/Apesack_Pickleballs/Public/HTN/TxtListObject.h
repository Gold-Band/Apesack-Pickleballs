// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Blueprint/UserWidget.h"
#include "TxtListObject.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS()
class APESACK_PICKLEBALLS_API UTxtListObject : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

protected:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text;
};
