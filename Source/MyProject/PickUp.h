// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "PickUp.generated.h"

class USphereComponent;

UCLASS()
class MYPROJECT_API APickUp : public AActor
{
	GENERATED_BODY()
public:
	APickUp();

	UPROPERTY(EditAnywhere)
	UBoxComponent* whereToSpawn;

	FVector getRandomPtInVolume();
	void spawnAnEnemy();

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	//APickUp();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// declare collision component
	UPROPERTY(VisibleAnywhere)
		class USphereComponent* MyCollisionSphere;

	// declare mesh component
	UPROPERTY(VisibleAnywhere)
		class UStaticMeshComponent* MyMesh;

	UPROPERTY(VisibleAnywhere)
	float SphereRadius;

	UFUNCTION()
		void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

};
