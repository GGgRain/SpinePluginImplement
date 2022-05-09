// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "EditorPhysicsConstraintComponent.generated.h"

/**
 * 
 */
UCLASS()
class UEditorPhysicsConstraintComponent : public UPhysicsConstraintComponent , public IRigEditInterface
{
	GENERATED_BODY()

		virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

	
	
		
};
