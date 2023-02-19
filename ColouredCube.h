// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ColouredCube.generated.h"
class UMaterialInstanceDynamic;

UCLASS()
class WEBSOCKETTEST_API AColouredCube : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AColouredCube();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
private:
	UMaterialInstanceDynamic* DynamicMaterial;

};
