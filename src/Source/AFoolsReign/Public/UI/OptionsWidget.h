// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OptionsWidget.generated.h"

class UImage;
class UGridNode;
class UVerticalBox;

template<typename T>
struct TOptionsData
{
	//explicit TOptionsData(const TObjectPtr<UTexture2D> OptionIcon, const int OptionCost, const TSoftObjectPtr<UTaskAsset>& OptionOrderTask, const T* Info): Icon(OptionIcon), Cost(OptionCost), OrderTask(OptionOrderTask), TypeInfo(Info) {}
	
	const TObjectPtr<UTexture2D> Icon;
	const int Cost = 0;
	//const TSoftObjectPtr<UTaskAsset> OrderTask;
	const T* TypeInfo;
};


UCLASS()
class AFOOLSREIGN_API UOptionsWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	template<typename T>
	void Setup(const TArray<TOptionsData<T>>& Data);

	void Reset() const;

	UGridNode* GetNode(const int Index);

private:
	
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	TObjectPtr<UVerticalBox> Options;

	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess=true))
	TSubclassOf<UUserWidget> OptionNodeClass;
	
	// Node pool
	UPROPERTY()
	TArray<TObjectPtr<UGridNode>> OptionNodes;
	 
	int SelectedOptionIndex = 0;
};