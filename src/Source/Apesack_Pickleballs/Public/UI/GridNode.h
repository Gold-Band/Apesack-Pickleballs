// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OptionsWidget.h"
#include "Blueprint/UserWidget.h"
#include "GridNode.generated.h"

class UTask;
class UImage;
/**
 * 
 */
UCLASS()
class APESACK_PICKLEBALLS_API UGridNode : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetSelected();
	void SetUnselected();

	UGridNode* GetUpNode() const {return UpNode;}
	UGridNode* GetDownNode() const {return DownNode;}

	//FDataTableRowHandle GetDataRowHandle() const {return DataRow;}

	//template<typename T>
	void* ObjectTypeInfo = nullptr;
	
	TSoftObjectPtr<UTask> OrderTask = nullptr;
	
private:
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	TObjectPtr<UImage> Icon = nullptr;

	int Cost = 0;

	UGridNode* UpNode = nullptr;
	UGridNode* DownNode = nullptr;
	//FDataTableRowHandle DataRow;

	friend class UOptionsWidget;
};
