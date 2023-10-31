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

	//Hit contains information about what the raycast hit.
	FHitResult Hit;

	//The length of the ray in units.
	float RayLength = SphereRadius;

	//The Origin of the raycast
	FVector StartLocation = GetActorLocation();

	//FVector fovline1 = StartLocation + (GetActorForwardVector() + SphereRadius); //try and get this to be the cone of vision for the enemy and make it a list so jimmy doesnt end you

	//The EndLocation of the raycast
	FVector EndLocation = StartLocation + (GetActorForwardVector() * RayLength);

	//Collision parameters. The following syntax means that we don't want the trace to be complex
	FCollisionQueryParams CollisionParameters;

	//Perform the line trace
	//The ECollisionChannel parameter is used in order to determine what we are looking for when performing the raycast
	ActorLineTraceSingle(Hit, StartLocation, EndLocation, ECollisionChannel::ECC_WorldDynamic, CollisionParameters);

	//DrawDebugLine is used in order to see the raycast we performed
	//The boolean parameter used here means that we want the lines to be persistent so we can see the actual raycast
	//The last parameter is the width of the lines.
	DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Green, true, -1, 0, 1.f);

	/*
	DrawDebugSphere(GetWorld(), GetActorLocation(), SphereRadius, 20, FColor::Purple, false, -1, 0, 1);

	if (_inTrigArea) 
	{
		DotProduct = FVector::DotProduct(character->GetActorForwardVector(), GetActorForwardVector());

		//dotprod > 0.0 same dir
		//dotpord == perpendicular
		//dotprod < 0.0 opposiote
		UE_LOG(LogTemp, Warning, TEXT("bing bong: %f"), DotProduct); //dotprod = where the character is looking not where the characters pos is, use breain to fix this
		if (DotProduct < 0.0f)
		{
			//inFov = true;
		}

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
			_enterPos = OtherActor->GetActorLocation();
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

