// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
//#include "ActorsController.h"
#include "UDP_Component.h"
#include "Ball.generated.h"

UCLASS()
class DRONE_FOLLOWER_API ABall : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABall();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//Set the pose from the Data received and return into the corresponding vectors
	//Position is in cm
	//void SetPose(FCustomPoseData* ReceivedData);
    void SetPose(FVector Position);
	
private:

	UPROPERTY(EditAnywhere, Category = "Geometry")
		USceneComponent* BallVisibleComponent;

	//ActorsController pointer
	//AActorsController* ActorController;
};
