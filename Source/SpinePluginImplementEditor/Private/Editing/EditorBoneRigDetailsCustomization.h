// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Input/Reply.h"
#include "IDetailCustomization.h"



class IDetailLayoutBuilder;
class UEditorBoneRigComponent;

//////////////////////////////////////////////////////////////////////////
// FEditorBoneRigDetailsCustomization

class FEditorBoneRigDetailsCustomization : public IDetailCustomization
{
public:
	/** Makes a new instance of this detail layout class for a specific detail view requesting it */
	static TSharedRef<IDetailCustomization> MakeInstance();

	// IDetailCustomization interface
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;
	// End of IDetailCustomization interface

	UEditorBoneRigComponent* GetFirstSelectedBoneRig() const;

protected:

	/*
	UFUNCTION()
		FReply RecreateBoneRig();

	UFUNCTION()
		FReply StartUpdating();

	UFUNCTION()
		FReply StopUpdating();


	UFUNCTION()
		FReply StartRagdollPhysicsUpdating();

	UFUNCTION()
		FReply StopRagdollPhysicsUpdating();
		*/

	UFUNCTION()
		FReply GenerateDefaultRigData();

	TArray<TWeakObjectPtr<UObject>> SelectedObjectsList;

};
