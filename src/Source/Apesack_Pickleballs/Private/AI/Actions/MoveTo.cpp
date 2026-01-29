#include "AI/Actions/MoveTo.h"
#include "AI/NPC/Npc.h"
#include "Buildings/Wall.h"
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
	if (Timer >= Owner->RaycastInterval && (ProbeDistanceCondition() || bUseLineOfSight))
	{
		Timer = 0;
		if (LineTraceMulti(HitResults))
		{
			if (Owner->bPrintDebug_MoveTo)
			{
				FString HitActors;
				for (auto It = HitResults.CreateConstIterator(); It; ++It)
				{
					HitActors.Append(FString::Printf(TEXT(", %s"), *It->GetActor()->GetActorNameOrLabel()));
				}
				UE_LOG(LogTemp, Warning, TEXT("Num actors in sight = %i%s"), HitResults.Num(), *HitActors);
				UE_LOG(LogTemp, Warning, TEXT("Target = %s"), *TargetActor->GetActorNameOrLabel());
			}
			for (const auto& It : HitResults)
			{
				AActor* HitActor = It.GetActor();
				
				if (HitActor == TargetActor)
				{
					if (It.Distance <= Owner->StopDistance)
					{
						State = EActionState::Succeeded;
						return;
					}
				}
				else if (bUseLineOfSight && Cast<AWall>(HitActor) != nullptr)
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
	return UKismetSystemLibrary::LineTraceMulti(
		Owner->GetWorld(), // world
		Owner->GetActorLocation(), // start 
		TargetActor->GetActorLocation(), // end 
		UEngineTypes::ConvertToTraceType(ECC_Visibility), // channel
		false,
		TArray<AActor*>{Owner}, // ignore 
		Owner->bPrintDebug_MoveTo? EDrawDebugTrace::ForOneFrame : EDrawDebugTrace::None, // debug
		OutHits,
		true);
}

bool FMoveToAction::ProbeDistanceCondition() const 
{
	const float DistanceSquared = FVector::DistSquaredXY(Owner->GetActorLocation(), TargetActor->GetActorLocation());
	return DistanceSquared <= Owner->StartRaycastingDistanceSquared;
}
