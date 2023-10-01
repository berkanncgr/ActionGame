// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "AG_GameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONGAME_API UAG_GameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
protected:
	
	/** The Array wilil be used for only for instance abilities. Otherwise we will have collisions between characters because they will use the same instance. */
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TArray<TSubclassOf<UGameplayEffect>> OngoingEffectsToJustApplyOnStart;

	/** The Array wilil be used for only for instance abilities. Otherwise we will have collisions between characters because they will use the same instance. */
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TArray<TSubclassOf<UGameplayEffect>> OngoingEffectsToRemoveOnEnd;
	
	TArray<FActiveGameplayEffectHandle> RemoveOnEndSpecHandles;
	
	UFUNCTION(BlueprintCallable,BlueprintPure)
	class AActionGameCharacter* GetActionGameCharacterFromActorInfo();
	
	
	
};
