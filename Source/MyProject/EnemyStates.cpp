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
	MyCollisionBox->InitBoxExtent(FVector(300,100,10));
	MyCollisionBox->SetCollisionProfileName("Trigger");
	MyCollisionBox->SetupAttachment(RootComponent);
	MyCollisionBox->SetRelativeLocation(FVector(250, 0, 0));

	static ConstructorHelpers::FObjectFinder<UStaticMesh>MeshAsset(TEXT("/Script/Engine.StaticMesh'/Game/StarterContent/Props/SM_Chair.SM_Chair'")); 
	UStaticMesh* Asset = MeshAsset.Object;
	MyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("My Mesh"));
	MyMesh->SetupAttachment(RootComponent);
	MyMesh->SetStaticMesh(Asset);
	//character = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	PitchValue = 0.f;
	YawValue = 0.f;
	RollValue = 0.f;
	
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

		//FVector PlayerForwardVector = character->GetActorForwardVector(); //	F
		FVector EnemyForwardVector = GetActorForwardVector();
		FVector DistanceVector = GetActorForwardVector() - character->GetActorForwardVector(); //	D
		float DistanceVectorSize = DistanceVector.Length(); //  |F|
		EnemyForwardVector.Normalize();
		//PlayerForwardVector.Normalize();
		DistanceVector.Normalize(); // |D|


		auto DotProductResult = FVector::DotProduct(EnemyForwardVector, DistanceVector); //dot f.d  //change playerforward to enemyforward

		//float _AngleRadian = FMath::Acos(DotProductResult);

		//float AngleDegree = FMath::RadiansToDegrees(_AngleRadian);
		
		 // ignore angle for now try using dot product, if dot < 0 should be in 90 forwards

		//float theta = FMath::Atan2(DotProductResult, DistanceVectorSize * PlayerForwardVectorSize); //0
		//UE_LOG(LogTemp, Warning, TEXT("angle:  %f"), AngleDegree);
		//if ((DotProductResult > 0.0f) && (DistanceVectorSize <= AngleDegree)) // get rid of distance vector check on angle and dot prod should do its job and work
		//{
		//	UE_LOG(LogTemp, Warning, TEXT("correctamundo"));
		//}		
		
		if ((DotProductResult < 0.0f)) // get rid of distance vector check on angle and dot prod should do its job and work
		{
			UE_LOG(LogTemp, Warning, TEXT("correctamundo"));
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

		if (inFov)
		{
			Chasing(DeltaTime);
		}
		else
		{
			Patrol(DeltaTime);
		}
	}
	
}

void AEnemyStates::Patrol(float DeltaTime)
{

	if (ArrWaypoints.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("NO THINGIES"));

		FVector targetVector = GetActorForwardVector();

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

		timer = timer - DeltaTime;
		if (timer <= 0)
		{
			Destroy();
		}
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
	FRotator NewRotation = FRotator(PitchValue, YawValue, RollValue);

	FQuat QuatRotation = FQuat(NewRotation);

	AddActorLocalRotation(QuatRotation, false, 0, ETeleportType::None);
	YawValue += DeltaTime;
}

void AEnemyStates::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && (OtherActor != this))
	{
		character = Cast<AMyProjectCharacter>(OtherActor);

		if (character != nullptr)
		{
			_inTrigArea = true;
			_enterPos = OtherActor->GetActorLocation();
			//_enterRotation = GetActorRotation();
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
