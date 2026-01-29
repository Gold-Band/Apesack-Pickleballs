#include "AI/Actions/MoveTo.h"
#include "AI/NPC/Npc.h"
#include "Kismet/KismetSystemLibrary.h"

FMoveToAction::FMoveToAction(ANpc* OwnerNpc)
{
	Owner = OwnerNpc;
	Name = "Move To";
	TargetActor = nullptr;
	Timer = Owner->RaycastInterval;
	HitResults = TArray<FHitResult>();
	bUseLineOfSight = false;
}

bool FMoveToAction::IsExecutable() const
{
	return Owner->bCanMove;
}

void FMoveToAction::Execute(float DeltaTime)
{
	// Is destination still valid?
	if (TargetActor == nullptr)
	{
		TargetActor = Owner->TargetActor;
		if (TargetActor == nullptr)
		{
			State = EActionState::Failed;
			return;
		}
	}
	
	// 
	// check line of sight / Are we there yet?
	if (Timer >= Owner->RaycastInterval)
	{
		Timer = 0;
		if ((bUseLineOfSight || ProbeDistanceCondition()) && LineTraceMulti(HitResults))
		{
			for (const auto& It : HitResults)
			{
				if (Owner->bPrintDebug_MoveTo) UE_LOG(LogTemp, Warning, TEXT("Dist=%f | Other=%s | Target=%s"), It.Distance, *It.GetActor()->GetActorNameOrLabel(), *TargetActor->GetActorNameOrLabel());
				if (It.GetActor() == TargetActor)
				{
					if (It.Distance <= Owner->StopDistance)
					{
						State = EActionState::Succeeded;
						return;
					}
				}
				else if (bUseLineOfSight)
				{
					if (Owner->bPrintDebug_MoveTo) UE_LOG(LogTemp, Warning, TEXT("no line of sight"));
					State = EActionState::Failed;
					return;
				}
			}
		}
	}
	Timer+=DeltaTime;
	
	
	// Move
	const float Direction = FVector::DotProduct(TargetActor->GetActorLocation() - Owner->GetActorLocation(), Owner->GetActorForwardVector()) > 0 ? 1.0f : -1.0f;
	Owner->MoveForwardScaled(Direction);
}

void FMoveToAction::Reset()
{
	FAction::Reset();
	Timer = Owner->RaycastInterval;
	TargetActor = nullptr;
}

bool FMoveToAction::LineTraceMulti(TArray<FHitResult>& OutHits) const
{
	const FVector StartLocation = Owner->GetActorLocation();	
	const FVector EndLocation = TargetActor->GetActorLocation();
	const ETraceTypeQuery TraceChannel = bUseLineOfSight? UEngineTypes::ConvertToTraceType(ECC_Destructible) : UEngineTypes::ConvertToTraceType(ECC_Visibility);
	const TArray<AActor*> IgnoreActors = {Owner};
	return UKismetSystemLibrary::LineTraceMulti(Owner->GetWorld(),StartLocation, EndLocation, TraceChannel, false, IgnoreActors, EDrawDebugTrace::ForOneFrame, OutHits, true);
}

bool FMoveToAction::ProbeDistanceCondition() const 
{
	const float DistanceSquared = FVector::DistSquaredXY(Owner->GetActorLocation(), TargetActor->GetActorLocation());
	return DistanceSquared <= Owner->StartRaycastingDistanceSquared;
}
