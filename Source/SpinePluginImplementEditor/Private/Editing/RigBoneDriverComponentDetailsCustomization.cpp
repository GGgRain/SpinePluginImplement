// Copyright Epic Games, Inc. All Rights Reserved.

#include "Editing/RigBoneDriverComponentDetailsCustomization.h"
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

#include "SpineRigBoneDriverComponent.h"




#define LOCTEXT_NAMESPACE "SpinePluginImplementEditor"

//////////////////////////////////////////////////////////////////////////
// FRigBoneDriverComponentDetailsCustomization

TSharedRef<IDetailCustomization> FRigBoneDriverComponentDetailsCustomization::MakeInstance()
{
	
	return MakeShareable(new FRigBoneDriverComponentDetailsCustomization);
}

void FRigBoneDriverComponentDetailsCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	/*

	const FText RecalibrateText = LOCTEXT("Recalibrate", "Recalibrate");

	// Cache set of selected things

	Category.AddCustomRow(RecalibrateText, false)
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
		.ToolTipText(LOCTEXT("Recalibrate", "Recalibrate"))
		.OnClicked(this, &FRigBoneDriverComponentDetailsCustomization::Recalibrate)
		.Content()
		[
			SNew(STextBlock)
			.Text(RecalibrateText)
		]
		];

	IDetailCategoryBuilder& SpriteCategory = DetailLayout.EditCategory("Sprite", FText::GetEmpty(), ECategoryPriority::Important);
	BuildSpriteSection(SpriteCategory, DetailLayout);
	*/

	SelectedObjectsList = DetailBuilder.GetSelectedObjects();

	IDetailCategoryBuilder& CollisionCategory = DetailBuilder.EditCategory("BoneBodySetup");
	BuildCollisionSection(CollisionCategory, DetailBuilder);
}

void FRigBoneDriverComponentDetailsCustomization::BuildCollisionSection(IDetailCategoryBuilder& SpriteCategory, IDetailLayoutBuilder& DetailLayout)
{
	// Show other normal properties in the sprite category so that desired ordering doesn't get messed up
	SpriteCategory.AddProperty(GET_MEMBER_NAME_CHECKED(USpineRigBoneDriverComponent, BoneName));
	//BuildTextureSection(SpriteCategory, DetailLayout);

	//SpriteCategory.AddProperty(GET_MEMBER_NAME_CHECKED(UDestructiblePaperSprite, GetDefaultMaterial()));

	/*
	// Show/hide the custom pivot point based on the pivot mode
	TSharedPtr<IPropertyHandle> PivotModeProperty = DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(UDestructiblePaperSprite, GetPivotMode()));
	TSharedPtr<IPropertyHandle> CustomPivotPointProperty = DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(UDestructiblePaperSprite, GetCustomPivotPoint()));
	TAttribute<EVisibility> CustomPivotPointVisibility = TAttribute<EVisibility>::Create(TAttribute<EVisibility>::FGetter::CreateSP(this, &FDestructibleSpriteDetailsCustomization::GetCustomPivotVisibility, PivotModeProperty));
	SpriteCategory.AddProperty(PivotModeProperty);
	SpriteCategory.AddProperty(CustomPivotPointProperty).Visibility(CustomPivotPointVisibility);
	*/
}

USpineRigBoneDriverComponent* FRigBoneDriverComponentDetailsCustomization::GetFirstSelectedBoneDriver() const
{
	// Find first selected valid ProcMeshComp
	USpineRigBoneDriverComponent* Comp = nullptr;
	for (const TWeakObjectPtr<UObject>& Object : SelectedObjectsList)
	{
		USpineRigBoneDriverComponent* TestComp = Cast<USpineRigBoneDriverComponent>(Object.Get());
		// See if this one is good
		if (TestComp != nullptr && !TestComp->IsTemplate())
		{
			Comp = TestComp;
			break;
		}
	}

	return Comp;
}




#undef LOCTEXT_NAMESPACE
