// Fill out your copyright notice in the Description page of Project Settings.


#include "DoorTele.h"
#include "DrawDebugHelpers.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "MyProjectCharacter.h"

// Sets default values
ADoorTele::ADoorTele()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereRadius = 100.0f;

	MyCollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("My Sphere Component"));
	MyCollisionSphere->InitSphereRadius(SphereRadius);
	MyCollisionSphere->SetCollisionProfileName("Trigger");
	RootComponent = MyCollisionSphere;

	MyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("My Mesh"));
	MyMesh->SetupAttachment(RootComponent);
	
	tele = false;
	//otherDoor->SetupAttachment(RootComponent);

	MyCollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &ADoorTele::OnOverlapBegin);
}

// Called when the game starts or when spawned
void ADoorTele::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADoorTele::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADoorTele::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) 
{
	AMyProjectCharacter* character = Cast<AMyProjectCharacter>(OtherActor);


	if (OtherActor && (OtherActor != this))
	{
		if (otherDoor) 
		{
			UE_LOG(LogTemp, Warning, TEXT("AnObjectIn"));

			//AMyCharacterTest* character = Cast<AMyCharacterTest>(OtherActor);
			if (character && !otherDoor->tele)
			{
				FName levelFName = FName(*levelName);
				UGameplayStatics::OpenLevel(GetWorld(), levelFName);

				//tele = true;
				//UE_LOG(LogTemp, Warning, TEXT("correct fella is : %s"), *OtherActor->GetName());
				//character->SetActorRotation(otherDoor->GetActorRotation());
				//character->GetController()->SetControlRotation(character->GetActorRotation());
				//character->SetActorLocation(otherDoor->GetActorLocation());
			}
		}


		//Destroy();

		//character->IsSuperJump = true;
	}
}
