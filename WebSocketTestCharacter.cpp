// Copyright Epic Games, Inc. All Rights Reserved.
#include "WebSocketTestCharacter.h"
#include "WebSocketTestMyGameInstance.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "XRMotionControllerBase.h" 
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "SwitchPawn.h"
#include "LightSwitchButton.h"
#include "LightSwitchButton.cpp"

//////////////////////////////////////////////////////////////////////////
// AWebSocketTestCharacter

AWebSocketTestCharacter::AWebSocketTestCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
	TriggerCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Trigger Capsule"));
	TriggerCapsule->InitCapsuleSize(55.f, 96.0f);;
	TriggerCapsule->SetCollisionProfileName(TEXT("Trigger"));
	TriggerCapsule->SetupAttachment(RootComponent);

	// bind trigger events
	TriggerCapsule->OnComponentBeginOverlap.AddDynamic(this, &AWebSocketTestCharacter::OnOverlapBegin);
	TriggerCapsule->OnComponentEndOverlap.AddDynamic(this, &AWebSocketTestCharacter::OnOverlapEnd);

	// set current light switch to null
	CurrentLightSwitch = nullptr;

}

//////////////////////////////////////////////////////////////////////////
// Input

/*void AWebSocketTestCharacter::NotifyServer()
{
	UWebSocketTestMyGameInstance* GameInstance = Cast<UWebSocketTestMyGameInstance>(GetGameInstance());
	if (GameInstance)
	{
		if (GameInstance->WebSocket->IsConnected())
		{
			TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
			JsonObject->SetStringField("action", "sendmessage");
			JsonObject->SetStringField("message", "Pressed E Key");
			FString OutputString;
			TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&OutputString);
			FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);
			GameInstance->WebSocket->Send(OutputString);
			GameInstance->WebSocket->Send("Pressed E Key");
		}
	}
} */
void AWebSocketTestCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &AWebSocketTestCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AWebSocketTestCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AWebSocketTestCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AWebSocketTestCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &AWebSocketTestCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &AWebSocketTestCharacter::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AWebSocketTestCharacter::OnResetVR);
	PlayerInputComponent->BindAction("NotifyServer", IE_Pressed, this, &AWebSocketTestCharacter::NotifyServer);
}


void AWebSocketTestCharacter::OnResetVR()
{
	// If WebSocketTest is added to a project via 'Add Feature' in the Unreal Editor the dependency on HeadMountedDisplay in WebSocketTest.Build.cs is not automatically propagated
	// and a linker error will result.
	// You will need to either:
	//		Add "HeadMountedDisplay" to [YourProject].Build.cs PublicDependencyModuleNames in order to build successfully (appropriate if supporting VR).
	// or:
	//		Comment or delete the call to ResetOrientationAndPosition below (appropriate if not supporting VR)
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void AWebSocketTestCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
		Jump();
}

void AWebSocketTestCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
		StopJumping();
}

void AWebSocketTestCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AWebSocketTestCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AWebSocketTestCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AWebSocketTestCharacter::MoveRight(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}
void AWebSocketTestCharacter::NotifyServer()
{
	UWebSocketTestMyGameInstance* GameInstance = Cast<UWebSocketTestMyGameInstance>(GetGameInstance());

	if (CurrentLightSwitch)
	{
		b++;

	}
	/*if (GameInstance)
	{
		if (GameInstance->WebSocket->IsConnected())
		{
			if (b % 2 != 0) {

				GameInstance->WebSocket->Send("{ \"action\":\"sendmessage\",\"message\":\"off1\"}");
				CurrentLightSwitch->fun1();
			}
			else {
				GameInstance->WebSocket->Send("{ \"action\":\"sendmessage\",\"message\":\"on1\"}");
				CurrentLightSwitch->fun2();
			}
		}
	}*/
}
void AWebSocketTestCharacter::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	if (OtherActor && (OtherActor != this) && OtherComp && OtherActor->GetClass()->IsChildOf(ALightSwitchButton::StaticClass()))
	{
		CurrentLightSwitch = Cast<ALightSwitchButton>(OtherActor);
	}
}

void AWebSocketTestCharacter::OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
	if (OtherActor && (OtherActor != this) && OtherComp) {
		CurrentLightSwitch = nullptr;

	}
}
