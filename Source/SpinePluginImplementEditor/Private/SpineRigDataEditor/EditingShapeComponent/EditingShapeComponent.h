// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"

#include "EditingShapeComponent.generated.h"

class UEditorRigBoneDriverComponent;
/**
 * 
 */
class IRigEditInterface {

public:
	IRigEditInterface() {}

	UPROPERTY()
		class UEditorRigBoneDriverComponent* ParentBoneComponent;

	void NotifyPropertyChange();
};


UCLASS()
class UBoxComponent_Edit : public UBoxComponent , public IRigEditInterface
{
	GENERATED_BODY()
public:
	UBoxComponent_Edit() {
		LineThickness = 0.6;
	}

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

	//virtual FPrimitiveSceneProxy* CreateSceneProxy() override;
};

UCLASS()
class UCapsuleComponent_Edit : public UCapsuleComponent, public IRigEditInterface
{
	GENERATED_BODY()
public:
	UCapsuleComponent_Edit() {
		
	}

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

	//virtual FPrimitiveSceneProxy* CreateSceneProxy() override;
};

UCLASS()
class USphereComponent_Edit : public USphereComponent, public IRigEditInterface
{
	GENERATED_BODY()
public:
	USphereComponent_Edit() {
		
	}

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

	//virtual FPrimitiveSceneProxy* CreateSceneProxy() override;
};

