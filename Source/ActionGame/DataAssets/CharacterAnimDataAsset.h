// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ActionGameTypes.h"
#include "CharacterAnimDataAsset.generated.h"

UCLASS()
class ACTIONGAME_API UCharacterAnimDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FCharacterAnimationData CharacterAnimationData;
};
