// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TxtListObject.h"
#include "BtnListObject.generated.h"

class UBorder;
class UButton;
/**
 * 
 */
UCLASS()
class AFOOLSREIGN_API UBtnListObject : public UTxtListObject
{
	GENERATED_BODY()
	
protected:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UBorder> Border;
	
	UFUNCTION(BlueprintCallable)
	void OnButtonClicked();
	
private:
	TFunction<void()> ButtonFunction;
	
	UPROPERTY()
	TObjectPtr<UUserWidget> Parent;
	
	int Cost;
	
	bool bCloseOnClicked;
};
