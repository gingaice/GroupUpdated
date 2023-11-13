// Fill out your copyright notice in the Description page of Project Settings.


#include "PickUp.h"
#include "DrawDebugHelpers.h"
#include "Components/SphereComponent.h"
#include "MyProjectCharacter.h"
#include "EnemyStates.h"
#include "Kismet/KismetMathLibrary.h"	//for random in vol

//#include "MyCharacterTest.h"

// Sets default values
APickUp::APickUp()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create a Box for the spawn volume.
	whereToSpawn = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	whereToSpawn->SetBoxExtent(FVector(1000.0, 500.0, 20.0));
	whereToSpawn->SetupAttachment(RootComponent);


	SphereRadius = 100.0f;

	MyCollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("My Sphere Component"));
	MyCollisionSphere->InitSphereRadius(SphereRadius);
	MyCollisionSphere->SetCollisionProfileName("Trigger");
	RootComponent = MyCollisionSphere;

	MyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("My Mesh"));
	MyMesh->SetupAttachment(RootComponent);

	MyCollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &APickUp::OnOverlapBegin);

}

void APickUp::spawnAnEnemy()
{
	if (GetWorld())
	{
		FActorSpawnParameters spawnParams;
		spawnParams.Owner = this;
		spawnParams.Instigator = GetInstigator();
		spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		FVector spawnLoc = getRandomPtInVolume();
		FRotator rot = FRotator::ZeroRotator;
		GetWorld()->SpawnActor<AEnemyStates>(AEnemyStates::StaticClass(), spawnLoc, rot, spawnParams);
	}
}

FVector APickUp::getRandomPtInVolume()
{
	FVector spawnOrigin = whereToSpawn->Bounds.Origin;
	FVector spawnExtent = whereToSpawn->Bounds.BoxExtent;
	return UKismetMathLibrary::RandomPointInBoundingBox(spawnOrigin, spawnExtent);
}
// Called when the game starts or when spawned
void APickUp::BeginPlay()
{
	Super::BeginPlay();

	//DrawDebugBox(GetWorld(), GetActorLocation(), GetActorScale() * 100, FColor::Purple, true, -1, 0, 5);
}

// Called every frame
void APickUp::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	DrawDebugSphere(GetWorld(), GetActorLocation(), SphereRadius, 20, FColor::Purple, false, -1, 0, 1);
}

void APickUp::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AMyProjectCharacter* character = Cast<AMyProjectCharacter>(OtherActor);


	if (OtherActor && (OtherActor != this))
	{
		UE_LOG(LogTemp, Warning, TEXT("AnObjectIn"));

		if (OtherActor == character)
		{
			if (ActorHasTag("Slip")) 
			{
				UE_LOG(LogTemp, Warning, TEXT("other fella is : %s"), *OtherActor->GetName());

				character->_slip = 500;

				for (int i = 0; i < 3; i++)
				{
					spawnAnEnemy();

				}

				Destroy();
			}

			if (ActorHasTag("Jump"))
			{
				//IsSuperJump = true;
				UE_LOG(LogTemp, Warning, TEXT("other fella is : %s"), *OtherActor->GetName());

				//character->IsSuperJump = true;
					//AMyProjectCharacter* character = Cast<AMyProjectCharacter>(OtherActor);

				character->IsSuperJump = true;

				UE_LOG(LogTemp, Warning, TEXT("correct fella is : %s"), *OtherActor->GetName());


				for (int i = 0; i < 3; i++)
				{
					spawnAnEnemy();

				}

				Destroy();
			}			
			
			if (ActorHasTag("Small"))
			{
				FVector scale = character->GetActorScale3D();
				UE_LOG(LogTemp, Warning, TEXT("other fella is : %s"), *OtherActor->GetName());

				character->SetActorScale3D(scale * 0.4f);

				for (int i = 0; i < 3; i++)
				{
					spawnAnEnemy();

				}

				Destroy();
			}
		}
	}
}