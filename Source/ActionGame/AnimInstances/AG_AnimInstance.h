// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "AG_AnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONGAME_API UAG_AnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:

	UAG_AnimInstance();
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite)
	class UCharacterMovementComponent* MovementComponent;

	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	UBlendSpace* GetLocomotionBlendSpace() const;

	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	UAnimSequenceBase* GetIdleAnimation() const;

	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	UBlendSpace* GetCrouchLocomotionBlendSpace() const;

	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	UAnimSequenceBase* GetCrouchIdleAnimation() const;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category=Animaiton)
	class UCharacterAnimDataAsset* DefaultCharacterAnimDataAsset;
	
	UPROPERTY(BlueprintReadWrite)
	class AActionGameCharacter* Character;
	
	UPROPERTY(BlueprintReadWrite)
	float Side;

	UPROPERTY(BlueprintReadWrite)
	float Forward;

	UPROPERTY(BlueprintReadWrite)
	FVector Velocity;

	UPROPERTY(BlueprintReadWrite)
	float GroundSpeed;

	UPROPERTY(BlueprintReadWrite)
	bool bShouldMove;

	UPROPERTY(BlueprintReadWrite)
	bool bIsInGround;

	UPROPERTY(BlueprintReadWrite)
	bool bIsFalling;

	UPROPERTY(BlueprintReadWrite)
	bool bIsCrouching;

};
