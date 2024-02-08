// Fill out your copyright notice in the Description page of Project Settings.


#include "SyndyaCommunicator.h"

#include "WebSocketsModule.h"
#include "IWebSocket.h"

USyndyaCommunicator::USyndyaCommunicator()
{

}

void USyndyaCommunicator::StartSearchingForMatch()
{
    if (GetStatus() != ESyndyaSocketStatus::SSS_NotConnected)
        StopSearchingForMatch();

    Socket = FWebSocketsModule::Get().CreateWebSocket(ServerURL, TEXT("ws"));

	Socket->OnConnected().AddLambda([this]() -> void {
        UE_LOG(LogTemp, Display, TEXT("Syndya Socket Connection Established"));
        SendPendingMetadatas();
	});

	Socket->OnConnectionError().AddLambda([this](const FString & Error) -> void {
        UE_LOG(LogTemp, Warning, TEXT("Syndya Socket Connection Error: %s"), *Error);
        ClearSearch();
	});

	Socket->OnClosed().AddLambda([this](int32 StatusCode, const FString& Reason, bool bWasClean) -> void {
        if (bWasClean)
        {
            UE_LOG(LogTemp, Display, TEXT("Syndya Socket Connection Closed: [%d] %s"), StatusCode, *Reason);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Syndya Socket Connection Closed: [%d] %s"), StatusCode, *Reason);
        }
        ClearSearch();
	});

	Socket->OnMessage().AddLambda([this](const FString & Message) -> void {
		HandleMessage(Message);
	});

	// Socket->OnRawMessage().AddLambda([this](const void* Data, SIZE_T Size, SIZE_T BytesRemaining) -> void {
		
	// });

	// Socket->OnMessageSent().AddLambda([this](const FString& MessageString) -> void {
		
	// });

	Socket->Connect();
}

void USyndyaCommunicator::StopSearchingForMatch()
{
    ClearSearch(true);
}

void USyndyaCommunicator::SendMetadata(FString Key, FString Value)
{
    PendingMetadata.Add(TPair<FString, FString>(Key, Value));
    if (GetStatus() == ESyndyaSocketStatus::SSS_Connected)
        SendPendingMetadatas();
}

ESyndyaSocketStatus USyndyaCommunicator::GetStatus() const
{
    if (Socket)
    {
        if (Socket->IsConnected())
        {
            return ESyndyaSocketStatus::SSS_Connected;
        }
        return ESyndyaSocketStatus::SSS_Connecting;
    }
    return ESyndyaSocketStatus::SSS_NotConnected;
}

void USyndyaCommunicator::SendPendingMetadatas()
{
    if (GetStatus() != ESyndyaSocketStatus::SSS_Connected)
        return;

    for (const TPair<FString, FString>& Data : PendingMetadata)
    {
        FString Message = FString::Printf(TEXT("meta %s=%s"), *Data.Key, *Data.Value);
        Socket->Send(Message);
    }
}

void USyndyaCommunicator::ClearSearch(bool CloseSocket)
{
    if (CloseSocket && Socket)
    {
        Socket->Close();
    }
    Socket = nullptr;
    PendingMetadata.Reset();
}

static bool IsCommand(const FString& Message, const FString& Command, FString& OutParameters)
{
    const FString CommandPrefix = Command + " "; 
    if (Message.StartsWith(Command))
    {
        OutParameters = Message;
        OutParameters.RemoveFromStart(CommandPrefix);
        return true;
    }
    else
    {
        return false;
    }
}

void USyndyaCommunicator::HandleMessage(const FString& Message)
{
    FString Parameters;
    if (IsCommand(Message, TEXT("meta"), Parameters))
    {
        OnRequestMetadata.Broadcast(Parameters);
    }
    else if (IsCommand(Message, TEXT("game"), Parameters))
    {
        FSyndyaMatchInfos MatchInfos;
        MatchInfos.GameAddr = Parameters;
        OnMatchFound.Broadcast(MatchInfos);
    }
}
