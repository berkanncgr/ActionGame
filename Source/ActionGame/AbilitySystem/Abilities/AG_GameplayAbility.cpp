// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AG_GameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "ActionGameCharacter.h"

void UAG_GameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UAbilitySystemComponent* AbilityComponent = ActorInfo->AbilitySystemComponent.Get();
	if(!AbilityComponent) return;

	FGameplayEffectContextHandle EffectContext = AbilityComponent->MakeEffectContext();
	for(auto GameplayEffect : OngoingEffectsToJustApplyOnStart)
	{
		if(!GameplayEffect.Get()) continue;
		
		FGameplayEffectSpecHandle SpecHandle = AbilityComponent->MakeOutgoingSpec(GameplayEffect,1,EffectContext);
		if(!SpecHandle.IsValid()) continue;

		FActiveGameplayEffectHandle ActiveGEHandle = ActorInfo->AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		if(!ActiveGEHandle.WasSuccessfullyApplied())
		{
			UE_LOG(LogTemp,Error,TEXT("Ability %s fFAILED to apply startup effect %s"),*GetName(), *GetNameSafe(GameplayEffect));
			//ABILITY_LOG
		}
	}

	if(!IsInstantiated()) return;

	for (auto GameplayEffect : OngoingEffectsToRemoveOnEnd)
	{
		if(!GameplayEffect.Get()) continue;
		FGameplayEffectSpecHandle SpecHandle = AbilityComponent->MakeOutgoingSpec(GameplayEffect,1,EffectContext);
		if(!SpecHandle.IsValid()) continue;

		FActiveGameplayEffectHandle ActiveGEHandle = AbilityComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

		const bool bIsSuccess = ActiveGEHandle.WasSuccessfullyApplied();
		if(!bIsSuccess)
		{
			UE_LOG(LogTemp,Error,TEXT("Ability %s fFAILED to apply runtime effect %s"),*GetName(), *GetNameSafe(GameplayEffect));
			continue;
		}
		RemoveOnEndSpecHandles.Add(ActiveGEHandle);
	}
}


void UAG_GameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if(!IsInstantiated()) return;
	
	UAbilitySystemComponent* AbilityComponent = ActorInfo->AbilitySystemComponent.Get();
	if(!AbilityComponent) return;

	for(FActiveGameplayEffectHandle ActiveEffectHandle : RemoveOnEndSpecHandles)
	{
		if(!ActiveEffectHandle.IsValid()) continue;
		AbilityComponent->RemoveActiveGameplayEffect(ActiveEffectHandle);
	}
	
	RemoveOnEndSpecHandles.Empty();
}



AActionGameCharacter* UAG_GameplayAbility::GetActionGameCharacterFromActorInfo()
{
	return Cast<AActionGameCharacter>(GetAvatarActorFromActorInfo());
}
