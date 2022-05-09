// Fill out your copyright notice in the Description page of Project Settings.


#include "SpineRigDataEditor/EditingShapeComponent/EditorPhysicsConstraintComponent.h"

void UEditorPhysicsConstraintComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) {
	Super::PostEditChangeProperty(PropertyChangedEvent);
	NotifyPropertyChange();
}