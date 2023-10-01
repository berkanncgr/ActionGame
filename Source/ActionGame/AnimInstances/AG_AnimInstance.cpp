// Fill out your copyright notice in the Description page of Project Settings.


#include "AG_AnimInstance.h"
#include "ActionGameCharacter.h"
#include "DataAssets/CharacterAnimDataAsset.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

UAG_AnimInstance::UAG_AnimInstance()
{
	
}

void UAG_AnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	
	Character = Cast<AActionGameCharacter>(GetOwningActor());
	MovementComponent = Cast<UCharacterMovementComponent>(GetOwningActor()->GetComponentByClass(UCharacterMovementComponent::StaticClass()));
}

void UAG_AnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	
	if(!Character) return;
	if(!MovementComponent) return;
	
	FVector UnrotateVector = UKismetMathLibrary::LessLess_VectorRotator(Character->GetVelocity(),Character->GetActorRotation());
	Side = UnrotateVector.Y;
	Forward = UnrotateVector.X;
	
	Velocity = MovementComponent->Velocity;
	GroundSpeed = UKismetMathLibrary::VSizeXY(Velocity);
	bIsFalling = MovementComponent->IsFalling();
	bShouldMove = GroundSpeed > 3 && MovementComponent->GetCurrentAcceleration() != FVector::ZeroVector;
	bIsCrouching = MovementComponent->IsCrouching();
}

UBlendSpace* UAG_AnimInstance::GetLocomotionBlendSpace() const
{
	if(!Character)
	{
		UE_LOG(LogTemp,Error,TEXT("Character is NULL - UAG_AnimInstance::GetLocomotionBlendSpace"))
		return DefaultCharacterAnimDataAsset->CharacterAnimationData.MovementBlendSpace;
	}

	FCharacterData Data = Character->GetCharacterData();
	if(!Data.CharacterAnimDataAsset)
	{
		UE_LOG(LogTemp,Error,TEXT("Data.CharacterAnimDataAsset is NULL - UAG_AnimInstance::GetLocomotionBlendSpace"))
		return DefaultCharacterAnimDataAsset->CharacterAnimationData.MovementBlendSpace;
	}

	return Data.CharacterAnimDataAsset->CharacterAnimationData.MovementBlendSpace;
}

UAnimSequenceBase* UAG_AnimInstance::GetIdleAnimation() const
{
	if(!Character)
	{
		UE_LOG(LogTemp,Error,TEXT("Character is NULL - UAG_AnimInstance::GetLocomotionBlendSpace"))
		return DefaultCharacterAnimDataAsset->CharacterAnimationData.IdleAnimationAsset;
	}

	FCharacterData Data = Character->GetCharacterData();
	if(!Data.CharacterAnimDataAsset)
	{
		UE_LOG(LogTemp,Error,TEXT("Data.CharacterAnimDataAsset is NULL - UAG_AnimInstance::GetLocomotionBlendSpace"))
		return DefaultCharacterAnimDataAsset->CharacterAnimationData.IdleAnimationAsset;
	}

	return Data.CharacterAnimDataAsset->CharacterAnimationData.IdleAnimationAsset;
}

UBlendSpace* UAG_AnimInstance::GetCrouchLocomotionBlendSpace() const
{
	if(!Character)
	{
		UE_LOG(LogTemp,Error,TEXT("Character is NULL - UAG_AnimInstance::GetLocomotionBlendSpace"))
		return DefaultCharacterAnimDataAsset->CharacterAnimationData.CrouchMovementBlendSpace;
	}

	FCharacterData Data = Character->GetCharacterData();
	if(!Data.CharacterAnimDataAsset)
	{
		UE_LOG(LogTemp,Error,TEXT("Data.CharacterAnimDataAsset is NULL - UAG_AnimInstance::GetLocomotionBlendSpace"))
		return DefaultCharacterAnimDataAsset->CharacterAnimationData.CrouchMovementBlendSpace;
	}

	return Data.CharacterAnimDataAsset->CharacterAnimationData.CrouchMovementBlendSpace;
}

UAnimSequenceBase* UAG_AnimInstance::GetCrouchIdleAnimation() const
{
	if(!Character)
	{
		UE_LOG(LogTemp,Error,TEXT("Character is NULL - UAG_AnimInstance::GetLocomotionBlendSpace"))
		return DefaultCharacterAnimDataAsset->CharacterAnimationData.CrouchIdleAnimationAsset;
	}

	FCharacterData Data = Character->GetCharacterData();
	if(!Data.CharacterAnimDataAsset)
	{
		UE_LOG(LogTemp,Error,TEXT("Data.CharacterAnimDataAsset is NULL - UAG_AnimInstance::GetLocomotionBlendSpace"))
		return DefaultCharacterAnimDataAsset->CharacterAnimationData.CrouchIdleAnimationAsset;
	}

	return Data.CharacterAnimDataAsset->CharacterAnimationData.CrouchIdleAnimationAsset;
}
