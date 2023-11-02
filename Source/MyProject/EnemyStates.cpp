// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyStates.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "DrawDebugHelpers.h"
#include "MyProjectCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AEnemyStates::AEnemyStates()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereRadius = 500.0f;

	MyCollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("My Sphere Component"));
	MyCollisionSphere->InitSphereRadius(SphereRadius);
	MyCollisionSphere->SetCollisionProfileName("Trigger");
	RootComponent = MyCollisionSphere;

	MyCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("My Box Component"));
	MyCollisionBox->InitBoxExtent(FVector(500,100,10));
	MyCollisionBox->SetCollisionProfileName("Trigger");
	MyCollisionBox->SetupAttachment(RootComponent);

	MyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("My Mesh"));
	MyMesh->SetupAttachment(RootComponent);

	//character = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	
}

// Called when the game starts or when spawned
void AEnemyStates::BeginPlay()
{
	Super::BeginPlay();

	if (ArrWaypoints.Num() >= 2)
	{
		_CurWaypoint = 0;
	}
}

// Called every frame
void AEnemyStates::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	MyCollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemyStates::OnOverlapBegin);
	MyCollisionSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemyStates::OnOverlapEnd);
	MyCollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AEnemyStates::OnOverlapBeginBox);
	MyCollisionBox->OnComponentEndOverlap.AddDynamic(this, &AEnemyStates::OnOverlapEndBox);
	DrawDebugSphere(GetWorld(), GetActorLocation(), SphereRadius, 20, FColor::Purple, false, -1, 0, 1);
	/*
	float Radius = SphereRadius;

	if (_inTrigArea)
	{
		//dotprod > 0.0 same dir
		//dotpord == perpendicular
		//dotprod < 0.0 opposiote

		//FVector PlayerForwardVector = character->GetActorForwardVector(); //	F
		//FVector DistanceVector = GetActorForwardVector() - character->GetActorForwardVector(); //	D

		//float DistanceVectorSize = DistanceVector.Length(); //  |F|
		//float PlayerForwardVectorSize = PlayerForwardVector.Length(); // |D|
		////check the paint drawing for comment explain

		//auto DotProductResult = FVector::DotProduct(PlayerForwardVector, DistanceVector); //dot f.d
		//float theta = FMath::Atan2(DotProductResult, DistanceVectorSize * PlayerForwardVectorSize); //0

		FVector PlayerForwardVector = character->GetActorForwardVector(); //	F
		FVector DistanceVector = GetActorForwardVector() - character->GetActorForwardVector(); //	D
		float DistanceVectorSize = DistanceVector.Length(); //  |F|
		PlayerForwardVector.Normalize();
		DistanceVector.Normalize(); // |D|


		auto DotProductResult = FVector::DotProduct(PlayerForwardVector, DistanceVector); //dot f.d

		float _AngleRadian = FMath::Acos(DotProductResult);

		float AngleDegree = FMath::RadiansToDegrees(_AngleRadian);

		//float theta = FMath::Atan2(DotProductResult, DistanceVectorSize * PlayerForwardVectorSize); //0
		UE_LOG(LogTemp, Warning, TEXT("angle:  %f"), AngleDegree);
		if ((DotProductResult < 0.0f) && (DistanceVectorSize <= AngleDegree))
		{
			UE_LOG(LogTemp, Warning, TEXT("correctamundo"));

			//inFov = true;
			//FMath::Acos(45.0F);
		}

		//if ((DotProductResult < 0.0f) && (DistanceVectorSize <= theta))
		//{
		//	UE_LOG(LogTemp, Warning, TEXT("correctamundo"));

		//	//inFov = true;
		//	//FMath::Acos(45.0F);
		//}

		if (inFov)
		{
			UE_LOG(LogTemp, Warning, TEXT("bing bong fov"));
			Chasing(DeltaTime);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("bing bong alsert"));
			Alert(DeltaTime);
		}
	}
	else
	{
		//perhaps have the character spin around slowly before going back into patrol here at the end of alert call the spin as i have already made the fvector shit there

		if (inFov)
		{
			Chasing(DeltaTime);
		}
		else
		{
			Patrol(DeltaTime);
		}

		//Patrol(DeltaTime);
	}
	*/

	if (_inTrigArea)
	{
		UE_LOG(LogTemp, Warning, TEXT("intrig"));

		Alert(DeltaTime);

		if (_inBoxTrigArea)
		{
			inFov = true;

			UE_LOG(LogTemp, Warning, TEXT("inboxtrig"));
			if (inFov)
			{
				//UE_LOG(LogTemp, Warning, TEXT("bing bong fov"));
				Chasing(DeltaTime);
			}
		}

		//UE_LOG(LogTemp, Warning, TEXT("angle:  %f"), angle);
	}
	else
	{
		Patrol(DeltaTime);
	}
}

