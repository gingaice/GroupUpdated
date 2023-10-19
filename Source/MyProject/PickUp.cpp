// Fill out your copyright notice in the Description page of Project Settings.


#include "PickUp.h"
#include "DrawDebugHelpers.h"
#include "Components/SphereComponent.h"
//#include "MyCharacterTest.h"
#include "MyProjectCharacter.h"

// Sets default values
APickUp::APickUp()
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

	MyCollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &APickUp::OnOverlapBegin);

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
		
		//AMyCharacterTest* character = Cast<AMyCharacterTest>(OtherActor);
		if (OtherActor == character)
		{
		//IsSuperJump = true;
		UE_LOG(LogTemp, Warning, TEXT("other fella is : %s"), *OtherActor->GetName());

		//character->IsSuperJump = true;
			//AMyProjectCharacter* character = Cast<AMyProjectCharacter>(OtherActor);

			character->IsSuperJump = true;

			UE_LOG(LogTemp, Warning, TEXT("correct fella is : %s"), *OtherActor->GetName());

			Destroy();
		}
		 //WORKING CODE JUST DELETE COMMENTS AS ITS FOR SUPER JUMP TESTING FOR SHRINK PLAYERa 
		/*
		FVector scale = character->GetActorScale3D();
		if (OtherActor == character)
		{
			UE_LOG(LogTemp, Warning, TEXT("other fella is : %s"), *OtherActor->GetName());

			character->SetActorScale3D(scale * 0.4f);

			Destroy();
		}
		*/
	}
}