#pragma once
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LightSwitchButton.generated.h"

UCLASS()
class WEBSOCKETTEST_API ALightSwitchButton : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ALightSwitchButton();
	void fun1();
	void fun2();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UPROPERTY(VisibleAnywhere, Category = "Light Switch")
		class UPointLightComponent* PointLight;
	UPROPERTY(VisibleAnywhere, Category = "Light Switch")
		class USphereComponent* LightSphere;
	UPROPERTY(VisibleAnywhere, Category = "Light Switch")
		float LightIntensity;
	UFUNCTION(BlueprintCallable, Category = "Light Switch")
		void ToggleLight();

};

