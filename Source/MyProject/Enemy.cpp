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

	SphereRadius = 1000.0f;

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
	MyCollisionSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::OnOverlapEnd);
	DrawDebugSphere(GetWorld(), GetActorLocation(), SphereRadius, 20, FColor::Purple, false, -1, 0, 1);

	//ROTATE TO PLAYER POS
	FVector NewLocation = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();
	FVector FacingVector = NewLocation - GetActorLocation();
	FRotator FacingRotate = FacingVector.Rotation();
	FQuat QuatRotation = FQuat(FacingRotate);
	FVector EnemyLocation = this->GetActorLocation();
	//SetActorRotation(QuatRotation, ETeleportType::None);

	//CHASE PLAYER POS IF IN AREA
	//float thisNorm = GetActorLocation().Normalize();
	//float otherNorm = NewLocation.Normalize();


	if (Movement)
	{
		//FVector EnemyLocation = this->GetActorLocation();

		FVector MovementVector = NewLocation - EnemyLocation;

		MovementVector.Normalize();
		FVector CurLoc = GetActorLocation();
		float speed = 400.0f;

		FVector Vel = (MovementVector * speed * DeltaTime);

		SetActorLocationAndRotation((CurLoc + Vel), QuatRotation); // caution means looks around near where player is, patrol is waypoints between back and forth for notes for paths?

		//make states - idle standing chasing

		//float timer = 10.0f;

		timer = timer - DeltaTime;
		//UE_LOG(LogTemp, Warning, TEXT("timer is: %f "), timer);
		if (timer <= 0)
		{
			Destroy();
		}
	}

	if (_takeDmg == true)
	{
		if (character)
		{
			//character->health = character->health - DeltaTime;
			//UE_LOG(LogTemp, Warning, TEXT("health is: %f "), character->health);
			UE_LOG(LogTemp, Warning, TEXT("health is: %f "), character->health);

			FVector _distance;
			float _floatdist = _distance.Dist(NewLocation, EnemyLocation);
			if (_floatdist <= 1000)
			{
				SetActorRotation(QuatRotation);

			}
			else if (_floatdist <= 500)
			{
				character->health = character->health - (DeltaTime);
			}
			else if (_floatdist <= 300)
			{
				character->health = character->health - (DeltaTime * 1.5);
			}
			else if (_floatdist <= 100)
			{
				character->health = character->health - (DeltaTime * 2);
			}
			UE_LOG(LogTemp, Warning, TEXT("distance?: %f "), _floatdist);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("no char "));
		}
	}
}

void AEnemy::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && (OtherActor != this))
	{
		//UE_LOG(LogTemp, Warning, TEXT("other fella is : %s"), *OtherActor->GetName());
		//AMyProjectCharacter* character = Cast<AMyProjectCharacter>(OtherActor);
		character = Cast<AMyProjectCharacter>(OtherActor);

		if (character != nullptr)
		{
			DotProduct = FVector::DotProduct(character->GetActorForwardVector(), GetActorForwardVector());
			//_takeDmg = true;
			//character->health = character->health - 1;

			//UE_LOG(LogTemp, Warning, TEXT("dot prod character and enemy: %f "), DotProduct);

			if (DotProduct < 0.0f)
			{
				Movement = true;
				_takeDmg = true;
				//UE_LOG(LogTemp, Warning, TEXT("in view"));
			}
		}
	}
}

void AEnemy::OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && (OtherActor != this) && OtherComp)
	{
		//UE_LOG(LogTemp, Warning, TEXT("other fella left is : %s"), *OtherActor->GetName());

		character = Cast<AMyProjectCharacter>(OtherActor);

		if (character != nullptr)
		{
			//UE_LOG(LogTemp, Warning, TEXT("other fella left is : %s"), *OtherActor->GetName());
			_takeDmg = false;
		}
	}
}