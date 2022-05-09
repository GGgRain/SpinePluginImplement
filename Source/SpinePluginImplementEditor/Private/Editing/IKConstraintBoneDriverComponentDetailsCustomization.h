// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Input/Reply.h"
#include "IDetailCustomization.h"



class IDetailLayoutBuilder;
class UIKConstraintBoneDriverComponent;

//////////////////////////////////////////////////////////////////////////
// FBoneRigDetailsCustomization

class FIKConstraintBoneDriverComponentDetailsCustomization : public IDetailCustomization
{
public:
	/** Makes a new instance of this detail layout class for a specific detail view requesting it */
	static TSharedRef<IDetailCustomization> MakeInstance();

	// IDetailCustomization interface
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;
	// End of IDetailCustomization interface

	UIKConstraintBoneDriverComponent* GetFirstSelectedBoneDriver() const;

protected:

	UFUNCTION()
		FReply SetTargetComponent();

	UFUNCTION()
		FReply StartUpdating();

	UFUNCTION()
		FReply StopUpdating();

	TArray<TWeakObjectPtr<UObject>> SelectedObjectsList;

};
