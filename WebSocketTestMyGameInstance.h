// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "IWebSocket.h"
#include "WebSocketTestMyGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class WEBSOCKETTEST_API UWebSocketTestMyGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	virtual void Init() override;
	virtual void Shutdown() override;
	TSharedPtr<IWebSocket>WebSocket;
	//int32 dist;
	//int32 moistval;
	//FString msg;
};
