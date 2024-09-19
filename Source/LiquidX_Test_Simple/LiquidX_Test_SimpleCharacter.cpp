// Copyright Epic Games, Inc. All Rights Reserved.

#include "LiquidX_Test_SimpleCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Kismet/GameplayStatics.h"
#include "PickupCube.h"
#include "InteractiveActor.h"
#include "Engine/DamageEvents.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Animation/AnimInstance.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// ALiquidX_Test_SimpleCharacter

ALiquidX_Test_SimpleCharacter::ALiquidX_Test_SimpleCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void ALiquidX_Test_SimpleCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateJetpack(DeltaTime);
	CheckWallRun();
	UpdateWallRun(DeltaTime);
}

void ALiquidX_Test_SimpleCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	bJetpackActive = false;
}

//////////////////////////////////////////////////////////////////////////
// Input

void ALiquidX_Test_SimpleCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ALiquidX_Test_SimpleCharacter::DoubleJump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ALiquidX_Test_SimpleCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ALiquidX_Test_SimpleCharacter::Look);

		// Jetpack
		EnhancedInputComponent->BindAction(JetpackAction, ETriggerEvent::Triggered, this, &ALiquidX_Test_SimpleCharacter::ActivateJetpack);
		EnhancedInputComponent->BindAction(JetpackAction, ETriggerEvent::Completed, this, &ALiquidX_Test_SimpleCharacter::DeactivateJetpack);

		// PickupThrow
		EnhancedInputComponent->BindAction(PickupThrowAction, ETriggerEvent::Triggered, this, &ALiquidX_Test_SimpleCharacter::PickupCube);
		EnhancedInputComponent->BindAction(PickupThrowAction, ETriggerEvent::Completed, this, &ALiquidX_Test_SimpleCharacter::ThrowCube);

		// Interact
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, this, &ALiquidX_Test_SimpleCharacter::Interact);

		// Punch
		EnhancedInputComponent->BindAction(PunchAction, ETriggerEvent::Triggered, this, &ALiquidX_Test_SimpleCharacter::PunchCube);

		// Sprint
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Triggered, this, &ALiquidX_Test_SimpleCharacter::StartSprint);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &ALiquidX_Test_SimpleCharacter::StopSprint);

	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ALiquidX_Test_SimpleCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

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

void ALiquidX_Test_SimpleCharacter::Look(const FInputActionValue& Value)
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

//////Jetpack/////
void ALiquidX_Test_SimpleCharacter::ActivateJetpack()
{
	bJetpackActive = true;
}

void ALiquidX_Test_SimpleCharacter::DeactivateJetpack()
{
	bJetpackActive = false;
}

void ALiquidX_Test_SimpleCharacter::UpdateJetpack(float DeltaTime)
{
	if (bJetpackActive && JetpackFuel > 0)
	{
		// Apply upward force
		GetCharacterMovement()->AddForce(FVector(0, 0, JetpackForce));

		// Consume fuel
		JetpackFuel = FMath::Max(0.0f, JetpackFuel - JetpackFuelConsumptionRate * DeltaTime);

		// Disable gravity while jetpack is active
		GetCharacterMovement()->GravityScale = 0.1f;
	}
	else
	{
		// Refill fuel when jetpack is not in use
		JetpackFuel = FMath::Min(100.0f, JetpackFuel + JetpackFuelRefillRate * DeltaTime);

		// Restore normal gravity
		GetCharacterMovement()->GravityScale = 1.75f;
	}
}

/////Cube/////
void ALiquidX_Test_SimpleCharacter::PickupCube()
{
	if (HeldCube)
	{
		ThrowCube();
		return;
	}

	FVector Start = GetActorLocation();
	float Radius = 150.0f;
	FVector End = Start; // Sphere trace will use the same start and end for radius.

	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	// Perform the sphere trace
	if (GetWorld()->SweepSingleByChannel(
		HitResult,
		Start,
		End,
		FQuat::Identity, // No rotation
		ECC_Visibility,
		FCollisionShape::MakeSphere(Radius),
		QueryParams))
	{
		APickupCube* Cube = Cast<APickupCube>(HitResult.GetActor());
		if (Cube)
		{
			HeldCube = Cube;
			HeldCube->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, "hand_r");
			HeldCube->GetStaticMeshComponent()->SetSimulatePhysics(false);
			HeldCube->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			UE_LOG(LogTemp, Warning, TEXT("Picked up"));
		}
	}

	// Draw a debug sphere to visualize the sphere trace
	DrawDebugSphere(GetWorld(), Start, Radius, 12, FColor::Green, false, 1.0f);

	// Draw a debug line from the character's location to the hit point
	FVector EndPoint = HitResult.TraceEnd;
	DrawDebugLine(GetWorld(), Start, EndPoint, FColor::Red, false, 1.0f);
}

