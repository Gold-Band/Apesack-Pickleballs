// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "WorldState.generated.h"

USTRUCT(BlueprintType)
struct APESACK_PICKLEBALLS_API FWorldStateOptions : public FTableRowBase
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bDefaultValue = false;
    
};

// A world state.
USTRUCT(BlueprintType)
struct FWorldState
{
    GENERATED_BODY()

    FWorldState():Name(""), Value(false) {}
    explicit FWorldState(const FName& SetName, bool bValue): Name(SetName), Value(bValue){}
	
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName Name;

    bool GetValue() const {return Value;}

    bool operator == (const FWorldState& Other) const {
        return Other.Name == Name && Other.Value == Value;
    }

    bool operator != (const FWorldState& Other) const {
        return !(*this == Other);
    }

    FWorldState& operator ! ()
    {
        Value = !Value;
        return *this;
    }
	
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    bool Value;

    friend class USensor;
};

USTRUCT(BlueprintType)
struct FWorldStateMaker
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(GetOptions="GetOptions"))
    FName StateName;
	
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool bStateValue = false;

    FWorldState ToWorldState() const { return FWorldState(StateName, bStateValue); }
};

USTRUCT(BlueprintType)
struct FWorldStateContainer
{
    GENERATED_BODY()
    
    int Num() const {return WorldStates.Num();}
    static FWorldStateContainer FromArray(const TArray<FWorldState*>& InArray);
    static FWorldStateContainer FromArray(const TArray<FWorldStateMaker>& InArray);
    int Add(const FWorldState& Elem) { return WorldStates.Add(Elem); }
    void Remove(const FWorldState& Elem) { WorldStates.Remove(Elem); }  
    void Empty() {WorldStates.Empty();}
    void SetToMatch(const FWorldStateContainer& Other);
    void SetToMatch(const FWorldState& Other);
    void MergeUnique(const FWorldStateContainer& Other);

    const TArray<FWorldState>& Get() const {return WorldStates;}
		
    bool operator == (const FWorldStateContainer& Other) const {
        for (const auto& Element : WorldStates) // for each req
        {
            auto Match = Other.WorldStates.FindByPredicate([&Element](const FWorldState& Elem) { return Elem.Name == Element.Name; });
            if (Match && Match->Value != Element.Value) return false;
        }
        return true;
    }
		
    bool operator != (const FWorldStateContainer& Other) const {
        return !(*this == Other);
    }

    FWorldStateContainer& operator ! ()
    {
        for (auto& Element : WorldStates)
        {
            Element = !Element;
        }
        return *this;
    }
		
private:
    TArray<FWorldState> WorldStates;
};