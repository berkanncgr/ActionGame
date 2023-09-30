// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActionGameTypes.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_Step.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONGAME_API UAnimNotify_Step : public UAnimNotify
{
	GENERATED_BODY()

public:

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	EFoot Foot;

	class ICharacterInterface* Character;                                                                             
};
