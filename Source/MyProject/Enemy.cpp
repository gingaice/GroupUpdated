// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "Components/SphereComponent.h"
#include "DrawDebugHelpers.h"
#include "MyProjectCharacter.h"

// Sets default values
AEnemy::AEnemy()
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

	//MyCollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnOverlapBegin);
	//MyCollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &APickUp::OnOverlapBegin);
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	//SphereRadius = 500.0f;
	//test
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	MyCollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnOverlapBegin);
	DrawDebugSphere(GetWorld(), GetActorLocation(), SphereRadius, 20, FColor::Purple, false, -1, 0, 1);

	//ROTATE TO PLAYER POS
	FVector NewLocation = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();
	FVector FacingVector = NewLocation - GetActorLocation();
	FRotator FacingRotate = FacingVector.Rotation();
	FQuat QuatRotation = FQuat(FacingRotate);
	//SetActorRotation(QuatRotation, ETeleportType::None);

	//CHASE PLAYER POS IF IN AREA
	//float thisNorm = GetActorLocation().Normalize();
	//float otherNorm = NewLocation.Normalize();

	if (Movement) 
	{
		FVector EnemyLocation = this->GetActorLocation();

		//NewLocation is the character

		FVector MovementVector = NewLocation - EnemyLocation;

		MovementVector.Normalize();
		//SetActorRotation(QuatRotation, ETeleportType::None);

		FVector CurLoc = GetActorLocation();
		float speed = 150.0f;

		//SetActorRotation(QuatRotation, ETeleportType::None);
		FVector Vel = (MovementVector * speed * DeltaTime);
		//CurLoc.X += speed * DeltaTime;

		SetActorLocationAndRotation((CurLoc + Vel),QuatRotation);

		//float timer = 10.0f;



		timer = timer - DeltaTime;
		UE_LOG(LogTemp, Warning, TEXT("timer is: %f "), timer);
		if (timer <= 0) 
		{

			Destroy();
		}
	}
}

void AEnemy::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	if (OtherActor && (OtherActor != this))
	{
		UE_LOG(LogTemp, Warning, TEXT("other fella is : %s"), *OtherActor->GetName());

		AMyProjectCharacter* character = Cast<AMyProjectCharacter>(OtherActor);

		if (character != nullptr)
		{
			DotProduct = FVector::DotProduct(character->GetActorForwardVector(), GetActorForwardVector());

			character->health = character->health - 1;

			UE_LOG(LogTemp, Warning, TEXT("dot prod character and enemy: %f "), DotProduct);

			if (DotProduct < 0.0f)
			{
				Movement = true;
				//UE_LOG(LogTemp, Warning, TEXT("in view"));
			}
		}
	}
}