// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "LiquidX_Test_SimpleCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class ALiquidX_Test_SimpleCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	/** Jetpack Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JetpackAction;

	/** PickupThrow Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* PickupThrowAction;

	/** Interact Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* InteractAction;

	/** Punch Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* PunchAction;

	/** Sprint Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SprintAction;

public:
	ALiquidX_Test_SimpleCharacter();

	virtual void Tick(float DeltaTime) override;

	// Jetpack functions
	UFUNCTION(BlueprintCallable, Category = "Jetpack")
	void ActivateJetpack();

	UFUNCTION(BlueprintCallable, Category = "Jetpack")
	void DeactivateJetpack();

	// Cube interaction functions
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void PickupCube();

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void ThrowCube();

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void Interact();

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void PunchCube();

	//Double jump function
	UFUNCTION(BlueprintCallable, Category = "Movement")
	void DoubleJump();

	//Sprint function
	UFUNCTION(BlueprintCallable, Category = "Movement")
	void StartSprint();

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void StopSprint();

	//Wall run function
	UFUNCTION(BlueprintCallable, Category = "Movement")
	void CheckWallRun();

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void StartWallRun();

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void StopWallRun();

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);
			

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay();

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

private:
	UFUNCTION()
	virtual void Landed(const FHitResult& Hit) override;

	// Jetpack properties
	UPROPERTY(EditAnywhere, Category = "Jetpack")
	float JetpackForce = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jetpack", meta = (AllowPrivateAccess = "true"))
	float JetpackFuel = 100.0f;

	UPROPERTY(EditAnywhere, Category = "Jetpack")
	float JetpackFuelConsumptionRate = 10.0f;

	UPROPERTY(EditAnywhere, Category = "Jetpack")
	float JetpackFuelRefillRate = 5.0f;

	bool bJetpackActive = false;

	// Cube interaction properties
	UPROPERTY(EditAnywhere, Category = "Interaction")
	float PickupRange = 200.0f;

	UPROPERTY(EditAnywhere, Category = "Interaction")
	float ThrowForce = 1000.0f;

	UPROPERTY(EditAnywhere, Category = "Interaction")
	FName CubeAttachSocketName = "hand_r";

	UPROPERTY()
	class APickupCube* HeldCube;

	// Damage properties
	UPROPERTY(EditAnywhere, Category = "Interaction")
	float InteractionRange = 200.0f;

	UPROPERTY(EditAnywhere, Category = "Interaction")
	float PunchForce = 500.0f;

	UPROPERTY(EditAnywhere, Category = "Interaction")
	float PunchDamage = 10.0f;

	//Double Jump Properties
	UPROPERTY(EditAnywhere, Category = "Movement")
	float DoubleJumpForce = 700.0f;

	bool bCanDoubleJump = false;
	int32 JumpCount = 0;

	void UpdateJetpack(float DeltaTime);

	UPROPERTY(EditAnywhere, Category = "Animation")
	class UAnimMontage* PunchMontage;

	UPROPERTY(EditAnywhere, Category = "Animation")
	float PunchAnimationDelay = 0.2f; 

	void PerformPunchDamage();

	// Sprint properties
	UPROPERTY(EditAnywhere, Category = "Movement")
	float SprintSpeedMultiplier = 1.5f;

	bool bIsSprinting = false;

	// Wall run properties
	UPROPERTY(EditAnywhere, Category = "Movement")
	float WallRunSpeed = 800.0f;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float WallRunDuration = 2.0f;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float WallCheckDistance = 60.0f;

	bool bIsWallRunning = false;
	FVector WallNormal;
	float WallRunTimer = 0.0f;

	void UpdateWallRun(float DeltaTime);
};

