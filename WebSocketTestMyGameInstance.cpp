// Fill out your copyright notice in the Description page of Project Settings.


#include "WebSocketTestMyGameInstance.h"
#include "WebSocketsModule.h"
void UWebSocketTestMyGameInstance::Init()
{
	Super::Init();
	if (!FModuleManager::Get().IsModuleLoaded("WebSockets"))
	{
		FModuleManager::Get().LoadModule("WebSockets");
	}
	WebSocket = FWebSocketsModule::Get().CreateWebSocket("wss://62tnfotvb8.execute-api.ap-northeast-1.amazonaws.com/production");
	WebSocket->OnConnected().AddLambda([]()
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, "Successfully Connected");
			//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Moisture value is greater than threshold value!!"));
		});
	WebSocket->OnConnectionError().AddLambda([](const FString& Error)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, Error);
		});
	WebSocket->OnClosed().AddLambda([](int32 StatusCode,const FString& Reason,bool bWasClean)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, bWasClean?FColor::Green:FColor::Red, "Connection Closed"+Reason);
		});
	WebSocket->OnMessage().AddLambda([](const FString& MessageString)
	{
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Cyan, "Received message:" + MessageString);
	});
	WebSocket->OnMessageSent().AddLambda([](const FString& MessageString)
	{
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "Sent message:" + MessageString);
	});
	WebSocket->Connect();
}
void UWebSocketTestMyGameInstance::Shutdown()
{
	if (WebSocket->IsConnected())
	{
		WebSocket->Close();
	}
	Super::Shutdown();
}
