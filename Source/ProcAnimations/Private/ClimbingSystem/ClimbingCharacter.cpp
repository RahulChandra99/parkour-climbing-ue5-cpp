// Copyright Epic Games, Inc. All Rights Reserved.


#include "ClimbingSystem/ClimbingCharacter.h"
#include "ClimbingSystem/ClimbingCharacter.h"

#include "ProcAnimations/DebugHelper.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "ClimbingSystem/CustomMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "MotionWarpingComponent.h"



AClimbingCharacter::AClimbingCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCustomMovementComponent>(ACharacter::CharacterMovementComponentName))
{

	/** Default Wide TP Camera */

	TPWideCameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("TPWideCameraBoom"));
	TPWideCameraBoom->SetupAttachment(GetRootComponent());
	// Adjust the camera to create an over-the-shoulder view
	TPWideCameraBoom->bUsePawnControlRotation = true; // Camera rotates with the controller
	// Initialize FollowCamera
	TPWideCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("TPWideCamera"));
	TPWideCamera->SetupAttachment(TPWideCameraBoom);
	TPWideCamera->bUsePawnControlRotation = false; // Camera is independent of the character rotation
	
	/** TP Camera */

	TPCameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("TPCameraBoom"));
	TPCameraBoom->SetupAttachment(GetRootComponent());
	// Adjust the camera to create an over-the-shoulder view
	TPCameraBoom->TargetArmLength = 200.f; 
	TPCameraBoom->SocketOffset = FVector(0.f, 50.f, 40.f); // Offset to the right and slightly above the character
	TPCameraBoom->bUsePawnControlRotation = true; // Camera rotates with the controller
	// Initialize FollowCamera
	TPCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("TPCamera"));
	TPCamera->SetupAttachment(TPCameraBoom);
	TPCamera->bUsePawnControlRotation = false; // Camera is independent of the character rotation

	TPCameraBoom->SetActive(false);
	TPCamera->SetActive(false);
	
	/** Combat TP Camera */

	CombatCameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CombatCameraBoom"));
	CombatCameraBoom->SetupAttachment(GetRootComponent());
	// Adjust the camera to create an over-the-shoulder view
	CombatCameraBoom->TargetArmLength = 150.f; 
	CombatCameraBoom->SocketOffset = FVector(0.f, 75.f, 60.f); // Offset to the right and slightly above the character
	CombatCameraBoom->bUsePawnControlRotation = true; // Camera rotates with the controller
	// Initialize FollowCamera
	CombatCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("CombatCamera"));
	CombatCamera->SetupAttachment(CombatCameraBoom);
	CombatCamera->bUsePawnControlRotation = false; // Camera is independent of the character rotation

	CombatCameraBoom->SetActive(false);
	CombatCamera->SetActive(false);
	
	

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Disable controller rotation affecting character directly
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Use custom movement component
	CustomMovementComponent = Cast<UCustomMovementComponent>(GetCharacterMovement());

	// Configure character movement
	GetCustomMovementComponent()->bOrientRotationToMovement = true; // Character moves in the direction of input
	GetCustomMovementComponent()->RotationRate = FRotator(0.0f, 360.0f, 0.0f); // Slower, more realistic rotation speed
	GetCustomMovementComponent()->MaxWalkSpeed = 300.f; // Slow, deliberate walk speed
	GetCustomMovementComponent()->BrakingDecelerationWalking = 1500.f; // Adjust braking for smooth stopping
	GetCustomMovementComponent()->JumpZVelocity = 0.f; // Hitman doesn't jump, so we can disable or set this to 0
	GetCustomMovementComponent()->AirControl = 0.f; // Disable air control for grounded movement feel
	GetCustomMovementComponent()->MinAnalogWalkSpeed = 10.f; // Minimal analog input speed
	GetCustomMovementComponent()->BrakingDecelerationWalking = 1500.f;

	// Adjust running mechanics (if applicable)
	GetCustomMovementComponent()->MaxWalkSpeedCrouched = 200.f; // Hitman walks slower when crouched


	MotionWarpingComponent = CreateDefaultSubobject<UMotionWarpingComponent>("MotionWarpingComp");
}

void AClimbingCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	GetCustomMovementComponent()->MaxWalkSpeed = WalkSpeed;
	
	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	if(CustomMovementComponent)
	{
		CustomMovementComponent->OnEnterClimbStateDelegate.BindUObject(this, &ThisClass::OnPlayerEnterClimbState);
		CustomMovementComponent->OnExitClimbStateDelegate.BindUObject(this, &ThisClass::OnPlayerExitClimbState);
		
	}
}

//////////////////////////////////////////////////////////////////////////
// Input




void AClimbingCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AClimbingCharacter::Move);

		//Running
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &AClimbingCharacter::Run);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &AClimbingCharacter::Run);

		//Toggle Running
		EnhancedInputComponent->BindAction(ToggleSprintAction, ETriggerEvent::Started, this, &AClimbingCharacter::ToggleRun);
		
		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AClimbingCharacter::Look);

		//Climbing
		EnhancedInputComponent->BindAction(ClimbAction, ETriggerEvent::Started, this, &AClimbingCharacter::OnClimbActionStarted);

		//Camera Switch
		EnhancedInputComponent->BindAction(CameraSwitchAction, ETriggerEvent::Started, this, &AClimbingCharacter::SwitchCamera);

	}

}


void AClimbingCharacter::Move(const FInputActionValue& Value)
{
	if(!CustomMovementComponent) return;
	if(CustomMovementComponent->IsClimbing())
	{
		HandleClimbMovementInput(Value);
	}
	else
	{
		HandleGroundMovementInput(Value);
	}
	
}

void AClimbingCharacter::Run(const FInputActionValue& Value)
{
	if(!CustomMovementComponent) return;

	if(bIsSprintOn) return;
	
	if(Value.GetMagnitude() > 0.f)
		GetCustomMovementComponent()->MaxWalkSpeed = RunSpeed;
	else
		GetCustomMovementComponent()->MaxWalkSpeed = WalkSpeed;
}

void AClimbingCharacter::ToggleRun(const FInputActionValue& Value)
{
	if(!CustomMovementComponent) return;

	if(!bIsSprintOn)
	{
		bIsSprintOn = true;
		GetCustomMovementComponent()->MaxWalkSpeed = RunSpeed;
	}
	else if(bIsSprintOn)
	{
		bIsSprintOn = false;
		GetCustomMovementComponent()->MaxWalkSpeed = WalkSpeed;
	}
}

	

	void AClimbingCharacter::SwitchCamera(const FInputActionValue& Value)
	{
		// Validate the input action value
		if (!Value.Get<bool>())
			return;

		// Increment camera index and loop back if it exceeds the limit
		CurrentActiveCamera = (CurrentActiveCamera + 1) % 3;

		// Deactivate all cameras initially
		DeactivateAllCameras();

		// Activate the selected camera based on CurrentActiveCamera
		switch (CurrentActiveCamera)
		{
		case 0:
			ActivateCamera(TPWideCameraBoom, TPWideCamera);
			break;
		case 1:
			ActivateCamera(CombatCameraBoom, CombatCamera);
			break;
		case 2:
			ActivateCamera(TPCameraBoom, TPCamera);
			break;
		default:
			UE_LOG(LogTemp, Warning, TEXT("Invalid camera index: %d"), CurrentActiveCamera);
			break;
		}
	}

	// Helper to deactivate all cameras
	void AClimbingCharacter::DeactivateAllCameras()
	{
		TPCameraBoom->SetActive(false);
		TPCamera->SetActive(false);

		CombatCameraBoom->SetActive(false);
		CombatCamera->SetActive(false);

		TPWideCameraBoom->SetActive(false);
		TPWideCamera->SetActive(false);
	}

	// Helper to activate a specific camera
	void AClimbingCharacter::ActivateCamera(USceneComponent* CameraBoom, UCameraComponent* Camera)
	{
		if (CameraBoom)
		{
			CameraBoom->SetActive(true);
		}
		if (Camera)
		{
			Camera->SetActive(true);
		}
	}


	



void AClimbingCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AClimbingCharacter::OnClimbActionStarted(const FInputActionValue& Value)
{
	
	if(!CustomMovementComponent)return;

	if(!CustomMovementComponent->IsClimbing())
	{
		
		CustomMovementComponent->ToggleClimbing(true);
	}
	else
	{
		CustomMovementComponent->ToggleClimbing(false);
	}
}

void AClimbingCharacter::HandleGroundMovementInput(const FInputActionValue& Value)
{
	// input is a Vector2D
	const FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AClimbingCharacter::HandleClimbMovementInput(const FInputActionValue& Value)
{
	// input is a Vector2D
	const FVector2D MovementVector = Value.Get<FVector2D>();

	// get right vector
	const FVector ForwardDirection = FVector::CrossProduct(
		-CustomMovementComponent->GetClimbableSurfaceNormal(),
		GetActorRightVector()
	);

	// get forward vector 
	const FVector RightDirection = FVector::CrossProduct(
	-CustomMovementComponent->GetClimbableSurfaceNormal(),
	-GetActorUpVector()
		);

	// add movement 
	AddMovementInput(ForwardDirection, MovementVector.Y);
	AddMovementInput(RightDirection, MovementVector.X);
}

void AClimbingCharacter::OnPlayerEnterClimbState()
{
}

void AClimbingCharacter::OnPlayerExitClimbState()
{
}





