#include "AI/Actions/TargetFarthestTower.h"

#include "AI/NPC/Npc.h"
#include "Managers/BuildingsManager.h"

FTargetFarthestTowerAction::FTargetFarthestTowerAction(ANpc* OwnerNpc)
{
	Name = "Target Farthest Tower";
	Owner = OwnerNpc;
	bPrintDebug = false;
	FarthestTower = nullptr;
	bIsFirstCall = true;
}

bool FTargetFarthestTowerAction::IsExecutable() const
{
	// do towers exist?
	const UBuildingsManager* BuildingsManager = UBuildingsManager::Get(Owner->GetWorld());
	if (bPrintDebug) UE_LOG(LogTemp, Warning, TEXT("tower exists? %s"), FarthestTower? TEXT("yes"): TEXT("no"));
	return BuildingsManager->TowersExist();
}

void FTargetFarthestTowerAction::Execute(float DeltaTime)
{
	if (bIsFirstCall)
	{
		bIsFirstCall = false;
		const UWorld* World = Owner->GetWorld();
		UBuildingsManager* BuildingsManager = UBuildingsManager::Get(World);
		FarthestTower= Owner->TargetActor = BuildingsManager->GetFarthestBuilding(EBuildingType::Tower, Owner->MainSide);
	}
	
	if (FarthestTower != nullptr) State = EActionState::Succeeded;
	else State = EActionState::Failed;
}

void FTargetFarthestTowerAction::Reset()
{
	FAction::Reset();
	bIsFirstCall = true;
	FarthestTower = nullptr;
}
