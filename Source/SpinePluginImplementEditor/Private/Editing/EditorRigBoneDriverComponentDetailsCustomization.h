// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Input/Reply.h"
#include "IDetailCustomization.h"



class IDetailLayoutBuilder;
class UEditorRigBoneDriverComponent;

//////////////////////////////////////////////////////////////////////////
// FBoneRigDetailsCustomization

class FEditorRigBoneDriverComponentDetailsCustomization : public IDetailCustomization
{
public:
	/** Makes a new instance of this detail layout class for a specific detail view requesting it */
	static TSharedRef<IDetailCustomization> MakeInstance();

	// IDetailCustomization interface
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;
	// End of IDetailCustomization interface

	UEditorRigBoneDriverComponent* GetFirstSelectedBoneDriver() const;

protected:


	TArray<TWeakObjectPtr<UObject>> SelectedObjectsList;

	void BuildCollisionSection(IDetailCategoryBuilder& SpriteCategory, IDetailLayoutBuilder& DetailLayout);


	UFUNCTION()
		FReply AddBox();

	UFUNCTION()
		FReply AddSphere();

	UFUNCTION()
		FReply AddCapsule();

	UFUNCTION()
		FReply AddConstraint();

	UFUNCTION()
		FReply FlushAllShapes();
};
