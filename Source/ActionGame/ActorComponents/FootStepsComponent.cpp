// Fill out your copyright notice in the Description page of Project Settings.

#include "ActorComponents/FootStepsComponent.h"
#include "Interfaces/CharacterInterface.h"
#include "PhysicalMaterials/AG_PhysicalMaterial.h"
#include "DrawDebugHelpers.h"

static TAutoConsoleVariable<int32> CVarShowFootsteps(
	TEXT("ShowDebugFootspes"),
	0,
	TEXT("Draws debug info about footsteps")
	TEXT("	0: off\n")
	TEXT("	1: on\n"),
	ECVF_Cheat);


UFootStepsComponent::UFootStepsComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bAllowTickOnDedicatedServer = false;
}

void UFootStepsComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UFootStepsComponent::HandleFootStep(EFoot Foot, USkeletalMeshComponent* SkeletalMesh)
{
	const int32 DebugShowFootsetps = CVarShowFootsteps.GetValueOnAnyThread();
	if(!SkeletalMesh) return;
	
	const FVector SocketLocation = SkeletalMesh->GetSocketLocation(Foot == EFoot::Left ? "foot_l" :  "foot_r");
	const FVector StartLocation = SocketLocation + FVector::UpVector*20;
	const FVector EndLocation = StartLocation + FVector::UpVector * -50;
	
	FCollisionQueryParams QueryParams;
	QueryParams.bReturnPhysicalMaterial = true;
	QueryParams.AddIgnoredActor(GetOwner());
	
	FHitResult HitResult;
	const bool bTraceResult = GetWorld()->LineTraceSingleByChannel(HitResult,StartLocation,EndLocation,ECC_WorldStatic,QueryParams);
	if(!bTraceResult) return;
	if(!HitResult.bBlockingHit) return;
	if(!HitResult.PhysMaterial.Get()) return;

	UAG_PhysicalMaterial* PhysicalMaterial = Cast<UAG_PhysicalMaterial>(HitResult.PhysMaterial.Get());
	if(!PhysicalMaterial) return;

	//UGameplayStatics::PlaySoundAtLocation(GetWorld(),PhysicalMaterial->FootstepSound,HitResult.Location,1.f);

	if(DebugShowFootsetps <= 0)
	{
		DrawDebugSphere(GetWorld(),StartLocation,16,16,FColor::White,false,4);
		return;
	}
	DrawDebugString(GetWorld(),StartLocation,PhysicalMaterial->GetName(),nullptr,FColor::White,4.f);
	
}

