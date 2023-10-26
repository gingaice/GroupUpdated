// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "wayPoints.h"
#include "GameFramework/Actor.h"
#include "EnemyStates.generated.h"


class USphereComponent;

UCLASS()
class MYPROJECT_API AEnemyStates : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemyStates();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	float DotProduct;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere)
	class USphereComponent* MyCollisionSphere;

	// declare mesh component
	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* MyMesh;

	UPROPERTY(VisibleAnywhere)
	float SphereRadius;

	bool inFov;

	float timer = 5.0f;

	UPROPERTY(EditAnywhere, Category = "Waypoints")
	TArray<class AwayPoints*> ArrWaypoints;

	UPROPERTY()
	class AMyProjectCharacter* character;

	void Patrol(float DeltaTime);
	int _CurWaypoint;

	void Chasing(float DeltaTime);

	void Alert(float DeltaTime);
	bool _inTrigArea;
	FVector _enterPos;

	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

};