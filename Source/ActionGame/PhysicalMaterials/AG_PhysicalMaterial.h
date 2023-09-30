// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "AG_PhysicalMaterial.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class ACTIONGAME_API UAG_PhysicalMaterial : public UPhysicalMaterial
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category=Sounds)
	class USoundBase* FootstepSound = nullptr;	
	
	
};
