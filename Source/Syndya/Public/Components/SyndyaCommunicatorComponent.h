// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SyndyaCommunicator.h"
#include "SyndyaCommunicatorComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMatchFoundSignature, const FSyndyaMatchInfos&, MatchInfos);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRequestMetadataSignature, FString, DataName);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), BlueprintType, Blueprintable )
class SYNDYA_API USyndyaCommunicatorComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	USyndyaCommunicatorComponent();


	UFUNCTION(BlueprintCallable, Category = "Communicator")
	FORCEINLINE USyndyaCommunicator* GetCommunicator() const { return Communicator; }

protected:

	virtual void BeginPlay() override;

	UPROPERTY(BlueprintAssignable)
	FOnMatchFoundSignature OnMatchFound;

	UPROPERTY(BlueprintAssignable)
	FOnRequestMetadataSignature OnRequestMetadata;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Communicator", meta = (AllowPrivateAccess = true))
	FString ServerURL;

	UPROPERTY()
	TObjectPtr<USyndyaCommunicator> Communicator;

};
