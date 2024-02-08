// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SyndyaCommunicator.generated.h"

UENUM(BlueprintType)
enum class ESyndyaSocketStatus : uint8
{
	SSS_NotConnected	UMETA(DisplayName="Not Connected"),
	SSS_Connecting		UMETA(DisplayName="Connecting"),
	SSS_Connected		UMETA(DisplayName="Connected"),
};

USTRUCT(BlueprintType)
struct FSyndyaMatchInfos
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString GameAddr;
};

DECLARE_MULTICAST_DELEGATE_OneParam(FOnMatchFoundDelegate, const FSyndyaMatchInfos&);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnRequestMetadataDelegate, const FString&);

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class SYNDYA_API USyndyaCommunicator : public UObject
{
	GENERATED_BODY()

public:

	USyndyaCommunicator();

	UFUNCTION(BlueprintCallable, Category="MatchMaking")
	void StartSearchingForMatch();

	UFUNCTION(BlueprintCallable, Category="MatchMaking")
	void StopSearchingForMatch();

	UFUNCTION(BlueprintCallable, Category="MatchMaking")
	void SendMetadata(FString Key, FString Value);

	UFUNCTION(BlueprintCallable, Category="Websocket")
	ESyndyaSocketStatus GetStatus() const;

	FOnMatchFoundDelegate OnMatchFound;

	FOnRequestMetadataDelegate OnRequestMetadata;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="URL")
	FString ServerURL;

protected:

	UFUNCTION()
	void SendPendingMetadatas();

	UFUNCTION()
	void ClearSearch(bool CloseSocket = false);

	UFUNCTION()
	void HandleMessage(const FString& Message);

private:

	TArray<TPair<FString, FString>> PendingMetadata;

	TSharedPtr<class IWebSocket> Socket;
};
