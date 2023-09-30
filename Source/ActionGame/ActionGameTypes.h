// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActionGameTypes.generated.h"

USTRUCT(BlueprintType)
struct FCharacterData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "GAS")
	TArray<TSubclassOf<class UGameplayEffect>> Effects;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "GAS")
	TArray<TSubclassOf<class UGameplayAbility>> Abilities;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category=Animation)
	class UCharacterAnimDataAsset* CharacterAnimDataAsset;
};

USTRUCT(BlueprintType)
struct FCharacterAnimationData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly)
	UBlendSpace* MovementBlendSpace = nullptr;

	UPROPERTY(EditDefaultsOnly)
	UAnimSequenceBase* IdleAnimationAsset = nullptr;
};

UENUM(BlueprintType)
enum class EFoot: uint8 { Right, Left  };

class ACTIONGAME_API ActionGameTypes
{
public:
	ActionGameTypes();
	~ActionGameTypes();
};
