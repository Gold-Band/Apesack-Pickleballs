
#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NpcManager.generated.h"


class UGridNode;
class AInteractionMenuActor;
class UWidgetComponent;
class UOptionsWidget;
class ANpcCharacter;

USTRUCT(BlueprintType)
struct FRankInfo : public FTableRowBase // row name is rank name
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UTexture2D> RankIcon;
};


USTRUCT(BlueprintType)
struct FToolInfo : public FTableRowBase // row name is tool name
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag ToolTag;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FDataTableRowHandle NextTool;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UTexture2D> ToolIcon;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UTexture2D> ToolTexture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int PurchaseCost = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int ToolDamage = 1;
};


USTRUCT(BlueprintType)
struct FClassInfo : public FTableRowBase // row name is class name
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSoftObjectPtr<class UTask>> ClassTasks;

	// not used
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag ClassTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FDataTableRowHandle BaseTool;
};


UCLASS()
class UNpcManager: public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	UNpcManager();
	
	static UNpcManager* Get(const UObject* WorldContextObject);

	void SetNpcClass(ANpcCharacter* NpcActor, FClassInfo* ClassInfo);

	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

	// **** I want NPC interaction to be handled by the manager ***** //
	UFUNCTION(BlueprintCallable)
	void StartNpcInteraction(ANpcCharacter* NpcActor);
	
	UFUNCTION(BlueprintCallable)
	void EndNpcInteraction();

	UFUNCTION(BlueprintCallable)
	void CycleOptions(const int Direction);

	UFUNCTION(BlueprintCallable)
	void ConfirmOption();
	
	const TArray<FClassInfo*>& GetAllClasses() const {return AllClasses;}
	
	TSoftObjectPtr<UTask> GetUpgradeTaskForTool(const FToolInfo* ToolInfo);
	TSoftObjectPtr<UTask> GetPromotionTaskForClass(const FClassInfo* ClassInfo);
	
	
private:
	bool bIsInteractingWithNpc = false;
	
	TArray<FClassInfo*> AllClasses;

	UGridNode* SelectedOptionNode = nullptr;
	
	UPROPERTY()
	TObjectPtr<AInteractionMenuActor> InteractionMenuActor;

	UPROPERTY()
	TSoftObjectPtr<UTask> WaitTask;

	UPROPERTY()
	TSoftObjectPtr<UTask> EmptyTask;
	
	// ***** temporary measure ****** // 
	UPROPERTY()
	TSoftObjectPtr<UTask> PromoteMeleeTask;

	UPROPERTY()
	TSoftObjectPtr<UTask> PromoteRangedTask;

	UPROPERTY()
	TSoftObjectPtr<UTask> PromoteBuilderTask;

	
	ANpcCharacter* CharacterWeAreInteractingWith = nullptr;
	
};
