// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActionGameTypes.h"
#include "Components/ActorComponent.h"
#include "FootStepsComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTIONGAME_API UFootStepsComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	
	UFootStepsComponent();

	void HandleFootStep(EFoot Foot, USkeletalMeshComponent* SkeletalMesh);

protected:
	
	virtual void BeginPlay() override;



		
};
