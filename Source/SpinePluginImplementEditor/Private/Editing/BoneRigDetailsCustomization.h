// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Input/Reply.h"
#include "IDetailCustomization.h"



class IDetailLayoutBuilder;
class UBoneRigComponent;

//////////////////////////////////////////////////////////////////////////
// FBoneRigDetailsCustomization

class FBoneRigDetailsCustomization : public IDetailCustomization
{
public:
	/** Makes a new instance of this detail layout class for a specific detail view requesting it */
	static TSharedRef<IDetailCustomization> MakeInstance();

	// IDetailCustomization interface
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;
	// End of IDetailCustomization interface

	UBoneRigComponent* GetFirstSelectedBoneRig() const;

protected:

	UFUNCTION()
		FReply CreateBoneRig();

	UFUNCTION()
		FReply RemoveBoneRig();

	UFUNCTION()
		FReply SetTargetComponent();

	UFUNCTION()
		FReply SetUpdate_Following();

	UFUNCTION()
		FReply SetUpdate_Driving();

	UFUNCTION()
		FReply SetUpdate_RagdollUpdating();

	UFUNCTION()
		FReply SetUpdate_None();

	TArray<TWeakObjectPtr<UObject>> SelectedObjectsList;

};
