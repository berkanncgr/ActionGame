// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/GA_Jump.h"

#include "AbilitySystemComponent.h"
#include "GameFramework/Character.h"


UGA_Jump::UGA_Jump()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::NonInstanced;
}

bool UGA_Jump::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if(!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags)) return false;

	// L->21 -  M->5:06
	ACharacter * Character = CastChecked<ACharacter>(ActorInfo->AvatarActor.Get(),ECastCheckedType::NullAllowed);
	if(!Character) return false;
	return Character->CanJump();
}

void UGA_Jump::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	//Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo)) return;
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo)) return;
	
	ACharacter * Character = CastChecked<ACharacter>(ActorInfo->AvatarActor.Get());
	if(!Character) return;
	
	Character->Jump();

	UAbilitySystemComponent* AbilityComponent = ActorInfo->AbilitySystemComponent.Get();
	if(!AbilityComponent) return;

	FGameplayEffectContextHandle EffectContext = AbilityComponent->MakeEffectContext();
	FGameplayEffectSpecHandle SpecHandle = AbilityComponent->MakeOutgoingSpec(JumpEffect,1,EffectContext);

	if(!SpecHandle.IsValid()) return;

	FActiveGameplayEffectHandle ActiveGameplayEffectHandle =  AbilityComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

	if(!ActiveGameplayEffectHandle.WasSuccessfullyApplied())
	{
		UE_LOG(LogTemp,Error,TEXT("Failed To Apply Jump Effect: %s"),*GetNameSafe(Character));
		return;
	}
}
