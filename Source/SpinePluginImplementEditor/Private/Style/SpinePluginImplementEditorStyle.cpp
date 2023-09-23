// Copyright Epic Games, Inc. All Rights Reserved.

#include "Style/SpinePluginImplementEditorStyle.h"
#include "Styling/SlateStyleRegistry.h"
#include "Styling/SlateTypes.h"
#include "Styling/CoreStyle.h"
#include "EditorStyleSet.h"
#include "Interfaces/IPluginManager.h"
#include "SlateOptMacros.h"


#define IMAGE_PLUGIN_BRUSH( RelativePath, ... ) FSlateImageBrush( FSpinePluginImplementEditorStyle::InContent( RelativePath, ".png" ), __VA_ARGS__ )
#define IMAGE_BRUSH(RelativePath, ...) FSlateImageBrush(StyleSet->RootToContentDir(RelativePath, TEXT(".png")), __VA_ARGS__)
#define BOX_BRUSH(RelativePath, ...) FSlateBoxBrush(StyleSet->RootToContentDir(RelativePath, TEXT(".png")), __VA_ARGS__)
#define DEFAULT_FONT(...) FCoreStyle::GetDefaultFontStyle(__VA_ARGS__)

FString FSpinePluginImplementEditorStyle::InContent(const FString& RelativePath, const ANSICHAR* Extension)
{
	static FString ContentDir = IPluginManager::Get().FindPlugin(TEXT("SpinePluginImplement"))->GetContentDir();
	return (ContentDir / RelativePath) + Extension;
}

TSharedPtr< FSlateStyleSet > FSpinePluginImplementEditorStyle::StyleSet = nullptr;
TSharedPtr< class ISlateStyle > FSpinePluginImplementEditorStyle::Get() { return StyleSet; }

