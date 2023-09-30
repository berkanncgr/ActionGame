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

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category=Animaiton)
	class UCharacterAnimDataAsset* DefaultCharacterAnimDataAsset;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite)
	class AActionGameCharacter* Character;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite)
	float Side;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite)
	float Forward;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite)
	FVector Velocity;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite)
	float GroundSpeed;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite)
	bool bShouldMove;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite)
	bool bIsInGround;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite)
	bool bIsFalling;

};
