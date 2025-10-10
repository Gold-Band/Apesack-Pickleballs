#include "HTN/WorldState.h"


FWorldStateContainer FWorldStateContainer::FromArray(const TArray<FWorldState*>& InArray)
{
	FWorldStateContainer Container;
	
	for (int i = 0; i < InArray.Num(); i++)
	{
		if (InArray[i])	Container.Add(*InArray[i]);
	}
	return Container;
}

FWorldStateContainer FWorldStateContainer::FromArray(const TArray<FWorldStateMaker>& InArray)
{
	FWorldStateContainer Container;
	for (int i = 0; i < InArray.Num(); i++)
	{
		Container.Add(InArray[i].ToWorldState());
	}
	return Container;
}

void FWorldStateContainer::SetToMatch(const FWorldStateContainer& Other)
{
	for (auto& Element : WorldStates)
	{
		const auto Found = Other.WorldStates.FindByPredicate([&Element](const FWorldState& Comp)
		{
			return Element.Name == Comp.Name;
		});
		if (Found) Element = *Found;
	}
}

void FWorldStateContainer::SetToMatch(const FWorldState& Other)
{
	auto Same = WorldStates.FindByPredicate([&Other](const FWorldState& Comp){ return Comp.Name == Other.Name;});
	if (Same) Same->Value = Other.Value;
}

void FWorldStateContainer::MergeUnique(const FWorldStateContainer& Other)
{
	for (int i = 0; i < Other.WorldStates.Num(); i++)
	{
		const auto& Element = Other.WorldStates[i];
		if (!WorldStates.FindByPredicate([&Element](const FWorldState& Comp)
		{
			return Element.Name == Comp.Name;
		}))
		{
			Add(Element);
		}
	}
}
