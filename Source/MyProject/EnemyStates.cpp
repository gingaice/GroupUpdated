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
	DrawDebugSphere(GetWorld(), GetActorLocation(), SphereRadius, 20, FColor::Purple, false, -1, 0, 1);
	//character = GetWorld()->GetFirstPlayerController();
	if (_inTrigArea)
	{
		EnemyForwardVector = GetActorForwardVector();

		DistanceVector = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation() - GetActorLocation(); // add delay when reacha  destination
		float DistanceVectorSize = DistanceVector.Length();

		EnemyForwardVector.Normalize();
		DistanceVector.Normalize();

		auto DotProductResult = FVector::DotProduct(EnemyForwardVector, DistanceVector);

		if (DotProductResult >= 0.6)
		{
			inFov = true;
		}

		if (inFov) 
		{
			//UE_LOG(LogTemp, Warning, TEXT("bing bong fov"));
			FVector targetVector = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();
			FVector FacingVector = targetVector - GetActorLocation();
			FRotator FacingRotate = FacingVector.Rotation();
			FQuat QuatRotation = FQuat(FacingRotate);
			SetActorRotation(QuatRotation);

			reactionTime = reactionTime - DeltaTime;

			UE_LOG(LogTemp, Warning, TEXT("tiemr:  %f"), reactionTime);
			if (reactionTime <= 0)
			{
				chase = true;
				Chasing(DeltaTime);

			}
		}
		else 
		{
			Alert(DeltaTime);
		}
	}
	else
	{

		if (chase) 
		{
			Chasing(DeltaTime);
		}
		else 
		{
			Patrol(DeltaTime);
		}
	}

}

FVector AEnemyStates::getRandomPtInVolume()
{
	FVector roamExtent = MyCollisionSphere->Bounds.BoxExtent;
	return UKismetMathLibrary::RandomPointInBoundingBox(GetActorLocation(), roamExtent);
}

void AEnemyStates::Patrol(float DeltaTime)
{
	if (ArrWaypoints.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("NO THINGIES"));

		FVector targetVector = getRandomPtInVolume();

		FVector FacingVector = targetVector - GetActorLocation();
		FRotator FacingRotate = FacingVector.Rotation();
		FQuat QuatRotation = FQuat(FacingRotate);
		FVector CurLoc = GetActorLocation();
		FVector _distance;
		float _floatdist = _distance.Dist(targetVector, CurLoc);
		FVector MovementVector = targetVector - CurLoc;
		MovementVector.Normalize();

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

		FVector Vel = (MovementVector * speed * DeltaTime);

		SetActorLocationAndRotation((CurLoc + Vel), QuatRotation);

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
	FVector EnemyLocation = GetActorLocation();
	//FVector NewLocation = character->GetActorLocation(); //gets players pos
	FVector NewLocation = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation(); //gets players pos
	FVector FacingVector = NewLocation - EnemyLocation; //to workout whats inbetween the stuff
	FRotator FacingRotate = FacingVector.Rotation(); //to spin the enemy round to see the player
	FQuat QuatRotation = FQuat(FacingRotate);

	FVector MovementVector = NewLocation - EnemyLocation;

	MovementVector.Normalize();
	FVector CurLoc = GetActorLocation();

	FVector Vel = (MovementVector * speed * DeltaTime);

	SetActorLocationAndRotation((CurLoc + Vel), QuatRotation);


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
			reactionTime = 1.5f;
		}
	}
}


/*
	if (_inTrigArea)
	{
		//if (!inFov)
		//{
		//	FVector EnemyForwardVector = GetActorForwardVector();

		//	FVector DistanceVector = character->GetActorLocation() - GetActorLocation(); //	D
		//	float DistanceVectorSize = DistanceVector.Length(); //  |F|

		//	EnemyForwardVector.Normalize();
		//	//PlayerForwardVector.Normalize();
		//	DistanceVector.Normalize(); // |D|

		//	auto DotProductResult = FVector::DotProduct(EnemyForwardVector, DistanceVector); //dot f.d

		//	if ((DotProductResult >= 0.6))
		//	{
		//		UE_LOG(LogTemp, Warning, TEXT("correctamundo"));
		//		inFov = true;
		//	}
		//GetWorld()->GetFirstPlayerController()
		//}

		EnemyForwardVector = GetActorForwardVector();

		DistanceVector = character->GetActorLocation() - GetActorLocation(); //	D
		float DistanceVectorSize = DistanceVector.Length(); //  |F|

		EnemyForwardVector.Normalize();
		//PlayerForwardVector.Normalize();
		DistanceVector.Normalize(); // |D|

		auto DotProductResult = FVector::DotProduct(EnemyForwardVector, DistanceVector); //dot f.d

		if (DotProductResult >= 0.6)
		{
			inFov = true;
			UE_LOG(LogTemp, Warning, TEXT("ineyes"));
			if (inFov)
			{
				////UE_LOG(LogTemp, Warning, TEXT("bing bong fov"));
				//FVector targetVector = character->GetActorLocation();
				//FVector FacingVector = targetVector - GetActorLocation();
				//FRotator FacingRotate = FacingVector.Rotation();
				//FQuat QuatRotation = FQuat(FacingRotate);
				//SetActorRotation(QuatRotation);

				//reactionTime = reactionTime - DeltaTime;

				//UE_LOG(LogTemp, Warning, TEXT("tiemr:  %f"), reactionTime);
				//if (reactionTime <= 0)
				//{
				//	chase = true;
				//	Chasing(DeltaTime);

				//}
				chase = true;
				Chasing(DeltaTime);
			}
			else
			{
				//UE_LOG(LogTemp, Warning, TEXT("bing bong alsert"));
				Alert(DeltaTime);
			}
		}
	}
	else
	{
		if (chase)
		{
			Chasing(DeltaTime);
		}
		else
		{
			Patrol(DeltaTime);
		}
	}
*/