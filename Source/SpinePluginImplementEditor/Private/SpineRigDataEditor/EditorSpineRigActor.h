// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpineAnimationHandlerComponent.h"
#include "EditorBoneRigComponent.h"
#include "EditorSpineRigActor.generated.h"

UCLASS()
class AEditorSpineRigActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEditorSpineRigActor();

	UPROPERTY(EditAnywhere)
		class USpineAnimationHandlerComponent* HandlerComponent;

	UPROPERTY(EditAnywhere)
		class UEditorBoneRigComponent* EditorBoneRigComponent;

	virtual bool ShouldTickIfViewportsOnly() const override;
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


};
