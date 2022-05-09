// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SpineRigBoneDriverComponent.h"

#include "SpineRigDataEditor/EditingShapeComponent/EditingShapeComponent.h"

#include "SpineRigData.h"

#include "EditorRigBoneDriverComponent.generated.h"

/**
 * 
 */
UCLASS()
class UEditorRigBoneDriverComponent : public USpineRigBoneDriverComponent
{
	GENERATED_BODY()

public:

	void CollectCollisionByRepresentingComponents();

	void RefreshRepresentingComponentsByData();

	void CollectConstraintDataByComponent();

	void RefreshRepresentingConstraintComponentsByData();

	void DeleteRepresentingComponent(USceneComponent* Comp);

	FPrimitiveSceneProxy* CreateSceneProxy() override;

public:

	UPROPERTY(EditAnywhere)
		TArray<UBoxComponent_Edit*> CollisionRefBoxs;

	UPROPERTY(EditAnywhere)
		TArray<UCapsuleComponent_Edit*> CollisionRefCapsules;

	UPROPERTY(EditAnywhere)
		TArray<USphereComponent_Edit*> CollisionRefSpheres;

public:

	void Callback_ChildPropertyChange();

	UPROPERTY(EditAnywhere)
		bool bUpdatable = true;

public:

	UFUNCTION()
		void RefreshBoxes();

	UFUNCTION()
		void RefreshCapsules();

	UFUNCTION()
		void RefreshSpheres();
	
public:
	UPROPERTY(EditAnywhere)
		USpineRigData* BaseDataToModify;

public:

	void AddNewBoxElemToCurrentBoneData();

	void AddNewSphereElemToCurrentBoneData();
	
	void AddNewCapsuleElemToCurrentBoneData();

	void AddNewConstraintBetweenParentBone();

	void FlushAllShapes();

	void RequestForRefresh();

};
