
#pragma once

#include "CoreMinimal.h"
#include "InteractionManager.generated.h"

class ADefaultGameMode;
struct FBuildingInfo;
class UTask;
struct FToolInfo;
struct FClassInfo;
class APlot;
class UGridNode;
class AInteractionMenuActor;
class UWidgetComponent;
class UOptionsWidget;
class ANpcFriendly;


UCLASS()
class UInteractionManager: public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	UInteractionManager();
	
	static UInteractionManager* Get(const UObject* WorldContextObject);

	void SetNpcClass(ANpcFriendly* NpcActor, FClassInfo* ClassInfo);

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
	
	TSoftObjectPtr<UTask> GetUpgradeTaskForTool(const FToolInfo* ToolInfo);
	TSoftObjectPtr<UTask> GetPromotionTaskForClass(const FClassInfo* ClassInfo);
	
private:
	bool bIsInteracting = false;

	const ADefaultGameMode* GameMode = nullptr;
	
	TArray<FClassInfo*> AllClasses;
	TArray<FBuildingInfo*> AllBuildings;
	
	UGridNode* SelectedOptionNode = nullptr;
	
	UPROPERTY()
	TObjectPtr<AInteractionMenuActor> InteractionMenuActor;

	UPROPERTY()
	TSoftObjectPtr<UTask> WaitTask;

	UPROPERTY()
	TSoftObjectPtr<UTask> EmptyTask;

	ANpcFriendly* CharacterWeAreInteractingWith = nullptr;
	APlot* PlotWeAreInteractingWith = nullptr;
	
};
