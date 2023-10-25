// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProjectCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PickUp.h"

// Sets default values
AMyProjectCharacter::AMyProjectCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
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
	CameraBoom->TargetArmLength = 600.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	health = 10;

	SphereRadius = 150.0f;

	MyCollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("My Sphere Component"));
	MyCollisionSphere->InitSphereRadius(SphereRadius);
	MyCollisionSphere->SetCollisionProfileName("Trigger");
	MyCollisionSphere->SetupAttachment(RootComponent);

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}

// Called to bind functionality to input
void AMyProjectCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AMyProjectCharacter::NormJump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &AMyProjectCharacter::StopJumping);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AMyProjectCharacter::StartSprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AMyProjectCharacter::StopSprint);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMyProjectCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMyProjectCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AMyProjectCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AMyProjectCharacter::LookUpAtRate);

	// handle touch devices
	//PlayerInputComponent->BindTouch(IE_Pressed, this, &AMyCharacterTest::TouchStarted);
	//PlayerInputComponent->BindTouch(IE_Released, this, &AMyCharacterTest::TouchStopped);
}


// Called when the game starts or when spawned
void AMyProjectCharacter::BeginPlay()
{
	Super::BeginPlay();

}
// Called every frame
void AMyProjectCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	MyCollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &AMyProjectCharacter::OnOverlapBegin);
	MyCollisionSphere->OnComponentEndOverlap.AddDynamic(this, &AMyProjectCharacter::OnOverlapEnd);
	DrawDebugSphere(GetWorld(), GetActorLocation(), SphereRadius, 20, FColor::Purple, false, -1, 0, 1);

	if (IsSprinting)
	{
		UE_LOG(LogTemp, Warning, TEXT("The float value is: %f"), GetCharacterMovement()->MaxWalkSpeed);
	}

	if (_justDone)
	{
		_climbTimer = _climbTimer - DeltaTime;
		//UE_LOG(LogTemp, Warning, TEXT("The Climb value is: %f"), _climbTimer);
		if (_climbTimer <= 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("The Climb value is: %f"), _climbTimer);
			UE_LOG(LogTemp, Warning, TEXT("bing bong crash why"));
			MyCollisionSphere->SetSphereRadius(SphereRadius);
			_justDone = false;
			_climbTimer = 3.0f;
		}
	}


	if (_canCharge)
	{
		_jumpTimer = _jumpTimer - DeltaTime;
		UE_LOG(LogTemp, Warning, TEXT("The float value is: %f"), _jumpTimer);
		if (_jumpTimer >= 1.7)
		{
			GetCharacterMovement()->JumpZVelocity = 600.0f;
			_IsCharged = true;
		}
		else if (_jumpTimer <= 0)
		{
			GetCharacterMovement()->JumpZVelocity = 1600.f;
			_IsCharged = true;
		}
	}


	if (health <= 0)
	{
		UGameplayStatics::SetGamePaused(GetWorld(), true);
	}
}

void AMyProjectCharacter::MoveForward(float Value)
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

void AMyProjectCharacter::MoveRight(float Value)
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

void AMyProjectCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AMyProjectCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AMyProjectCharacter::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && (OtherActor != this) && OtherComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("other fella is : %s"), *OtherActor->GetName());
		_mantleClimb = true;

		if (!GetCharacterMovement()->IsMovingOnGround()) //is jumping
		{
			_inAir = true;
		}
		else
		{
			_inAir = false;
		}

		if (_inAir)
		{
			Mantle();
			MyCollisionSphere->SetSphereRadius(0);
		}
	}
}

void AMyProjectCharacter::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && (OtherActor != this))
	{
		UE_LOG(LogTemp, Warning, TEXT("other left fella is : %s"), *OtherActor->GetName());
		//_justDone = true;
		_mantleClimb = false;
		_inAir = false;
	}
}

void AMyProjectCharacter::Mantle()
{
	//UE_LOG(LogTemp, Warning, TEXT("in air")); // check if the object whilst jumping is climable, then do a set location upwards
	if (_mantleClimb)
	{

		if (!_justDone)
		{
			_curMantleUp.X = this->GetActorLocation().X;
			_curMantleUp.Y = this->GetActorLocation().Y;
			_curMantleUp.Z = this->GetActorLocation().Z + 125; // PERHAPS LOOK INTO ADD FORCE TO HAVE IT MOVE MORE SMOOTHLY

			//UE_LOG(LogTemp, Warning, TEXT("The X float value is: %f"), _curMantleUp.X);
			//UE_LOG(LogTemp, Warning, TEXT("The Y float value is: %f"), _curMantleUp.Y);
			//UE_LOG(LogTemp, Warning, TEXT("The Z float value is: %f"), _curMantleUp.Z);

			this->SetActorLocation(_curMantleUp);
			//_climbTimer = 3.0f;
			_justDone = true;
		}

		//MyCollisionSphere->SetSphereRadius(SphereRadius);
		/*
		_curMantleUp.X = character->GetActorLocation().X;
		_curMantleUp.Y = character->GetActorLocation().Y + 20;
		_curMantleUp.Z = character->GetActorLocation().Z;
		UE_LOG(LogTemp, Warning, TEXT("The X float value is: %f"), _curMantleUp.X);
		UE_LOG(LogTemp, Warning, TEXT("The Y float value is: %f"), _curMantleUp.Y);
		UE_LOG(LogTemp, Warning, TEXT("The Z float value is: %f"), _curMantleUp.Z);
		*/
		//character->SetActorLocation(_curMantleUp);
	}
	//MyCollisionSphere->SetSphereRadius(SphereRadius);
}

void AMyProjectCharacter::StartSlip()
{
	IsInputKeyDown = true;
}

void AMyProjectCharacter::StopSlip()
{
	IsInputKeyDown = false;
}

void AMyProjectCharacter::StartSprint()
{
	UE_LOG(LogTemp, Warning, TEXT("The sprint value is %s"), (IsSprinting ? TEXT("true") : TEXT("false")));
	GetCharacterMovement()->MaxWalkSpeed = 1900;
}

void AMyProjectCharacter::StopSprint()
{
	UE_LOG(LogTemp, Warning, TEXT("The sprint value is %s"), (IsSprinting ? TEXT("true") : TEXT("false")));
	GetCharacterMovement()->MaxWalkSpeed = NormalMoveSpeed;
}

void AMyProjectCharacter::NormJump()
{
	float _NormJumpVel = 600.0f;

	if (!IsSuperJump)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Why"));
		GetCharacterMovement()->JumpZVelocity = _NormJumpVel;
		Jump();
	}
	else if (IsSuperJump)
	{
		_canCharge = true;
		//UE_LOG(LogTemp, Warning, TEXT("can charge"));

	}

	/*
	if (!IsSuperJump)
	{
		UE_LOG(LogTemp, Warning, TEXT("Why"));
		GetCharacterMovement()->JumpZVelocity = _NormJumpVel;
		Jump();

		//StopSuperJumping();
	}
	else if (IsSuperJump)
	{
		UE_LOG(LogTemp, Warning, TEXT("Super Jump"));
		GetCharacterMovement()->JumpZVelocity = 1600.f;
		Jump();
	}
	*/
}

void AMyProjectCharacter::StopJumping()
{
	if (_IsCharged)
	{
		UE_LOG(LogTemp, Warning, TEXT("charged"));
		Jump();
		_jumpTimer = 2.0f;
		//GetCharacterMovement()->JumpZVelocity = _NormJumpVel;
		_IsCharged = false;
	}

	_canCharge = false;
	_jumpTimer = 2.0f;
}