// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/GA_Vault.h"
#include "ActionGameCharacter.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Kismet/KismetSystemLibrary.h"

UGA_Vault::UGA_Vault()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

bool UGA_Vault::CommitCheck(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, FGameplayTagContainer* OptionalRelevantTags)
{
	if(!Super::CommitCheck(Handle, ActorInfo, ActivationInfo, OptionalRelevantTags)) return false;

	AActionGameCharacter* Character = GetActionGameCharacterFromActorInfo();
	if(!Character) return false;

	const FVector StartLocation = Character->GetActorLocation();
	const FVector ForwardVector = Character->GetActorForwardVector();
	const FVector UpVector = Character->GetActorUpVector();

	static const IConsoleVariable* CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("ShowDebugTraversal"));
	const bool bShowDebugTraversal = CVar && CVar->GetInt() > 0;
	EDrawDebugTrace::Type DebugDrawType = bShowDebugTraversal ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;

	float MaxJumpDistance = HorizontalTraceLenght;
	TArray<AActor*> ActorsToIgnore = {Character};
	bool bIsLocationToJumpSet = false; int32 LocationToJumpIndex = INDEX_NONE;
	FHitResult HitResult; int i = 0;
	
	for (; i< HorizontalTraceCount; ++i)
	{
		const FVector TraceStartLocation = StartLocation + (i * UpVector * HorizontalTraceStep);
		const FVector TraceEndLocation = TraceStartLocation + ForwardVector * HorizontalTraceLenght;

		bool bTraceResult = UKismetSystemLibrary::SphereTraceSingleForObjects(GetWorld(),TraceStartLocation,TraceEndLocation,HorizontalTraceRadius,TraceObjectTypes,true,ActorsToIgnore,DebugDrawType,HitResult,true);

		if(!bTraceResult)
		{
			if(LocationToJumpIndex != INDEX_NONE) break;
			continue;
		}

		if(LocationToJumpIndex == INDEX_NONE && (i < HorizontalTraceCount - 1))
		{
			LocationToJumpIndex = i;
			LocationToJump = HitResult.Location;
			bIsLocationToJumpSet = true;
		}

		// We are lookint at the wall and try to jump:
		else if(LocationToJumpIndex == (i-1))
		{
			MaxJumpDistance = FVector::Dist2D(HitResult.Location,TraceStartLocation);
			break;
		}
	}

	if(LocationToJumpIndex == INDEX_NONE) return false;

	const float DistanceToJump = FVector::Dist2D(StartLocation,LocationToJump);
	const float MaxVerticalTraceDistance = MaxJumpDistance - DistanceToJump;
	if( MaxVerticalTraceDistance < 0 ) return false;
	if(i == HorizontalTraceCount) i = HorizontalTraceCount -1;

	bool bIsJumpOverLocationSet = false;
	const float VerticalTraceLenght = FMath::Abs(LocationToJump.Z - (StartLocation + i * UpVector * HorizontalTraceStep).Z);
	FVector VerticalStartLocation = LocationToJump + UpVector * VerticalTraceLenght;
	const float VerticalTraceCount = MaxVerticalTraceDistance / VerticalTraceStep;
	i = 0;
	
	return true;

	/*
	for(;i<VerticalTraceCount;++i)
	{
		
	}*/
	

	
	
}

void UGA_Vault::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UGA_Vault::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
