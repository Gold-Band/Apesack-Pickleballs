// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TaskResult.h"
#include "WorldState.h"
#include "Task.generated.h"

typedef TFunction<void(const FTaskResult&)> FTaskCallback;
UCLASS(Blueprintable, BlueprintType)
class APESACK_PICKLEBALLS_API UPrimitiveTask : public UObject
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere)
	FString Name;

#if WITH_EDITOR
	UFUNCTION()
	TArray<FName> GetOptions() const
	{
		if (OptionsDataTable)
		{
			return OptionsDataTable->GetRowNames();
		}
		return {};
	}
#endif
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UDataTable> OptionsDataTable;
	
	UPROPERTY(EditAnywhere, meta=(EditCondition="OptionsDataTable!=nullptr"))
	TArray<FWorldStateMaker> Requirement;

	UPROPERTY(EditAnywhere, meta=(EditCondition="OptionsDataTable!=nullptr"))
	TArray<FWorldStateMaker> Effect;


	UFUNCTION(BlueprintCallable)
	void Run();
	
	UFUNCTION(BlueprintCallable)
	void Pause();

	void ForceComplete();

	UFUNCTION(BlueprintCallable)
	virtual void OnTaskCompleted(FTaskResult ReturnedObjects);
	
	bool ShouldTick() const {return bShouldTick;} 
	
	virtual void Tick(float DeltaTime);

	virtual void Initialize(AActor* InstigatorActor, const FTaskCallback& OnCompleteCallback, const FTaskResult& PreviousTaskResult);

	int GetSubsystemId() const {return SubsystemId;}
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bPrintStatusInLog = false;
	
protected:
	
	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="Tick"))
	void ReceiveTick(float DeltaTime);

	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="Initialize"))
	void ReceiveInitialize(AActor* InstigatorActor, const FTaskResult& PreviousTaskResult);

	
	UPROPERTY(BlueprintReadOnly)
	FWorldStateContainer EffectContainer;
	
	UPROPERTY()
	TObjectPtr<AActor> Instigator;
	
private:
	
	FTaskCallback OnTaskCompleteCallback;
	
	int SubsystemId = -1;
	bool bShouldTick;
};

USTRUCT()
struct FMethod
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	FName Name = "Method";
	
	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<UPrimitiveTask>> Steps;
};	

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class APESACK_PICKLEBALLS_API UTask : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	FName Name = "Task";
	
	UPROPERTY(EditAnywhere)
	TArray<FMethod> Methods;
};

UCLASS(Blueprintable)
class APESACK_PICKLEBALLS_API UTaskSubsystem: public UGameInstanceSubsystem, public FTickableGameObject
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	
	UFUNCTION(BlueprintPure)
	static UTaskSubsystem* Get(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure)
	AGameModeBase* GetGameMode(const UObject* WorldContextObject);

	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;
	virtual ETickableTickType GetTickableTickType() const override;
	virtual bool IsTickableWhenPaused() const override;
	virtual bool IsTickableInEditor() const override;
	int RegisterPrimitiveTask(UPrimitiveTask* Task);
	bool UnRegisterPrimitiveTask(const UPrimitiveTask* Task);
	
private:
	bool bCleanupRunningTasks;
	uint32 IdIndex;
	TArray<TWeakObjectPtr<UPrimitiveTask>> RunningPrimitiveTasks;

};