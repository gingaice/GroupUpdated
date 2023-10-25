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
	
}

// Called every frame
void AEnemyStates::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	MyCollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemyStates::OnOverlapBegin);
	MyCollisionSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemyStates::OnOverlapEnd);
	DrawDebugSphere(GetWorld(), GetActorLocation(), SphereRadius, 20, FColor::Purple, false, -1, 0, 1);

	Patrol(DeltaTime);
}

void AEnemyStates::Patrol(float DeltaTime)
{
	if (ArrWaypoints.IsEmpty()) 
	{
		UE_LOG(LogTemp, Warning, TEXT("bing bong"));
	}
	else 
	{
		FVector targetVector = ArrWaypoints[1]->GetActorLocation();

		FVector FacingVector = targetVector - GetActorLocation();
		FRotator FacingRotate = FacingVector.Rotation();
		FQuat QuatRotation = FQuat(FacingRotate);
		FVector CurLoc = GetActorLocation();
		
		FVector MovementVector = targetVector - CurLoc;

		MovementVector.Normalize();
		float speed = 400.0f;

		FVector Vel = (MovementVector * speed * DeltaTime);

		SetActorLocationAndRotation((CurLoc + Vel), QuatRotation);
	}
}

void AEnemyStates::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}

void AEnemyStates::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

