// Copyright Epic Games, Inc. All Rights Reserved.

#include "Editing/EditorRigBoneDriverComponentDetailsCustomization.h"
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

#include "SpineRigDataEditor/EditorRigBoneDriverComponent.h"




#define LOCTEXT_NAMESPACE "SpinePluginImplementEditor"

//////////////////////////////////////////////////////////////////////////
// FEditorRigBoneDriverComponentDetailsCustomization

TSharedRef<IDetailCustomization> FEditorRigBoneDriverComponentDetailsCustomization::MakeInstance()
{
	
	return MakeShareable(new FEditorRigBoneDriverComponentDetailsCustomization);
}

void FEditorRigBoneDriverComponentDetailsCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{

	IDetailCategoryBuilder& CollisionCategory = DetailBuilder.EditCategory("BoneBodySetup");
	BuildCollisionSection(CollisionCategory, DetailBuilder);

	const FText BoxText = LOCTEXT("AddBox", "Add Box");
	const FText SphereText = LOCTEXT("AddSphere", "Add Sphere");
	const FText CapsuleText = LOCTEXT("AddCapsule", "Add Capsule");

	const FText FlushText = LOCTEXT("FlushAllShapes", "Flush All Shapes");
	const FText ConstraintText = LOCTEXT("AddConstraint", "Add Constraint");

	SelectedObjectsList = DetailBuilder.GetSelectedObjects();

	CollisionCategory.AddCustomRow(BoxText, false)
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
		.ToolTipText(BoxText)
		.OnClicked(this, &FEditorRigBoneDriverComponentDetailsCustomization::AddBox)
		.Content()
		[
			SNew(STextBlock)
			.Text(BoxText)
		]
		];

	CollisionCategory.AddCustomRow(SphereText, false)
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
		.ToolTipText(SphereText)
		.OnClicked(this, &FEditorRigBoneDriverComponentDetailsCustomization::AddSphere)
		.Content()
		[
			SNew(STextBlock)
			.Text(SphereText)
		]
		];

	CollisionCategory.AddCustomRow(CapsuleText, false)
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
		.ToolTipText(CapsuleText)
		.OnClicked(this, &FEditorRigBoneDriverComponentDetailsCustomization::AddCapsule)
		.Content()
		[
			SNew(STextBlock)
			.Text(CapsuleText)
		]
		];

	CollisionCategory.AddCustomRow(ConstraintText, false)
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
		.ToolTipText(ConstraintText)
		.OnClicked(this, &FEditorRigBoneDriverComponentDetailsCustomization::AddConstraint)
		.Content()
		[
			SNew(STextBlock)
			.Text(ConstraintText)
		]
		];


	IDetailCategoryBuilder& AdvancedCategory = DetailBuilder.EditCategory("Advanced");
	BuildCollisionSection(AdvancedCategory, DetailBuilder);

	AdvancedCategory.AddCustomRow(FlushText, false)
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
		.ToolTipText(FlushText)
		.OnClicked(this, &FEditorRigBoneDriverComponentDetailsCustomization::FlushAllShapes)
		.Content()
		[
			SNew(STextBlock)
			.Text(FlushText)
		]
		];



}

void FEditorRigBoneDriverComponentDetailsCustomization::BuildCollisionSection(IDetailCategoryBuilder& SpriteCategory, IDetailLayoutBuilder& DetailLayout)
{

}

UEditorRigBoneDriverComponent* FEditorRigBoneDriverComponentDetailsCustomization::GetFirstSelectedBoneDriver() const
{
	// Find first selected valid ProcMeshComp
	UEditorRigBoneDriverComponent* Comp = nullptr;
	for (const TWeakObjectPtr<UObject>& Object : SelectedObjectsList)
	{
		UEditorRigBoneDriverComponent* TestComp = Cast<UEditorRigBoneDriverComponent>(Object.Get());
		// See if this one is good
		if (TestComp != nullptr && !TestComp->IsTemplate())
		{
			Comp = TestComp;
			break;
		}
	}

	return Comp;
}



FReply FEditorRigBoneDriverComponentDetailsCustomization::AddBox() {
	if (UEditorRigBoneDriverComponent* Comp = GetFirstSelectedBoneDriver()) {
		Comp->AddNewBoxElemToCurrentBoneData();
	}

	return FReply::Handled();
}

FReply FEditorRigBoneDriverComponentDetailsCustomization::AddSphere() {
	if (UEditorRigBoneDriverComponent* Comp = GetFirstSelectedBoneDriver()) {
		Comp->AddNewSphereElemToCurrentBoneData();
	}

	return FReply::Handled();
}

FReply FEditorRigBoneDriverComponentDetailsCustomization::AddCapsule() {
	if (UEditorRigBoneDriverComponent* Comp = GetFirstSelectedBoneDriver()) {
		Comp->AddNewCapsuleElemToCurrentBoneData();
	}

	return FReply::Handled();
}

FReply FEditorRigBoneDriverComponentDetailsCustomization::AddConstraint() {
	if (UEditorRigBoneDriverComponent* Comp = GetFirstSelectedBoneDriver()) {
		Comp->AddNewConstraintBetweenParentBone();
	}

	return FReply::Handled();
}

FReply FEditorRigBoneDriverComponentDetailsCustomization::FlushAllShapes() {
	if (UEditorRigBoneDriverComponent* Comp = GetFirstSelectedBoneDriver()) {

		const FText WarningTitle =
			LOCTEXT(
				"WarningTitle",
				"Warning"
			);

		const FText WarningContext =
			LOCTEXT(
				"WarningContext",
				"This function will flush every shape data. Are you ready to proceed?"
			);


		EAppReturnType::Type Return = FMessageDialog::Open(EAppMsgType::OkCancel, EAppReturnType::No, WarningContext, &WarningTitle);

		switch (Return) {

		case EAppReturnType::Ok: {
			Comp->FlushAllShapes();
			break;
		}
		case EAppReturnType::Cancel: {
			break;
		}

		}
	}

	return FReply::Handled();
}


#undef LOCTEXT_NAMESPACE
