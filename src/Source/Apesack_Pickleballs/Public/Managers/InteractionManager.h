
#pragma once

#include "CoreMinimal.h"
#include "InteractionManager.generated.h"


struct FBuildingInfo;
class UTask;
struct FToolInfo;
struct FClassInfo;
class APlot;
class UGridNode;
class AInteractionMenuActor;
class UWidgetComponent;
class UOptionsWidget;
class ANpcCharacter;


UCLASS()
class UInteractionManager: public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	UInteractionManager();
	
	static UInteractionManager* Get(const UObject* WorldContextObject);

	void SetNpcClass(ANpcCharacter* NpcActor, FClassInfo* ClassInfo);

	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

	// **** I want NPC interaction to be handled by the manager ***** //
	UFUNCTION(BlueprintCallable)
	void StartInteraction(AActor* Actor);
	
	UFUNCTION(BlueprintCallable)
	void EndInteraction();
	
	UFUNCTION(BlueprintCallable)
	void CycleOptions(const int Direction);

	UFUNCTION(BlueprintCallable)
	void ConfirmOption();
	
	const TArray<FClassInfo*>& GetAllClasses() const {return AllClasses;}
	const TArray<FBuildingInfo*>& GetAllBuildings() const {return AllBuildings;}
	//const TArray<FDataTableRowHandle*>& GetAllBuildings() const {return AllBuildingHandles;}
	
	
	TSoftObjectPtr<UTask> GetUpgradeTaskForTool(const FToolInfo* ToolInfo);
	TSoftObjectPtr<UTask> GetPromotionTaskForClass(const FClassInfo* ClassInfo);
	
	
private:
	bool bIsInteracting = false;
	
	TArray<FClassInfo*> AllClasses;
	TArray<FBuildingInfo*> AllBuildings;
	//TArray<FDataTableRowHandle*> AllBuildingHandles; 
	
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
	APlot* PlotWeAreInteractingWith = nullptr;
	
};
