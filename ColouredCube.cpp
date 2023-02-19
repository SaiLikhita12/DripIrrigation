// Fill out your copyright notice in the Description page of Project Settings.


#include "ColouredCube.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInterface.h"
#include "WebSocketTest/WebSocketTestMyGameInstance.h"
// Sets default values
AColouredCube::AColouredCube()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AColouredCube::BeginPlay()
{
	Super::BeginPlay();
	auto Cube = FindComponentByClass<UStaticMeshComponent>();
	auto Material = Cube->GetMaterial(0);
	DynamicMaterial = UMaterialInstanceDynamic::Create(Material, NULL);
	Cube->SetMaterial(0, DynamicMaterial);

	
}

// Called every frame
void AColouredCube::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UWebSocketTestMyGameInstance* GameInstance = Cast<UWebSocketTestMyGameInstance>(GetGameInstance());
	/*if (GameInstance)
	{
		if(GameInstance->moistval < 1200)
		{
			DynamicMaterial->SetScalarParameterValue(TEXT("Blend"), 0.0532);
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, "Moisture value less than 1200");
		}
		else
		{
			DynamicMaterial->SetScalarParameterValue(TEXT("Blend"), 0.732);
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "Moisture value greater than 1200");
		}
	}*/

}

