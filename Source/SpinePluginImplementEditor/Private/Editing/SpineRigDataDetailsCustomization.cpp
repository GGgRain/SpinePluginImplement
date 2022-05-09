// Copyright Epic Games, Inc. All Rights Reserved.

#include "Editing/SpineRigDataDetailsCustomization.h"
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



#define LOCTEXT_NAMESPACE "SpinePluginImplementEditor"

//////////////////////////////////////////////////////////////////////////
// FEditorBoneRigDetailsCustomization

TSharedRef<IDetailCustomization> FSpineRigDataDetailsCustomization::MakeInstance()
{
	return MakeShareable(new FDefaultDetailsCustomization);
}

void FSpineRigDataDetailsCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	//IDetailCustomization::CustomizeDetails(DetailBuilder);
}


#undef LOCTEXT_NAMESPACE
