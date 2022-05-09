// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BoneRigComponent.h"
#include "EditorBoneRigComponent.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SPINEPLUGINIMPLEMENTEDITOR_API UEditorBoneRigComponent : public UBoneRigComponent
{
	GENERATED_BODY()

public:


	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Update")
		void UpdateBoneData();

	UFUNCTION(BlueprintCallable,CallInEditor,Category = "Update")
		void UpdateConstraintMixesData();

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Update")
		void UpdateAnimationRootMotionData();

public:

	UFUNCTION()
		void GenerateDefaultRigData();


	UFUNCTION()
		void RecreateRigWithoutFlushingAllComp(USpineAnimationHandlerComponent* Handler);

	UFUNCTION()
		void RefreshCollisionComponents();




	void RequestForRefresh();

	void RefreshAllJointConstraintComponents();

	TWeakPtr<class FSpineRigDataEditorToolkit> ParentToolkit;
};
