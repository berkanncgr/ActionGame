// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AG_GameplayAbility.h"
#include "GA_Vault.generated.h"


UCLASS()
class ACTIONGAME_API UGA_Vault : public UAG_GameplayAbility
{
	GENERATED_BODY()

public:

	UGA_Vault();
	
	/** The last chance to fail before committing, this will usually be the same as CanActivateAbility. Some abilities may need to do extra checks here if they are consuming extra stuff in CommitExecute */
	// In theory, could be done in CanActiveAbility().
	virtual bool CommitCheck(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) override;
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:

	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* VaultMontage = nullptr;

	UPROPERTY(EditDefaultsOnly)
	class UAbilityTask_PlayMontageAndWait* MontageTask = nullptr;

	FVector LocationToJump, LocationToVault, JumpOverLocation;

	UPROPERTY(EditDefaultsOnly,Category = HorizontalTrace)
	float HorizontalTraceRadius = 30.f;

	UPROPERTY(EditDefaultsOnly,Category = HorizontalTrace)
	float HorizontalTraceLenght = 500.f;
	
	UPROPERTY(EditDefaultsOnly,Category = HorizontalTrace)
	float HorizontalTraceCount = 5.f;

	UPROPERTY(EditDefaultsOnly,Category = HorizontalTrace)
	float HorizontalTraceStep = 30.f;

	UPROPERTY(EditDefaultsOnly,Category = VerticalTrace)
	float VerticalTraceStep = 30.f;

	UPROPERTY(EditDefaultsOnly,Category= ObjectTypes)
	TArray<TEnumAsByte<EObjectTypeQuery>> TraceObjectTypes;

	UPROPERTY(EditDefaultsOnly,Category= ObjectTypes)
	TArray<TEnumAsByte<ECollisionChannel>> CollisionChannelsToIgnore;

	/*UPROPERTY(EditDefaultsOnly,Category=HorizontalTrace)
	float VerticalTraceRadius = 30.f;

	UPROPERTY(EditDefaultsOnly,Category=HorizontalTrace)
	float VerticalTraceLenght = 500.f;
	
	UPROPERTY(EditDefaultsOnly,Category=HorizontalTrace)
	float VerticalTraceCount = 5.f;*/


};
