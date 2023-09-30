// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/AnimNotify_Step.h"
#include "Interfaces/CharacterInterface.h"

void UAnimNotify_Step::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation);

	if(!Character)
	{
		Character = Cast<ICharacterInterface>(MeshComp->GetOwner());
		if(!Character) return;
	}

	if(Character) Character->HandleFootstep(Foot);
}
