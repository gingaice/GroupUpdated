// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyStates.h"
#include "Components/SphereComponent.h"
#include "DrawDebugHelpers.h"
#include "MyProjectCharacter.h"

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

	MyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("My Mesh"));
	MyMesh->SetupAttachment(RootComponent);
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
	DrawDebugSphere(GetWorld(), GetActorLocation(), SphereRadius, 20, FColor::Purple, false, -1, 0, 1);

	if (inFov) 
	{
		Chasing(DeltaTime);
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
		UE_LOG(LogTemp, Warning, TEXT("bing bong"));
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
	FVector NewLocation = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();
	FVector FacingVector = NewLocation - GetActorLocation();
	FRotator FacingRotate = FacingVector.Rotation();
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

void AEnemyStates::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && (OtherActor != this))
	{
		character = Cast<AMyProjectCharacter>(OtherActor);

		if (character != nullptr)
		{
			DotProduct = FVector::DotProduct(character->GetActorForwardVector(), GetActorForwardVector());

			if (DotProduct < 0.0f)
			{
				inFov = true;
				//_takeDmg = true;
			}
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
			//inFov = false;
		}
	}
}

