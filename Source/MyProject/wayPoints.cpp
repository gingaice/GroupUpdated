// Fill out your copyright notice in the Description page of Project Settings.


#include "wayPoints.h"
#include "DrawDebugHelpers.h"
#include "Components/SphereComponent.h"

// Sets default values
AwayPoints::AwayPoints()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereRadius = 100.0f;

	MyCollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("My Sphere Component"));
	MyCollisionSphere->InitSphereRadius(SphereRadius);
	MyCollisionSphere->SetCollisionProfileName("Trigger");
	RootComponent = MyCollisionSphere;
}

// Called when the game starts or when spawned
void AwayPoints::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AwayPoints::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

