// Copyright Epic Games, Inc. All Rights Reserved.

#include "Editing/BoneRigDetailsCustomization.h"
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

#include "BoneRigComponent.h"
#include "SpineAnimationHandlerComponent.h"



#define LOCTEXT_NAMESPACE "SpinePluginImplementEditor"

//////////////////////////////////////////////////////////////////////////
// FBoneRigDetailsCustomization

TSharedRef<IDetailCustomization> FBoneRigDetailsCustomization::MakeInstance()
{
	
	return MakeShareable(new FBoneRigDetailsCustomization);
}

void FBoneRigDetailsCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	IDetailCategoryBuilder& Category = DetailBuilder.EditCategory("BoneRig");

	IDetailCategoryBuilder& ModeUpdateCategory = DetailBuilder.EditCategory("ModeUpdate");

	const FText CreateBoneRigText = LOCTEXT("CreateBoneRig", "Create Bone Rig");
	const FText RemoveBoneRigText = LOCTEXT("RemoveBoneRig", "Remove Bone Rig");

	const FText SetUpdate_FollowingText = LOCTEXT("SetUpdate_FollowingText", "Set Update_Following");
	const FText SetUpdate_DrivingText = LOCTEXT("SetUpdate_DrivingText", "Set Update_Driving");
	const FText SetUpdate_RagdollUpdatingText = LOCTEXT("SetUpdate_RagdollUpdatingText", "Set Update_RagdollUpdating");
	const FText SetUpdate_NoneText = LOCTEXT("SetUpdate_NoneText", "Set Update_None");

	// Cache set of selected things
	SelectedObjectsList = DetailBuilder.GetSelectedObjects();

	Category.AddCustomRow(CreateBoneRigText, false)
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
		.ToolTipText(CreateBoneRigText)
		.OnClicked(this, &FBoneRigDetailsCustomization::CreateBoneRig)
		.Content()
		[
			SNew(STextBlock)
			.Text(CreateBoneRigText)
		]
		];

	Category.AddCustomRow(RemoveBoneRigText, false)
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
		.ToolTipText(RemoveBoneRigText)
		.OnClicked(this, &FBoneRigDetailsCustomization::RemoveBoneRig)
		.Content()
		[
			SNew(STextBlock)
			.Text(RemoveBoneRigText)
		]
		];


	//------------------------------



	ModeUpdateCategory.AddCustomRow(SetUpdate_FollowingText, false)
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
		.ToolTipText(SetUpdate_FollowingText)
		.OnClicked(this, &FBoneRigDetailsCustomization::SetUpdate_Following)
		.Content()
		[
			SNew(STextBlock)
			.Text(SetUpdate_FollowingText)
		]
		];

	ModeUpdateCategory.AddCustomRow(SetUpdate_DrivingText, false)
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
		.ToolTipText(SetUpdate_DrivingText)
		.OnClicked(this, &FBoneRigDetailsCustomization::SetUpdate_Driving)
		.Content()
		[
			SNew(STextBlock)
			.Text(SetUpdate_DrivingText)
		]
		];

	ModeUpdateCategory.AddCustomRow(SetUpdate_RagdollUpdatingText, false)
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
		.ToolTipText(SetUpdate_RagdollUpdatingText)
		.OnClicked(this, &FBoneRigDetailsCustomization::SetUpdate_RagdollUpdating)
		.Content()
		[
			SNew(STextBlock)
			.Text(SetUpdate_RagdollUpdatingText)
		]
		];

	ModeUpdateCategory.AddCustomRow(SetUpdate_NoneText, false)
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
		.ToolTipText(SetUpdate_NoneText)
		.OnClicked(this, &FBoneRigDetailsCustomization::SetUpdate_None)
		.Content()
		[
			SNew(STextBlock)
			.Text(SetUpdate_NoneText)
		]
		];

	IDetailCategoryBuilder& SettingCategory = DetailBuilder.EditCategory("Setting");


	const FText SetTargetComponentText = LOCTEXT("SetTargetComponent", "Set Target Component");

	SettingCategory.AddCustomRow(SetTargetComponentText, false)
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
		.ToolTipText(SetTargetComponentText)
		.OnClicked(this, &FBoneRigDetailsCustomization::SetTargetComponent)
		.Content()
		[
			SNew(STextBlock)
			.Text(SetTargetComponentText)
		]
		];


}

UBoneRigComponent* FBoneRigDetailsCustomization::GetFirstSelectedBoneRig() const
{
	// Find first selected valid ProcMeshComp
	UBoneRigComponent* Comp = nullptr;
	for (const TWeakObjectPtr<UObject>& Object : SelectedObjectsList)
	{
		UBoneRigComponent* TestComp = Cast<UBoneRigComponent>(Object.Get());
		// See if this one is good
		if (TestComp != nullptr && !TestComp->IsTemplate())
		{
			Comp = TestComp;
			break;
		}
	}

	return Comp;
}

FReply FBoneRigDetailsCustomization::CreateBoneRig() {

	UBoneRigComponent* Comp = GetFirstSelectedBoneRig();
	if (Comp) {
		if (AActor* CompActor = Comp->GetOwner()) {
			if (UActorComponent* TestComp = CompActor->GetComponentByClass(USpineAnimationHandlerComponent::StaticClass())) {
				Comp->CreateRig();
			}
		}
	}

	return FReply::Handled();
}

FReply FBoneRigDetailsCustomization::RemoveBoneRig() {
	UBoneRigComponent* Comp = GetFirstSelectedBoneRig();
	if (Comp) {
		Comp->RemoveRig();
	}

	return FReply::Handled();
}

FReply FBoneRigDetailsCustomization::SetTargetComponent() {
	UBoneRigComponent* Comp = GetFirstSelectedBoneRig();

	if (Comp) {

		if (AActor* Owner = Comp->GetOwner()) {

			TArray<UActorComponent*> Arr;
			Arr = Owner->GetComponentsByClass(USpineAnimationHandlerComponent::StaticClass());

			for (UActorComponent* Test : Arr) {

				if (Test->GetFName() == Comp->TargetComponentName) {

					Comp->SetTargetComponent(Cast<USpineAnimationHandlerComponent>(Test));
					break;
				}
			}
		}
	}

	return FReply::Handled();
}


FReply FBoneRigDetailsCustomization::SetUpdate_Following() {
	UBoneRigComponent* Comp = GetFirstSelectedBoneRig();
	if (Comp) {
		Comp->SetUpdate_Following();
	}

	return FReply::Handled();
}

FReply FBoneRigDetailsCustomization::SetUpdate_Driving() {
	UBoneRigComponent* Comp = GetFirstSelectedBoneRig();
	if (Comp) {
		Comp->SetUpdate_Driving();
	}

	return FReply::Handled();
}

FReply FBoneRigDetailsCustomization::SetUpdate_RagdollUpdating() {
	UBoneRigComponent* Comp = GetFirstSelectedBoneRig();
	if (Comp) {
		Comp->SetUpdate_RagdollUpdating();
	}

	return FReply::Handled();
}

FReply FBoneRigDetailsCustomization::SetUpdate_None() {
	UBoneRigComponent* Comp = GetFirstSelectedBoneRig();
	if (Comp) {
		Comp->SetUpdate_None();
	}

	return FReply::Handled();
}


#undef LOCTEXT_NAMESPACE
