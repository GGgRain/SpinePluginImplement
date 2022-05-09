// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BoneRigComponent.h"
#include "SpineAnimationHandlerComponent.h"
#include "DebugAnimationActor.generated.h"

UCLASS()
class SPINEPLUGINIMPLEMENTEDITOR_API ADebugAnimationActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADebugAnimationActor();

	UPROPERTY(EditAnywhere)
		USpineAnimationHandlerComponent* Handler;
		
	UPROPERTY(EditAnywhere)
		UBoneRigComponent* BoneRig;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
