// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/SyndyaCommunicatorComponent.h"

USyndyaCommunicatorComponent::USyndyaCommunicatorComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USyndyaCommunicatorComponent::BeginPlay()
{
	Super::BeginPlay();

	Communicator = NewObject<USyndyaCommunicator>();
	Communicator->ServerURL = ServerURL;

	if (IsValid(Communicator))
	{
		Communicator->OnMatchFound.AddLambda([this](const FSyndyaMatchInfos& MatchInfos) {
			OnMatchFound.Broadcast(MatchInfos);
		});

		Communicator->OnRequestMetadata.AddLambda([this](const FString& DataName) {
			OnRequestMetadata.Broadcast(DataName);
		});
	}
}


