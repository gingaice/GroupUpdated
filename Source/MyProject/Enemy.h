// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Enemy.generated.h"

class USphereComponent;

UCLASS()
class MYPROJECT_API AEnemy : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	float DotProduct;
public:	
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

	bool Movement;

	float timer = 10.0f;

	bool _takeDmg = false;

	UFUNCTION()
	void ApplyDmg();

	UPROPERTY()
	class AMyProjectCharacter* character;

	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