void AEnemyStates::Patrol(float DeltaTime)
{

	if (ArrWaypoints.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("NO THINGIES"));
	}
	else
	{
		FVector targetVector = ArrWaypoints[_CurWaypoint]->GetActorLocation();

		FVector FacingVector = targetVector - GetActorLocation();
		FRotator FacingRotate = FacingVector.Rotation();
		FQuat QuatRotation = FQuat(FacingRotate);
		FVector CurLoc = GetActorLocation();
		FVector _distance;
		float _floatdist = _distance.Dist(targetVector, CurLoc);
		FVector MovementVector = targetVector - CurLoc;

		MovementVector.Normalize();
		float speed = 400.0f;

		FVector Vel = (MovementVector * speed * DeltaTime);

		SetActorLocationAndRotation((CurLoc + Vel), QuatRotation);
		//UE_LOG(LogTemp, Warning, TEXT("bing bong: %f"), _floatdist);

		if (_floatdist <= 5)
		{
			if ((_CurWaypoint + 1) == ArrWaypoints.Num())
			{
				_CurWaypoint = 0;
			}
			else
			{
				_CurWaypoint = _CurWaypoint + 1;
			}
			//targetVector = ArrWaypoints[0]->GetActorLocation();
		}

	}
}

void AEnemyStates::Chasing(float DeltaTime)
{
	//ROTATE TO PLAYER POS
	FVector NewLocation = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation(); //gets players pos
	FVector FacingVector = NewLocation - GetActorLocation(); //to workout whats inbetween the stuff
	FRotator FacingRotate = FacingVector.Rotation(); //to spin the enemy round to see the player
	FQuat QuatRotation = FQuat(FacingRotate);
	FVector EnemyLocation = this->GetActorLocation();

	FVector MovementVector = NewLocation - EnemyLocation;

	MovementVector.Normalize();
	FVector CurLoc = GetActorLocation();
	float speed = 400.0f;

	FVector Vel = (MovementVector * speed * DeltaTime);

	SetActorLocationAndRotation((CurLoc + Vel), QuatRotation); // caution means looks around near where player is, patrol is waypoints between back and forth for notes for paths?

	//make states - idle standing chasing

	timer = timer - DeltaTime;
	if (timer <= 0)
	{
		Destroy();
	}
}

void AEnemyStates::Alert(float DeltaTime)
{
	//UE_LOG(LogTemp, Warning, TEXT("bing bongers"));
	//FVector NewLocation = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();
	FVector LastSeen = character->GetActorForwardVector();
	// _enterpos for the entry point of contact do like go there lol
	//find a way to spin the player before moving them that way
}

void AEnemyStates::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && (OtherActor != this))
	{
		character = Cast<AMyProjectCharacter>(OtherActor);

		if (character != nullptr)
		{
			_inTrigArea = true;

			//DotProduct = FVector::DotProduct(character->GetActorForwardVector(), GetActorForwardVector());

			//if (DotProduct < 0.0f)
			//{
			//	inFov = true;
			//}

		}
	}
}

void AEnemyStates::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && (OtherActor != this) && OtherComp)
	{

		character = Cast<AMyProjectCharacter>(OtherActor);

		if (character != nullptr)
		{
			_inTrigArea = false;
			//inFov = false;
		}
	}
}

void AEnemyStates::OnOverlapBeginBox(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && (OtherActor != this))
	{
		character = Cast<AMyProjectCharacter>(OtherActor);

		if (character != nullptr)
		{
			_inBoxTrigArea = true;

			//DotProduct = FVector::DotProduct(character->GetActorForwardVector(), GetActorForwardVector());

			//if (DotProduct < 0.0f)
			//{
			//	inFov = true;
			//}

		}
	}
}

void AEnemyStates::OnOverlapEndBox(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && (OtherActor != this) && OtherComp)
	{

		character = Cast<AMyProjectCharacter>(OtherActor);

		if (character != nullptr)
		{
			_inBoxTrigArea = false;
			//inFov = false;
		}
	}
}
