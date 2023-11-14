// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "MyProjectCharacter.generated.h"


UCLASS(config=Game)
class AMyProjectCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
public:
	// Sets default values for this character's properties
	AMyProjectCharacter();

	UPROPERTY(VisibleAnywhere)
	bool IsSuperJump;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SideEffect)
	float _jump = 1600.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SideEffect)
	float _sprint = 1900.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SideEffect)
	float _slip;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Health)
	float health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Health)
	FString levelName;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void MoveForward(float Value);
	void MoveRight(float Value);
	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);

	void StartSprint();
	bool IsSprinting;
	void StopSprint();

	const float NormalMoveSpeed = 600.0f;

	void NormJump();
	void StopJumping();
	bool _canCharge = false;
	bool _IsCharged = false;
	float _jumpTimer = 2.0f;

	void StartSlip();

	UPROPERTY()
	class AMyProjectCharacter* character;

public:
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;


	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};