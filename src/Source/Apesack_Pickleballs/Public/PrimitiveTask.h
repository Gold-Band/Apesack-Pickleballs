// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GoapSubsystem.h"
#include "PrimitiveTask.generated.h"

enum class EWorldStateOption : uint8;
class UHTNBrain;
class UWorldStateVariable;
/**
 * 
 */

UENUM(BlueprintType)
enum class ETaskCompleteType : uint8
{
	InProgress,
	Success,
	Failed
};

UCLASS(Blueprintable, BlueprintType)
class APESACK_PICKLEBALLS_API UPrimitiveTask : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	FName Name;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FWorldStateVariable Requirement;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FWorldStateVariable Effect;
	
	virtual ETaskCompleteType Run();

	virtual void Init(AActor* OwningActorRef, AActor* TargetRef = nullptr);

	UPROPERTY(BlueprintReadWrite, Transient)
	TObjectPtr<AActor> ReturnedTarget = nullptr;
	
protected:
	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="Run"))
	ETaskCompleteType RecieveRun();

	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="Init"))
	void RecieveInit(AActor* OwningActorRef, AActor* TargetRef);

	UPROPERTY(BlueprintReadWrite, Transient)
	TWeakObjectPtr<AActor> OwningActor;
};