void ALiquidX_Test_SimpleCharacter::ThrowCube()
{
	if (HeldCube)
	{
		// Detach and set physics
		HeldCube->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

		// Store the reference to the cube being thrown
		UStaticMeshComponent* CubeMesh = HeldCube->GetStaticMeshComponent();
		if (CubeMesh)
		{
			// Enable physics simulation
			CubeMesh->SetSimulatePhysics(true);
			CubeMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

			// Calculate throw direction and position
			FVector ThrowDirection = GetActorForwardVector();
			FVector ThrowPosition = GetMesh()->GetSocketLocation(CubeAttachSocketName);

			// Move the cube slightly forward to prevent collision with the character
			HeldCube->SetActorLocation(ThrowPosition + ThrowDirection * 100.0f);

			// Apply the throwing force
			CubeMesh->AddImpulse(ThrowDirection * ThrowForce);

			// Set a timer to disable physics
			FTimerHandle TimerHandle;
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, [CubeMesh]()
				{
					// Disable physics after 1 seconds
					CubeMesh->SetSimulatePhysics(false);
				}, 1.0f, false);

			HeldCube = nullptr;
			UE_LOG(LogTemp, Warning, TEXT("Throw"));
		}
	}
}

/////Interact/////
void ALiquidX_Test_SimpleCharacter::Interact()
{
	FVector Start = GetActorLocation();
	FVector End = Start + GetActorForwardVector() * InteractionRange;

	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, QueryParams))
	{
		AInteractiveActor* InteractiveActor = Cast<AInteractiveActor>(HitResult.GetActor());
		if (InteractiveActor)
		{
			InteractiveActor->Interact();
		}
	}
	DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 1.0f);
}

/////Damage/////
void ALiquidX_Test_SimpleCharacter::PunchCube()
{
	if (PunchMontage)
	{
		// Play the punch animation montage
		PlayAnimMontage(PunchMontage);

		// Schedule the actual punch damage after a short delay
		FTimerHandle TimerHandle;
		GetWorldTimerManager().SetTimer(TimerHandle, this, &ALiquidX_Test_SimpleCharacter::PerformPunchDamage, PunchAnimationDelay, false);
	}
	else
	{
		// If no montage is set, perform the punch immediately
		PerformPunchDamage();
	}
}

void ALiquidX_Test_SimpleCharacter::PerformPunchDamage()
{
	FVector Start = GetActorLocation();
	FVector End = Start + GetActorForwardVector() * InteractionRange;

	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, QueryParams))
	{
		APickupCube* Cube = Cast<APickupCube>(HitResult.GetActor());
		if (Cube)
		{
			FDamageEvent DamageEvent;
			Cube->TakeDamage(PunchDamage, DamageEvent, GetController(), this);
			Cube->GetStaticMeshComponent()->AddImpulse(GetActorForwardVector() * PunchForce);
		}
	}
	DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 1.0f);
}

/////Double jump/////
void ALiquidX_Test_SimpleCharacter::DoubleJump()
{
	if (JumpCount == 0)
	{
		Jump();
		JumpCount++;
	}
	else if (JumpCount == 1 && bCanDoubleJump)
	{
		LaunchCharacter(FVector(0, 0, DoubleJumpForce), false, true);
		JumpCount++;
	}
}

void ALiquidX_Test_SimpleCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	JumpCount = 0;
	bCanDoubleJump = true;
}

/////Sprint/////
void ALiquidX_Test_SimpleCharacter::StartSprint()
{
	bIsSprinting = true;
	GetCharacterMovement()->MaxWalkSpeed *= SprintSpeedMultiplier;
}

void ALiquidX_Test_SimpleCharacter::StopSprint()
{
	bIsSprinting = false;
	GetCharacterMovement()->MaxWalkSpeed = 600.0f;
}


/////Wall run/////
void ALiquidX_Test_SimpleCharacter::CheckWallRun()
{
	if (!GetCharacterMovement()->IsFalling())
	{
		StopWallRun();
		return;
	}

	FVector Start = GetActorLocation();
	FVector Right = GetActorRightVector();
	FVector ForwardOffset = GetActorForwardVector() * 50.0f;

	TArray<FVector> Directions = { Right, -Right };

	for (const FVector& Direction : Directions)
	{
		FVector End = Start + Direction * WallCheckDistance + ForwardOffset;
		FHitResult HitResult;
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this);

		if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, QueryParams))
		{
			StartWallRun();
			WallNormal = HitResult.Normal;
			return;
		}
	}

	StopWallRun();
}

void ALiquidX_Test_SimpleCharacter::StartWallRun()
{
	if (!bIsWallRunning)
	{
		bIsWallRunning = true;
		GetCharacterMovement()->SetMovementMode(MOVE_Flying);
		WallRunTimer = 0.0f;
	}
}

void ALiquidX_Test_SimpleCharacter::StopWallRun()
{
	if (bIsWallRunning)
	{
		bIsWallRunning = false;
		GetCharacterMovement()->SetMovementMode(MOVE_Falling);
	}
}

void ALiquidX_Test_SimpleCharacter::UpdateWallRun(float DeltaTime)
{
	if (bIsWallRunning)
	{
		WallRunTimer += DeltaTime;
		if (WallRunTimer >= WallRunDuration)
		{
			StopWallRun();
			return;
		}

		FVector WallRunDirection = FVector::CrossProduct(WallNormal, FVector::UpVector);
		if (FVector::DotProduct(WallRunDirection, GetActorForwardVector()) < 0)
		{
			WallRunDirection = -WallRunDirection;
		}

		GetCharacterMovement()->Velocity = WallRunDirection * WallRunSpeed;
	}
}