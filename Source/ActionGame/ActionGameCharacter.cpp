// Copyright Epic Games, Inc. All Rights Reserved.

#include "ActionGameCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/AttributeSets/AG_AttributeSetBase.h"
#include "AbilitySystem/Components/AG_AbilitySystemComponentBase.h"
#include "ActorComponents/AG_CharacterMovementComponent.h"
#include "ActorComponents/FootStepsComponent.h"
#include "DataAssets/CharacterDataAsset.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"
#include "ActorComponents/AG_MotionWarpingComponent.h"

AActionGameCharacter::AActionGameCharacter(const FObjectInitializer& ObjectInitializer):
Super(ObjectInitializer.SetDefaultSubobjectClass<UAG_CharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
#pragma region UEDEFAULTS
	
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

#pragma endregion UEDEFAULTS

	AbilitySystemComponent = CreateDefaultSubobject<UAG_AbilitySystemComponentBase>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetMaxMovementSpeedAttribute()).AddUObject(this,&AActionGameCharacter::OnMaxMovementSpeedChange);

	AttributeSet = CreateDefaultSubobject<UAG_AttributeSetBase>(TEXT("AttributeSet"));
	CharacterMovementComponent = Cast<UAG_CharacterMovementComponent>(GetCharacterMovement());
	FootStepsComponent = CreateDefaultSubobject<UFootStepsComponent>(TEXT("FootStepsComponent"));
	MotionWarpingComponent = CreateDefaultSubobject<UAG_MotionWarpingComponent>(TEXT("MotionWarpingComponent"));
}


void AActionGameCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if(!IsValid(CharacterDataAsset)) return;
	
	SetCharacterData(CharacterDataAsset->CharacterData);
}


void AActionGameCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
}

void AActionGameCharacter::PossessedBy(AController* NewController)
{
	// This is called in server:
	Super::PossessedBy(NewController);

	AbilitySystemComponent->InitAbilityActorInfo(this,this);
	GiveAbilities();
	ApplyStartupEffects();
}

void AActionGameCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	UKismetSystemLibrary::PrintString(GetWorld(),TEXT("AActionGameCharacter::OnRep_PlayerState"),1,1,FColor::Cyan,3);
	
	AbilitySystemComponent->InitAbilityActorInfo(this,this);
	//ApplyStartupEffects();
}

void AActionGameCharacter::PawnClientRestart()
{
	Super::PawnClientRestart();
	UKismetSystemLibrary::PrintString(GetWorld(),TEXT("PawnClientRestart"));
	
	APlayerController* PlayerController = Cast<APlayerController>(Controller); if(!Controller) return;
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()); if(!Subsystem) return;

	Subsystem->AddMappingContext(DefaultInputMappingContext, 0);
}

void AActionGameCharacter::GiveAbilities()
{
	if(!HasAuthority()) return;
	if(!AbilitySystemComponent) return;

	for(auto DefaultAbility : CharacterData.Abilities)
	{
		AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(DefaultAbility));
	}
}

void AActionGameCharacter::ApplyStartupEffects()
{
	// If we are server and there is no nullptr. (InitializeAttributes already called in server so why GetLocalRole Check? [Maybe for OnRep_PlayerState()])
	// Try early return here:
	if(GetLocalRole() == ROLE_Authority)
	{
		FGameplayEffectContextHandle EffectContex = AbilitySystemComponent->MakeEffectContext();
		EffectContex.AddSourceObject(this);

		for(auto CharacterEffect : CharacterData.Effects)
		{
			ApplyGameplayEffectToSelf(CharacterEffect,EffectContex);
		}
	}
}


bool AActionGameCharacter::ApplyGameplayEffectToSelf(TSubclassOf<UGameplayEffect> Effect, FGameplayEffectContextHandle InEffectContext)
{
	if(!Effect.Get()) return false;

	FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(Effect,1,InEffectContext);
	if(!SpecHandle.IsValid()) return false;

	FActiveGameplayEffectHandle AGEH = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	return AGEH.WasSuccessfullyApplied();
}

void AActionGameCharacter::SetCharacterData(const FCharacterData& InData)
{
	CharacterData = InData;
	InitFromCharacterData(CharacterData);
}

void AActionGameCharacter::InitFromCharacterData(const FCharacterData& InCharacterData, bool bFromReplication)
{
	if(!bFromReplication) return;
}

void AActionGameCharacter::OnRep_CharacterData(FCharacterData OldData)
{
	InitFromCharacterData(CharacterData,true);
}

UAbilitySystemComponent* AActionGameCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AActionGameCharacter::HandleFootstep(EFoot Foot)
{
	FootStepsComponent->HandleFootStep(Foot,GetMesh());
}


#pragma region Input

void AActionGameCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
	if(!EnhancedInputComponent) return;


	//Moving
	EnhancedInputComponent->BindAction(IA_MoveForward, ETriggerEvent::Triggered, this, &AActionGameCharacter::MoveForward);
	EnhancedInputComponent->BindAction(IA_MoveRight, ETriggerEvent::Triggered, this, &AActionGameCharacter::MoveRight);

	//Looking
	EnhancedInputComponent->BindAction(IA_LookUp, ETriggerEvent::Triggered, this, &AActionGameCharacter::LookUp);
	EnhancedInputComponent->BindAction(IA_Turn, ETriggerEvent::Triggered, this, &AActionGameCharacter::Turn);

	//Jumping
	EnhancedInputComponent->BindAction(IA_Jump, ETriggerEvent::Started, this, &AActionGameCharacter::JumpStart);
	EnhancedInputComponent->BindAction(IA_Jump, ETriggerEvent::Completed, this, &AActionGameCharacter::JumpEnd);

	//Crouching
	EnhancedInputComponent->BindAction(IA_Crouch, ETriggerEvent::Started, this, &AActionGameCharacter::OnCrouchStart);
	EnhancedInputComponent->BindAction(IA_Crouch, ETriggerEvent::Completed, this, &AActionGameCharacter::OnCrouchEnd);

	//Sprinting
	EnhancedInputComponent->BindAction(IA_Sprint, ETriggerEvent::Started, this, &AActionGameCharacter::OnSprintStart);
	EnhancedInputComponent->BindAction(IA_Sprint, ETriggerEvent::Completed, this, &AActionGameCharacter::OnSprintEnd);
}



void AActionGameCharacter::MoveForward(const FInputActionValue& Value)
{
	// input is a float
	float Magnitude = Value.Get<float>();
	
	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// add movement 
		AddMovementInput(ForwardDirection, Magnitude);
	}
}

void AActionGameCharacter::MoveRight(const FInputActionValue& Value)
{
	// input is a float
	float Magnitude = Value.Get<float>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(RightDirection, Magnitude);
	}
}

void AActionGameCharacter::LookUp(const FInputActionValue& Value)
{
	AddControllerPitchInput(-1* Value.GetMagnitude() * TurnRateGamepad/10 * GetWorld()->GetDeltaSeconds());
}

void AActionGameCharacter::Turn(const FInputActionValue& Value)
{
	AddControllerYawInput(Value.GetMagnitude() * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void AActionGameCharacter::JumpStart(const FInputActionValue& Value)
{
	/* Default Unreal Style */ // Super::Jump();

	/* // Old Style 
	FGameplayEventData PayLoad;
	PayLoad.Instigator = this; PayLoad.EventTag = JumpEventTag;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this,JumpEventTag,PayLoad); */

	CharacterMovementComponent->TryTraversal(AbilitySystemComponent);
}

void AActionGameCharacter::JumpEnd(const FInputActionValue& Value)
{
	/* Old Style */ //  Super::StopJumping();
}

void AActionGameCharacter::OnCrouchStart(const FInputActionValue& Value)
{
	AbilitySystemComponent->TryActivateAbilitiesByTag(CrouchTags);	
}

void AActionGameCharacter::OnCrouchEnd(const FInputActionValue& Value)
{
	AbilitySystemComponent->CancelAbilities(&CrouchTags);
}

void AActionGameCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	if(!AbilitySystemComponent) return;

	AbilitySystemComponent->RemoveActiveEffectsWithTags(InAirTags);
}

void AActionGameCharacter::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);

	if(!CrouchStateEffect.Get()) return;

	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(CrouchStateEffect,1,EffectContext);
	if(!SpecHandle.IsValid()) return;

	FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	if(!ActiveGEHandle.WasSuccessfullyApplied())
	{
		UE_LOG(LogTemp,Error,TEXT("ActiveGEHandle FAILED-> AActionGameCharacter::OnStartCrouch")); return;
	}
	
}

void AActionGameCharacter::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	AbilitySystemComponent->RemoveActiveGameplayEffectBySourceEffect(CrouchStateEffect,AbilitySystemComponent);
	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
}

void AActionGameCharacter::OnSprintStart(const FInputActionValue& InputActionValue)
{
	AbilitySystemComponent->TryActivateAbilitiesByTag(SprintTags);	
}

void AActionGameCharacter::OnSprintEnd(const FInputActionValue& InputActionValue)
{
	AbilitySystemComponent->CancelAbilities(&SprintTags);
}

void AActionGameCharacter::OnMaxMovementSpeedChange(const FOnAttributeChangeData& AttributeData)
{
	// Syncing data value and CharacterMovementComponent value.
	GetCharacterMovement()->MaxWalkSpeed = AttributeData.NewValue;
}


#pragma endregion Input

#pragma region UEDEFAULTS

void AActionGameCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AActionGameCharacter,CharacterData)
	
}

/*
void AActionGameCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AActionGameCharacter::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AActionGameCharacter::Look);
	}
}


void AActionGameCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AActionGameCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
} 


*/

#pragma endregion UEDEFAULTS
