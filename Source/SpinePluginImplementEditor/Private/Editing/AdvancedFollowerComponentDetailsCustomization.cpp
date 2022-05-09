// Copyright Epic Games, Inc. All Rights Reserved.

#include "Editing/AdvancedFollowerComponentDetailsCustomization.h"
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


#include "AdvancedComponents/SpineAdvancedFollowerComponent.h"
#include "SpineAnimationHandlerComponent.h"



#define LOCTEXT_NAMESPACE "SpinePluginImplementEditor"

//////////////////////////////////////////////////////////////////////////
// FAdvancedFollowerComponentDetailsCustomization

TSharedRef<IDetailCustomization> FAdvancedFollowerComponentDetailsCustomization::MakeInstance()
{
	
	return MakeShareable(new FAdvancedFollowerComponentDetailsCustomization);
}

void FAdvancedFollowerComponentDetailsCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	SelectedObjectsList = DetailBuilder.GetSelectedObjects();

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
		.OnClicked(this, &FAdvancedFollowerComponentDetailsCustomization::SetTargetComponent)
		.Content()
		[
			SNew(STextBlock)
			.Text(SetTargetComponentText)
		]
		];

}

USpineAdvancedFollowerComponent* FAdvancedFollowerComponentDetailsCustomization::GetFirstSelectedBoneFollower() const
{
	// Find first selected valid ProcMeshComp
	USpineAdvancedFollowerComponent* Comp = nullptr;
	for (const TWeakObjectPtr<UObject>& Object : SelectedObjectsList)
	{
		USpineAdvancedFollowerComponent* TestComp = Cast<USpineAdvancedFollowerComponent>(Object.Get());
		// See if this one is good
		if (TestComp != nullptr && !TestComp->IsTemplate())
		{
			Comp = TestComp;
			break;
		}
	}

	return Comp;
}

FReply FAdvancedFollowerComponentDetailsCustomization::SetTargetComponent() {
	USpineAdvancedFollowerComponent* Comp = GetFirstSelectedBoneFollower();

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
