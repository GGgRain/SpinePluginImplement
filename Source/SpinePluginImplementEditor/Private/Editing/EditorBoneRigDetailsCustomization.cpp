// Copyright Epic Games, Inc. All Rights Reserved.

#include "Editing/EditorBoneRigDetailsCustomization.h"
#include "Modules/ModuleManager.h"
#include "Misc/PackageName.h"
#include "Widgets/SNullWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Application/SlateWindowHelper.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"
#include "IAssetTools.h"
#include "AssetToolsModule.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "DetailCategoryBuilder.h"
#include "IDetailsView.h"

#include "EditorBoneRigComponent.h"

#include "Misc/MessageDialog.h"



#define LOCTEXT_NAMESPACE "SpinePluginImplementEditor"

//////////////////////////////////////////////////////////////////////////
// FEditorBoneRigDetailsCustomization

TSharedRef<IDetailCustomization> FEditorBoneRigDetailsCustomization::MakeInstance()
{
	
	return MakeShareable(new FEditorBoneRigDetailsCustomization);
}

void FEditorBoneRigDetailsCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	IDetailCategoryBuilder& Category = DetailBuilder.EditCategory("Generate");


	const FText GenerateDefaultRigDataText = LOCTEXT("GenerateDefaultRigData", "Generate Default Rig Data");


	// Cache set of selected things
	SelectedObjectsList = DetailBuilder.GetSelectedObjects();

	Category.AddCustomRow(GenerateDefaultRigDataText, false)
		.NameContent()
		[
			SNullWidget::NullWidget
		]
	.ValueContent()
		.VAlign(VAlign_Center)
		.MaxDesiredWidth(250)
		[
			SNew(SButton)
			.VAlign(VAlign_Center)
		.ToolTipText(GenerateDefaultRigDataText)
		.ForegroundColor(FColor(0, 120, 255,255))
		.OnClicked(this, &FEditorBoneRigDetailsCustomization::GenerateDefaultRigData)
		.Content()
		[
			SNew(STextBlock)
			.Text(GenerateDefaultRigDataText)
		]
		];

}

UEditorBoneRigComponent* FEditorBoneRigDetailsCustomization::GetFirstSelectedBoneRig() const
{
	// Find first selected valid ProcMeshComp
	UEditorBoneRigComponent* Comp = nullptr;
	for (const TWeakObjectPtr<UObject>& Object : SelectedObjectsList)
	{
		UEditorBoneRigComponent* TestComp = Cast<UEditorBoneRigComponent>(Object.Get());
		// See if this one is good
		if (TestComp != nullptr && !TestComp->IsTemplate())
		{
			Comp = TestComp;
			break;
		}
	}

	return Comp;
}


FReply FEditorBoneRigDetailsCustomization::GenerateDefaultRigData() {
	UEditorBoneRigComponent* Comp = GetFirstSelectedBoneRig();
	if (Comp) {

		const FText GenerateDefaultRigDataWarningTitle =
			LOCTEXT(
				"GenerateDefaultRigDataWarningTitle",
				"THIS IS IMPORTANT"
			);

		const FText GenerateDefaultRigDataWarning = 
			LOCTEXT(
				"GenerateDefaultRigDataWarning",
				"Warning : This function will flush every data we have RN and replace it with auto-generated data... Are you sure about this 100 percents and really really ready to proceed ? "
			);

		
		EAppReturnType::Type Return = FMessageDialog::Open(EAppMsgType::OkCancel, EAppReturnType::No, GenerateDefaultRigDataWarning, &GenerateDefaultRigDataWarningTitle);

		switch (Return) {

		case EAppReturnType::Ok: {
			Comp->GenerateDefaultRigData();
			break;
		}
		case EAppReturnType::Cancel: {
			break;
		}

		}
	}

	return FReply::Handled();
}

/*
FReply FEditorBoneRigDetailsCustomization::RecreateBoneRig() {

	UEditorBoneRigComponent* Comp = GetFirstSelectedBoneRig();
	if (Comp) {
		Comp->RecreateRig();
	}

	return FReply::Handled();
}


FReply FEditorBoneRigDetailsCustomization::StartUpdating() {
	UEditorBoneRigComponent* Comp = GetFirstSelectedBoneRig();
	if (Comp) {
		Comp->StartUpdating();
	}

	return FReply::Handled();
}


FReply FEditorBoneRigDetailsCustomization::StopUpdating() {
	UEditorBoneRigComponent* Comp = GetFirstSelectedBoneRig();
	if (Comp) {
		Comp->StopUpdating();
	}

	return FReply::Handled();
}


FReply FEditorBoneRigDetailsCustomization::StartRagdollPhysicsUpdating() {
	UEditorBoneRigComponent* Comp = GetFirstSelectedBoneRig();
	if (Comp) {
		Comp->StartRagdollPhysicsUpdating();
	}

	return FReply::Handled();
}


FReply FEditorBoneRigDetailsCustomization::StopRagdollPhysicsUpdating() {
	UEditorBoneRigComponent* Comp = GetFirstSelectedBoneRig();
	if (Comp) {
		Comp->StopRagdollPhysicsUpdating();
	}

	return FReply::Handled();
}
*/

#undef LOCTEXT_NAMESPACE
