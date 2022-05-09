// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "SpineRigDataEditorActions.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "SpineRigData.h"
#include "Style/SpinePluginImplementEditorStyle.h"
#include "SpineRigDataEditorToolkit.h"

#define LOCTEXT_NAMESPACE "SpineRigDataEditorActions"


/* FTextAssetActions constructors
 *****************************************************************************/

FSpineRigDataEditorActions::FSpineRigDataEditorActions(EAssetTypeCategories::Type InAssetCategory, TSharedRef<ISlateStyle> InStyle)
	: Style(InStyle), MyAssetCategory(InAssetCategory)
{
}


/* FAssetTypeActions_Base overrides
 *****************************************************************************/

bool FSpineRigDataEditorActions::CanFilter()
{
	return true;
}


void FSpineRigDataEditorActions::GetActions(const TArray<UObject*>& InObjects, FMenuBuilder& MenuBuilder)
{
	FAssetTypeActions_Base::GetActions(InObjects, MenuBuilder);

	auto Assets = GetTypedWeakObjectPtrs<USpineRigData>(InObjects);
}


uint32 FSpineRigDataEditorActions::GetCategories()
{
	return EAssetTypeCategories::MaterialsAndTextures;
}


FText FSpineRigDataEditorActions::GetName() const
{
	return NSLOCTEXT("AssetTypeActions", "AssetTypeActions_SpineRigData", "Spine Rig Data");
}


UClass* FSpineRigDataEditorActions::GetSupportedClass() const
{
	return USpineRigData::StaticClass();
}


FColor FSpineRigDataEditorActions::GetTypeColor() const
{
	return FColor::Red;
}


bool FSpineRigDataEditorActions::HasActions(const TArray<UObject*>& InObjects) const
{
	return true;
}

void FSpineRigDataEditorActions::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor)
{


	for (auto ObjIt = InObjects.CreateConstIterator(); ObjIt; ++ObjIt)
	{
		if (USpineRigData* SpineRigData = Cast<USpineRigData>(*ObjIt))
		{

			TSharedRef<FSpineRigDataEditorToolkit> Toolkit = MakeShareable(new FSpineRigDataEditorToolkit(Style));
			Toolkit->Initialize(SpineRigData, EToolkitMode::Standalone, EditWithinLevelEditor);
		}
	}
	
}


#undef LOCTEXT_NAMESPACE
