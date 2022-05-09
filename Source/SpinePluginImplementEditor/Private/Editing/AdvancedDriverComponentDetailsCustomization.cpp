// Copyright Epic Games, Inc. All Rights Reserved.

#include "Editing/AdvancedDriverComponentDetailsCustomization.h"
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

#include "AdvancedComponents/SpineAdvancedDriverComponent.h"
#include "SpineAnimationHandlerComponent.h"



#define LOCTEXT_NAMESPACE "SpinePluginImplementEditor"

//////////////////////////////////////////////////////////////////////////
// FAdvancedDriverComponentDetailsCustomization

TSharedRef<IDetailCustomization> FAdvancedDriverComponentDetailsCustomization::MakeInstance()
{
	
	return MakeShareable(new FAdvancedDriverComponentDetailsCustomization);
}

void FAdvancedDriverComponentDetailsCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	SelectedObjectsList = DetailBuilder.GetSelectedObjects();

	// Cache set of selected things
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
		.OnClicked(this, &FAdvancedDriverComponentDetailsCustomization::SetTargetComponent)
		.Content()
		[
			SNew(STextBlock)
			.Text(SetTargetComponentText)
		]
		];

}

USpineAdvancedDriverComponent* FAdvancedDriverComponentDetailsCustomization::GetFirstSelectedBoneDriver() const
{
	// Find first selected valid ProcMeshComp
	USpineAdvancedDriverComponent* Comp = nullptr;
	for (const TWeakObjectPtr<UObject>& Object : SelectedObjectsList)
	{
		USpineAdvancedDriverComponent* TestComp = Cast<USpineAdvancedDriverComponent>(Object.Get());
		// See if this one is good
		if (TestComp != nullptr && !TestComp->IsTemplate())
		{
			Comp = TestComp;
			break;
		}
	}

	return Comp;
}

FReply FAdvancedDriverComponentDetailsCustomization::SetTargetComponent() {

	USpineAdvancedDriverComponent* Comp = GetFirstSelectedBoneDriver();

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



#undef LOCTEXT_NAMESPACE
