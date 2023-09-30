// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionGame/AbilitySystem/AttributeSets/AG_AttributeSetBase.h"
#include "GameplayEffectExtension.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"

void UAG_AttributeSetBase::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if(GetOwningActor())
	{
		UKismetSystemLibrary::PrintString(GetWorld(),FString::Printf(TEXT("UAG_AttributeSetBase::PostGameplayEffectExecute %s"),*GetOwningActor()->GetName()),1,1,FColor::Purple,3);
	}

	// ????
	if(Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetMaxHealth(FMath::Clamp(GetHealth(),0,GetMaxHealth()));
	}

	else if(Data.EvaluatedData.Attribute == GetMaxMovementSpeedAttribute())
	{
		UCharacterMovementComponent* CharacterMovement = GetOwningActor() ? Cast<UCharacterMovementComponent>(GetOwningActor()->GetComponentByClass(UCharacterMovementComponent::StaticClass())) : nullptr;
		if(!CharacterMovement) return;

		const float MaxSpeed = GetMaxMovementSpeed();
		CharacterMovement->MaxWalkSpeed = MaxSpeed;
	}
}

void UAG_AttributeSetBase::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAG_AttributeSetBase,Health,OldHealth);
}

void UAG_AttributeSetBase::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAG_AttributeSetBase,Health,OldMaxHealth);
}

void UAG_AttributeSetBase::OnRep_Stamina(const FGameplayAttributeData& OldStamina)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAG_AttributeSetBase,Stamina,OldStamina);
}

void UAG_AttributeSetBase::OnRep_MaxStamina(const FGameplayAttributeData& OldMaxStamina)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAG_AttributeSetBase,MaxStamina,OldMaxStamina);
}

void UAG_AttributeSetBase::OnRep_MaxMovementSpeed(const FGameplayAttributeData& OldMaxMovementSpeed)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAG_AttributeSetBase,MaxMovementSpeed,OldMaxMovementSpeed);
}

void UAG_AttributeSetBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	//@ !!   todo: try COND_OwnerOnly !!!!!!
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(UAG_AttributeSetBase,Health,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAG_AttributeSetBase,MaxHealth,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAG_AttributeSetBase,Stamina,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAG_AttributeSetBase,MaxStamina,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAG_AttributeSetBase,MaxMovementSpeed,COND_None,REPNOTIFY_Always);
}



