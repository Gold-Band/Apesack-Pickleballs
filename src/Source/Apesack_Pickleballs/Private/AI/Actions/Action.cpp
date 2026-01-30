#include "AI/Actions/Action.h"

FAction::FAction(const FString& ActionName) : State(EActionState::InProgress), Name(ActionName)
{
}

FAction::~FAction()
{
	ConditionDelegate.Unbind();
	ExecutionDelegate.Unbind();
	ResetDelegate.Unbind();
}

void FAction::Reset()
{
	State = EActionState::InProgress;
	ResetDelegate.ExecuteIfBound();
}

bool FAction::CanExecute() const
{
	if (ConditionDelegate.IsBound()) return ConditionDelegate.Execute();
	return true;
}
