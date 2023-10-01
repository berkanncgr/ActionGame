// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "ActionGameTypes.h"
#include "InputAction.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Abilities/GameplayAbility.h"
#include "Interfaces/CharacterInterface.h"
#include "ActionGameCharacter.generated.h"

class UAG_AbilitySystemComponentBase;
class UAG_AttributeSetBase;
class UGameplayEffect;
class UGameplayAbility;

UCLASS(config=Game)
class AActionGameCharacter : public ACharacter, public IAbilitySystemInterface, public ICharacterInterface
{
	GENERATED_BODY()
	
public:
	
	AActionGameCharacter(const FObjectInitializer& ObjectInitializer);
	
	bool ApplyGameplayEffectToSelf(TSubclassOf<UGameplayEffect> Effect, FGameplayEffectContextHandle InEffectContext);
	
	UFUNCTION(BlueprintCallable)
	FORCEINLINE FCharacterData GetCharacterData() const
	{ return CharacterData;}

	UFUNCTION(BlueprintCallable)
	void SetCharacterData(const FCharacterData& InData);

	FORCEINLINE virtual USkeletalMeshComponent* GetSkeletalMesh() const override
	{ return GetMesh(); }

	virtual void HandleFootstep(EFoot Foot) override;

protected:

	// Before BeginPlay in all Locals and Server:
	virtual void PostInitializeComponents() override;

	// For Server side:
	virtual void PossessedBy(AController* NewController) override;

	// For Owning Client side:
	virtual void OnRep_PlayerState() override;

	virtual void PawnClientRestart() override;
	
	virtual void InitFromCharacterData(const FCharacterData& InCharacterData, bool bFromReplication = false);
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	void GiveAbilities();
	void ApplyStartupEffects();
	
	UFUNCTION()
	void OnRep_CharacterData(FCharacterData OldData);

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	class UFootStepsComponent* FootStepsComponent;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,ReplicatedUsing=OnRep_CharacterData)
	FCharacterData CharacterData;

	UPROPERTY(EditDefaultsOnly)
	class UCharacterDataAsset* CharacterDataAsset;
	
	UPROPERTY(EditDefaultsOnly)
	UAG_AbilitySystemComponentBase* AbilitySystemComponent;

	UPROPERTY(Transient)
	UAG_AttributeSetBase* AttributeSet;

#pragma region Input

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	class UInputMappingContext* DefaultInputMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UInputAction* IA_MoveForward;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UInputAction* IA_MoveRight;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite)
	UInputAction* IA_Turn;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite)
	UInputAction* IA_LookUp;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite)
	UInputAction* IA_Jump;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite)
	UInputAction* IA_Crouch;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float TurnRateGamepad = 20;

	void MoveForward(const FInputActionValue& Value);
	void MoveRight(const FInputActionValue& Value);
	void LookUp(const FInputActionValue& Value);
	void Turn(const FInputActionValue& Value);
	void JumpStart(const FInputActionValue& Value);
	void JumpEnd(const FInputActionValue& Value);
	void OnCrouchStart(const FInputActionValue& Value);
	void OnCrouchEnd(const FInputActionValue& Value);

#pragma endregion Input

#pragma region UEDEFAULTS
	
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
	
	// MappingContext 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	// /** Jump Input Action 
	UPROPERTY(BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;


	// /** Move Input Action
	UPROPERTY(BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	 UInputAction* MoveAction;

	// /** Look Input Action
	UPROPERTY(BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;
	
	// /** Called for movement input
	void Move(const FInputActionValue& Value);

	// /** Called for looking input
	void Look(const FInputActionValue& Value); 

	// // APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void Landed(const FHitResult& Hit) override;

	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

public:
	
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

#pragma endregion UEDEFAULTS

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FGameplayTag JumpEventTag;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTagContainer InAirTags;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTagContainer CrouchTags;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> CrouchStateEffect;
};