FName FSpinePluginImplementEditorStyle::GetStyleSetName()
{
	static FName SpinePluginImplementEditorStyle(TEXT("SpinePluginImplementEditorStyle"));
	return SpinePluginImplementEditorStyle;
}

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void FSpinePluginImplementEditorStyle::Initialize()
{
	// Const icon sizes
	const FVector2D Icon8x8(8.0f, 8.0f);
	const FVector2D Icon16x16(16.0f, 16.0f);
	const FVector2D Icon20x20(20.0f, 20.0f);
	const FVector2D Icon40x40(40.0f, 40.0f);

	// Only register once
	if (StyleSet.IsValid())
	{
		return;
	}

	StyleSet = MakeShareable(new FSlateStyleSet(GetStyleSetName()));
	StyleSet->SetContentRoot(FPaths::EngineContentDir() / TEXT("Editor/Slate"));
	StyleSet->SetCoreContentRoot(FPaths::EngineContentDir() / TEXT("Slate"));

	const FTextBlockStyle& NormalText = FEditorStyle::Get().GetWidgetStyle<FTextBlockStyle>("NormalText");

	// Shared editors
	{
		StyleSet->Set("DestructiblePaper2D.Common.ViewportZoomTextStyle", FTextBlockStyle(NormalText)
			.SetFont(DEFAULT_FONT("BoldCondensed", 16))
			);

		StyleSet->Set("DestructiblePaper2D.Common.ViewportTitleTextStyle", FTextBlockStyle(NormalText)
			.SetFont(DEFAULT_FONT("Regular", 18))
			.SetColorAndOpacity(FLinearColor(1.0, 1.0f, 1.0f, 0.5f))
			);

		StyleSet->Set("DestructiblePaper2D.Common.ViewportTitleBackground", new BOX_BRUSH("Old/Graph/GraphTitleBackground", FMargin(0)));
	}



	// Sprite editor
	{
		StyleSet->Set("SpriteEditor.SetShowGrid", new IMAGE_BRUSH(TEXT("Icons/icon_MatEd_Grid_40x"), Icon40x40));
		StyleSet->Set("SpriteEditor.SetShowGrid.Small", new IMAGE_BRUSH(TEXT("Icons/icon_MatEd_Grid_40x"), Icon20x20));
		StyleSet->Set("SpriteEditor.SetShowSourceTexture", new IMAGE_PLUGIN_BRUSH(TEXT("Icons/icon_ShowSpriteSheetButton_40x"), Icon40x40));
		StyleSet->Set("SpriteEditor.SetShowSourceTexture.Small", new IMAGE_PLUGIN_BRUSH(TEXT("Icons/icon_ShowSpriteSheetButton_40x"), Icon20x20));
		StyleSet->Set("SpriteEditor.SetShowBounds", new IMAGE_BRUSH(TEXT("Icons/icon_StaticMeshEd_Bounds_40x"), Icon40x40));
		StyleSet->Set("SpriteEditor.SetShowBounds.Small", new IMAGE_BRUSH(TEXT("Icons/icon_StaticMeshEd_Bounds_40x"), Icon20x20));
		StyleSet->Set("SpriteEditor.SetShowCollision", new IMAGE_BRUSH(TEXT("Icons/icon_StaticMeshEd_Collision_40x"), Icon40x40));
		StyleSet->Set("SpriteEditor.SetShowCollision.Small", new IMAGE_BRUSH(TEXT("Icons/icon_StaticMeshEd_Collision_40x"), Icon20x20));

		StyleSet->Set("SpriteEditor.ExtractSprites", new IMAGE_PLUGIN_BRUSH(TEXT("Icons/SpriteEditor/icon_ExtractSprites_40x"), Icon40x40));
		StyleSet->Set("SpriteEditor.ExtractSprites.Small", new IMAGE_PLUGIN_BRUSH(TEXT("Icons/SpriteEditor/icon_ExtractSprites_40x"), Icon20x20));
		StyleSet->Set("SpriteEditor.ToggleShowRelatedSprites", new IMAGE_PLUGIN_BRUSH(TEXT("Icons/SpriteEditor/icon_ShowOtherSprites_40x"), Icon40x40));
		StyleSet->Set("SpriteEditor.ToggleShowRelatedSprites.Small", new IMAGE_PLUGIN_BRUSH(TEXT("Icons/SpriteEditor/icon_ShowOtherSprites_40x"), Icon20x20));
		StyleSet->Set("SpriteEditor.ToggleShowSpriteNames", new IMAGE_PLUGIN_BRUSH(TEXT("Icons/SpriteEditor/icon_ShowSpriteNames_40x"), Icon40x40));
		StyleSet->Set("SpriteEditor.ToggleShowSpriteNames.Small", new IMAGE_PLUGIN_BRUSH(TEXT("Icons/SpriteEditor/icon_ShowSpriteNames_40x"), Icon20x20));

		StyleSet->Set("SpriteEditor.SetShowSockets", new IMAGE_BRUSH(TEXT("Icons/icon_StaticMeshEd_ShowSockets_40x"), Icon40x40));
		StyleSet->Set("SpriteEditor.SetShowSockets.Small", new IMAGE_BRUSH(TEXT("Icons/icon_StaticMeshEd_ShowSockets_40x"), Icon20x20));
		StyleSet->Set("SpriteEditor.SetShowPivot", new IMAGE_BRUSH(TEXT("Icons/icon_StaticMeshEd_ShowPivot_40x"), Icon40x40));
		StyleSet->Set("SpriteEditor.SetShowPivot.Small", new IMAGE_BRUSH(TEXT("Icons/icon_StaticMeshEd_ShowPivot_40x"), Icon20x20));

		StyleSet->Set("SpriteEditor.EnterViewMode", new IMAGE_PLUGIN_BRUSH(TEXT("Icons/icon_DestructiblePaper2D_ViewSprite_40x"), Icon40x40));
		StyleSet->Set("SpriteEditor.EnterViewMode.Small", new IMAGE_PLUGIN_BRUSH(TEXT("Icons/icon_DestructiblePaper2D_ViewSprite_40x"), Icon20x20));
		StyleSet->Set("SpriteEditor.EnterCollisionEditMode", new IMAGE_PLUGIN_BRUSH("Icons/icon_DestructiblePaper2D_EditCollision_40x", Icon40x40));
		StyleSet->Set("SpriteEditor.EnterCollisionEditMode.Small", new IMAGE_PLUGIN_BRUSH("Icons/icon_DestructiblePaper2D_EditCollision_40x", Icon20x20));
		StyleSet->Set("SpriteEditor.EnterSourceRegionEditMode", new IMAGE_PLUGIN_BRUSH("Icons/icon_DestructiblePaper2D_EditSourceRegion_40x", Icon40x40));
		StyleSet->Set("SpriteEditor.EnterSourceRegionEditMode.Small", new IMAGE_PLUGIN_BRUSH("Icons/icon_DestructiblePaper2D_EditSourceRegion_40x", Icon20x20));
		StyleSet->Set("SpriteEditor.EnterRenderingEditMode", new IMAGE_PLUGIN_BRUSH("Icons/icon_DestructiblePaper2D_RenderGeom_40x", Icon40x40));
		StyleSet->Set("SpriteEditor.EnterRenderingEditMode.Small", new IMAGE_PLUGIN_BRUSH("Icons/icon_DestructiblePaper2D_RenderGeom_40x", Icon20x20));
	}

	// Sprite geometry editor (shared between the sprite editor, tile set editor, etc...)
	{
		StyleSet->Set("SpriteGeometryEditor.AddBoxShape", new IMAGE_PLUGIN_BRUSH(TEXT("Icons/icon_DestructiblePaper2D_AddBoxShape_40x"), Icon40x40));
		StyleSet->Set("SpriteGeometryEditor.AddBoxShape.Small", new IMAGE_PLUGIN_BRUSH(TEXT("Icons/icon_DestructiblePaper2D_AddBoxShape_40x"), Icon20x20));
		StyleSet->Set("SpriteGeometryEditor.AddCircleShape", new IMAGE_PLUGIN_BRUSH(TEXT("Icons/icon_DestructiblePaper2D_AddCircleShape_40x"), Icon40x40));
		StyleSet->Set("SpriteGeometryEditor.AddCircleShape.Small", new IMAGE_PLUGIN_BRUSH(TEXT("Icons/icon_DestructiblePaper2D_AddCircleShape_40x"), Icon20x20));

		StyleSet->Set("SpriteGeometryEditor.ToggleAddPolygonMode", new IMAGE_PLUGIN_BRUSH(TEXT("Icons/icon_DestructiblePaper2D_AddPolygon_40x"), Icon40x40));
		StyleSet->Set("SpriteGeometryEditor.ToggleAddPolygonMode.Small", new IMAGE_PLUGIN_BRUSH(TEXT("Icons/icon_DestructiblePaper2D_AddPolygon_40x"), Icon20x20));

		StyleSet->Set("SpriteGeometryEditor.SnapAllVertices", new IMAGE_PLUGIN_BRUSH(TEXT("Icons/icon_DestructiblePaper2D_SnapToPixelGrid_40x"), Icon40x40));
		StyleSet->Set("SpriteGeometryEditor.SnapAllVertices.Small", new IMAGE_PLUGIN_BRUSH(TEXT("Icons/icon_DestructiblePaper2D_SnapToPixelGrid_40x"), Icon20x20));

		StyleSet->Set("SpriteGeometryEditor.SetShowNormals", new IMAGE_BRUSH(TEXT("Icons/icon_StaticMeshEd_Normals_40x"), Icon40x40));
		StyleSet->Set("SpriteGeometryEditor.SetShowNormals.Small", new IMAGE_BRUSH(TEXT("Icons/icon_StaticMeshEd_Normals_40x"), Icon20x20));
	}

	// Asset actions
	{
		StyleSet->Set("AssetActions.CreateSprite", new IMAGE_PLUGIN_BRUSH(TEXT("Icons/icon_Texture_CreateSprite_16x"), Icon16x16));
		StyleSet->Set("AssetActions.ExtractSprites", new IMAGE_PLUGIN_BRUSH(TEXT("Icons/icon_Texture_ExtractSprites_16x"), Icon16x16));
		StyleSet->Set("AssetActions.ConfigureForRetroSprites", new IMAGE_PLUGIN_BRUSH(TEXT("Icons/icon_Texture_ConfigureForRetroSprites_16x"), Icon16x16));
		StyleSet->Set("AssetActions.CreateTileSet", new IMAGE_PLUGIN_BRUSH(TEXT("Icons/icon_Texture_CreateTileSet_16x"), Icon16x16));
	}

	FSlateStyleRegistry::RegisterSlateStyle(*StyleSet.Get());
};

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef IMAGE_PLUGIN_BRUSH
#undef IMAGE_BRUSH
#undef BOX_BRUSH
#undef DEFAULT_FONT

void FSpinePluginImplementEditorStyle::Shutdown()
{
	if (StyleSet.IsValid())
	{
		FSlateStyleRegistry::UnRegisterSlateStyle(*StyleSet.Get());
		if (StyleSet.IsUnique()) {
			StyleSet.Reset();
		}
	}
}
