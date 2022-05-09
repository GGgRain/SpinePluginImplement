// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Input/Reply.h"
#include "IDetailCustomization.h"



class IDetailLayoutBuilder;
class USpineAdvancedDriverComponent;

//////////////////////////////////////////////////////////////////////////
// FBoneRigDetailsCustomization

class FAdvancedDriverComponentDetailsCustomization : public IDetailCustomization
{
public:
	/** Makes a new instance of this detail layout class for a specific detail view requesting it */
	static TSharedRef<IDetailCustomization> MakeInstance();

	// IDetailCustomization interface
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;
	// End of IDetailCustomization interface

	USpineAdvancedDriverComponent* GetFirstSelectedBoneDriver() const;

protected:

	UFUNCTION()
		FReply SetTargetComponent();

	TArray<TWeakObjectPtr<UObject>> SelectedObjectsList;

};
