#include "AI/Actions/MoveTo.h"
#include "AI/NPC/Npc.h"
#include "Kismet/KismetSystemLibrary.h"

FMoveToAction::FMoveToAction(ANpc* OwnerNpc)
{
	Owner = OwnerNpc;
	Name = "Move To";
	TargetActor = nullptr;
	bPrintDebug = true;
	Timer = Owner->RaycastInterval;
	HitResults = TArray<FHitResult>();
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
	
	
	// Are we there yet?
	const float DistanceSquared = FVector::DistSquaredXY(Owner->GetActorLocation(), TargetActor->GetActorLocation());
	if (DistanceSquared <= Owner->StartRaycastingDistanceSquared && Timer >= Owner->RaycastInterval)
	{
		Timer = 0;
		
		const FVector StartLocation = Owner->GetActorLocation();	
		const FVector EndLocation = TargetActor->GetActorLocation();
		const ETraceTypeQuery TraceChannel = UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Visibility);
		const TArray<AActor*> IgnoreActors = {Owner};
		if (UKismetSystemLibrary::LineTraceMulti(Owner->GetWorld(),StartLocation, EndLocation, TraceChannel, false, IgnoreActors, EDrawDebugTrace::None, HitResults, true))
		{
			for (const auto& It : HitResults)
			{
				if (It.GetActor() == TargetActor && It.Distance <= Owner->StopDistance)
				{
					State = EActionState::Succeeded;
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
}
