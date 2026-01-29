#include "AI/Actions/TargetNearestAttackable.h"
#include "AI/NPC/Npc.h"
#include "Managers/NpcManager.h"

FTargetNearestAttackableAction::FTargetNearestAttackableAction(ANpc* OwnerNpc) : 
	Owner(OwnerNpc),
	bIsFirstCall(true),
	World(nullptr),
	NpcManager(nullptr),
	BuildingsManager(nullptr),
	NearestNpc(nullptr),
	NearestBuilding(nullptr),
	NearestAttackable(nullptr),
	NpcDistance(UE_MAX_FLT),
	BuildingDistance(UE_MAX_FLT)
{
	Name = "Target Attackable";
}

bool FTargetNearestAttackableAction::IsExecutable() const
{
	return true;
}

void FTargetNearestAttackableAction::Execute(float DeltaTime)
{
	if (bIsFirstCall)
	{
		bIsFirstCall = false;	
		
		if (!World) World = Owner->GetWorld();
		if (!NpcManager) NpcManager = UNpcManager::Get(World);
		if (!BuildingsManager) BuildingsManager = UBuildingsManager::Get(World);
	}
	
	// reset
	NpcDistance = UE_MAX_FLT;
	BuildingDistance = UE_MAX_FLT;
	Location = Owner->GetActorLocation();
	
	// get nearest building and npc
	NearestNpc = NpcManager->FindNearestNpc(Location, ENpcSearchOption::AnyFriendly);
	NearestBuilding = BuildingsManager->GetFarthestBuilding(EBuildingType::Wall, Owner->MainSide);
	
	if (NearestNpc) NpcDistance = FVector::DistSquared2D(Location, NearestNpc->GetActorLocation());
	if (NearestBuilding) BuildingDistance = FVector::DistSquared2D(Location, NearestBuilding->GetActorLocation());
	
	// target the closest
	NearestAttackable = Owner->TargetActor = NpcDistance > BuildingDistance ? NearestBuilding : NearestNpc;
	if (Owner->bPrintDebug_TargetNearestAny) UE_LOG(LogTemp, Warning, TEXT("NearestAttackable=%s | Owner->TargetActor=%s"), NearestAttackable? *NearestAttackable->GetName(): TEXT("Null"), Owner->TargetActor? *Owner->TargetActor->GetName() : TEXT("Null"));
	
	if (NearestAttackable != nullptr) State = EActionState::Succeeded;
	else State = EActionState::Failed;
}

void FTargetNearestAttackableAction::Reset()
{
	FAction::Reset();
	bIsFirstCall = true;
	NearestAttackable = nullptr;
}