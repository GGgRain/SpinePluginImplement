// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "SpineRigDataEditorToolkit.h"
#include "Editor.h"
#include "EditorReimportHandler.h"
#include "EditorStyleSet.h"
#include "SSingleObjectDetailsPanel.h"
#include "EditorViewportClient.h"
#include "SEditorViewport.h"
#include "SSpineRigDataViewport.h"
#include "SCommonEditorViewportToolbarBase.h"
#include "SpineRigDataEditor/EditorSpineRigActor.h"
#include "SpineRigDataEditorViewportClient.h"
#include "PropertyEditorModule.h"
#include "Editing/DefaultDetailsCustomization.h"
#include "Editing/SpineRigDataDetailsCustomization.h"
#include "SpineRigDataEditor/EditorRigBoneDriverComponent.h"

#include "PropertyEditorModule.h"
#include "AdvancedPreviewSceneModule.h"

#include "UObject/NameTypes.h"
#include "Widgets/Docking/SDockTab.h"




#define LOCTEXT_NAMESPACE "SpineRigDataEditorToolkit"

DEFINE_LOG_CATEGORY_STATIC(LogTextAssetEditor, Log, All);

namespace EditorTapIDs
{
	static const FName AppIdentifier("SpineRigDataEditorApp");
	static const FName DetailsID("DetailsID");
	static const FName ViewportID("ViewportID");
	static const FName HierarchyPreviewID("HierarchyPreviewID");
}


class SSpineRigDataPropertiesTabBody : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SSpineRigDataPropertiesTabBody) {}
	SLATE_ARGUMENT(TWeakPtr<FSpineRigDataEditorToolkit>, InParentToolkits)
		SLATE_END_ARGS()

public:

	TWeakPtr<class FSpineRigDataEditorToolkit> SpineRigDataEditorToolkitPtr;

	TSharedPtr<class IDetailsView> PropertyView;


public:

	void Construct(const FArguments& InArgs)
	{
		SpineRigDataEditorToolkitPtr = InArgs._InParentToolkits;

		FDetailsViewArgs DetailsViewArgs(false, false, true, FDetailsViewArgs::HideNameArea, true);
		DetailsViewArgs.HostCommandList = SpineRigDataEditorToolkitPtr.Pin()->GetToolkitCommands();
		DetailsViewArgs.HostTabManager = SpineRigDataEditorToolkitPtr.Pin()->GetTabManager();
		DetailsViewArgs.bAllowSearch = false;

		FPropertyEditorModule& PropertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
		PropertyView = PropertyEditorModule.CreateDetailView(DetailsViewArgs);

		PropertyView->RegisterInstancedCustomPropertyLayout(USpineRigData::StaticClass(), FOnGetDetailCustomizationInstance::CreateStatic(&FSpineRigDataDetailsCustomization::MakeInstance));
		PropertyView->SetObject(SpineRigDataEditorToolkitPtr.Pin()->GetEditingAsset());
	}

	TSharedPtr<class IDetailsView> GetPropertyView() {
		return PropertyView;
	}


	// 
	void SetPropertyWindowContents(TArray<UObject*> Objects) {
		if (FSlateApplication::IsInitialized())
		{
			check(PropertyView.IsValid());
			PropertyView->SetObjects(Objects);
		}
	}

	void UpdateShowingObject(TArray<UObject*> Objects) {
		if (Objects.Num()) {
			SetPropertyWindowContents(Objects);
		}
		else {
			TArray<UObject*> Arr;
			Arr.Add(SpineRigDataEditorToolkitPtr.Pin()->GetEditingAsset());

			SetPropertyWindowContents(Arr);
		}
	}
};


/* FTextAssetEditorToolkit structors
 *****************************************************************************/

FSpineRigDataEditorToolkit::FSpineRigDataEditorToolkit(const TSharedRef<ISlateStyle>& InStyle) :
	Style(InStyle), 
	EdtingSpineRigData(nullptr)
{ }


FSpineRigDataEditorToolkit::~FSpineRigDataEditorToolkit()
{
	FReimportManager::Instance()->OnPreReimport().RemoveAll(this);
	FReimportManager::Instance()->OnPostReimport().RemoveAll(this);

	EdtingSpineRigData->OnPropertyChange.RemoveAll(this);

	GEditor->UnregisterForUndo(this);
}


/* FTextAssetEditorToolkit interface
 *****************************************************************************/

void FSpineRigDataEditorToolkit::Initialize(USpineRigData* InSpineRigData, const EToolkitMode::Type InMode, const TSharedPtr<class IToolkitHost>& InToolkitHost)
{
	EdtingSpineRigData = InSpineRigData;

	EdtingSpineRigData->OnPropertyChange.AddRaw(this, &FSpineRigDataEditorToolkit::OnEditingRigDataPropertyChanged);

	// Support undo/redo
	EdtingSpineRigData->SetFlags(RF_Transactional);
	GEditor->RegisterForUndo(this);

	UE_LOG(LogTemp, Warning, TEXT("FSpineRigDataEditorToolkit / Initialize"));

	SSCSEditorPtr = SNew(SSCSEditor)
		.EditorMode(EComponentEditorMode::ActorInstance)
		.AllowEditing(this, &FSpineRigDataEditorToolkit::GetAllowComponentTreeEditing)
		.ActorContext(this, &FSpineRigDataEditorToolkit::GetActorContext)
		.OnSelectionUpdated(this, &FSpineRigDataEditorToolkit::OnSelectionUpdate)
		.OnItemDoubleClicked(this, &FSpineRigDataEditorToolkit::OnItemDoubleClickedOnTreeView);


	ViewportPtr = SNew(SSpineRigDataViewport)
		.InParentToolkits(SharedThis(this))
		.ObjectToEdit(EdtingSpineRigData);


	FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	const FDetailsViewArgs DetailsViewArgs(false, false , true, FDetailsViewArgs::ObjectsUseNameArea, false);
	DetailPtr = PropertyEditorModule.CreateDetailView(DetailsViewArgs);



	FAdvancedPreviewSceneModule& AdvancedPreviewSceneModule = FModuleManager::LoadModuleChecked<FAdvancedPreviewSceneModule>("AdvancedPreviewScene");
	PreviewSettingsWidget = AdvancedPreviewSceneModule.CreateAdvancedPreviewSceneSettingsWidget(ViewportPtr->GetPreviewScene());
	

	const TSharedRef<FTabManager::FLayout> StandaloneDefaultLayout = FTabManager::NewLayout("Standalone_EditorTapIDs_Layout_v2")
		->AddArea
		(
			FTabManager::NewPrimaryArea()
			->SetOrientation(Orient_Vertical)
			->Split
			(
				FTabManager::NewStack()
				->SetSizeCoefficient(0.1f)
				->SetHideTabWell(true)
				->AddTab(GetToolbarTabId(), ETabState::OpenedTab)
			)
			->Split
			(
				FTabManager::NewSplitter()
				->SetOrientation(Orient_Horizontal)
				->SetSizeCoefficient(0.9f)
				->Split
				(
					FTabManager::NewStack()
					->SetSizeCoefficient(0.3f)
					->SetHideTabWell(true)
					->AddTab(EditorTapIDs::HierarchyPreviewID, ETabState::OpenedTab)
				)
				->Split
				(
					FTabManager::NewStack()
					->SetSizeCoefficient(0.8f)
					->SetHideTabWell(true)
					->AddTab(EditorTapIDs::ViewportID, ETabState::OpenedTab)
				)
				->Split
				(
					FTabManager::NewStack()
					->SetSizeCoefficient(0.75f)
					->AddTab(EditorTapIDs::DetailsID, ETabState::OpenedTab)
				)
			)
		);
	

	UE_LOG(LogTemp, Warning, TEXT("FSpineRigDataEditorToolkit / InitAssetEditor"));

	FAssetEditorToolkit::InitAssetEditor(
		InMode,
		InToolkitHost,
		EditorTapIDs::AppIdentifier,
		StandaloneDefaultLayout,
		true,
		true,
		EdtingSpineRigData
	);

	ViewportPtr->SpineRigDataEditorViewportClientptr.Get()->ActivateEditMode();

	ExtendToolbar();

	RegenerateMenusAndToolbars();

	SSCSEditorPtr.Get()->RefreshSelectionDetails();
	SSCSEditorPtr.Get()->UpdateTree(true);
	SSCSEditorPtr.Get()->Invalidate(EInvalidateWidgetReason::All);
}


void FSpineRigDataEditorToolkit::ExtendToolbar()
{
	TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);

	AddToolbarExtender(ToolbarExtender);
}



/* FAssetEditorToolkit interface
 *****************************************************************************/

FString FSpineRigDataEditorToolkit::GetDocumentationLink() const
{
	return FString(TEXT("Sorry, there is no documentation for this plugin."));
}


void FSpineRigDataEditorToolkit::RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	WorkspaceMenuCategory = InTabManager->AddLocalWorkspaceMenuCategory(LOCTEXT("WorkspaceMenu_SpineRigDataEditor", "Spine Rig Data Editor"));
	auto WorkspaceMenuCategoryRef = WorkspaceMenuCategory.ToSharedRef();

	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);

	
	InTabManager->RegisterTabSpawner(EditorTapIDs::ViewportID, FOnSpawnTab::CreateSP(this, &FSpineRigDataEditorToolkit::SpawnTab_Viewport, EditorTapIDs::ViewportID))
		.SetDisplayName(LOCTEXT("EditorTapIDsTabName", "Editor"))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.Tabs.Viewports"));

	InTabManager->RegisterTabSpawner(EditorTapIDs::HierarchyPreviewID, FOnSpawnTab::CreateSP(this, &FSpineRigDataEditorToolkit::SpawnTab_HierarchyPreview, EditorTapIDs::HierarchyPreviewID))
		.SetDisplayName(LOCTEXT("HierarchyLabel", "Hierarchy Preview"))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.Tabs.Modes"));

	InTabManager->RegisterTabSpawner(EditorTapIDs::DetailsID, FOnSpawnTab::CreateSP(this, &FSpineRigDataEditorToolkit::SpawnTab_Details , EditorTapIDs::DetailsID))
		.SetDisplayName(LOCTEXT("DetailsTabLabel", "Details"))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.Tabs.Details"));

	UE_LOG(LogTemp, Warning, TEXT("FSpineRigDataEditorToolkit / RegisterTabSpawners"));
		
}


void FSpineRigDataEditorToolkit::UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	FAssetEditorToolkit::UnregisterTabSpawners(InTabManager);

	InTabManager->UnregisterTabSpawner(EditorTapIDs::ViewportID);
	InTabManager->UnregisterTabSpawner(EditorTapIDs::HierarchyPreviewID);
	InTabManager->UnregisterTabSpawner(EditorTapIDs::DetailsID);
}


/* IToolkit interface
 *****************************************************************************/

FText FSpineRigDataEditorToolkit::GetBaseToolkitName() const
{
	return LOCTEXT("AppLabel", "Spine Rig Data Editor");
}


FName FSpineRigDataEditorToolkit::GetToolkitFName() const
{
	return FName("SpineRigDataEditor");
}


FLinearColor FSpineRigDataEditorToolkit::GetWorldCentricTabColorScale() const
{
	return FLinearColor(0.3f, 0.2f, 0.5f, 0.5f);
}


FString FSpineRigDataEditorToolkit::GetWorldCentricTabPrefix() const
{
	return LOCTEXT("WorldCentricTabPrefix", "Spine Rig Data").ToString();
}


/* FGCObject interface
 *****************************************************************************/

void FSpineRigDataEditorToolkit::AddReferencedObjects(FReferenceCollector& Collector)
{
	Collector.AddReferencedObject(EdtingSpineRigData);
}


/* FEditorUndoClient interface
*****************************************************************************/

void FSpineRigDataEditorToolkit::PostUndo(bool bSuccess)
{ }


void FSpineRigDataEditorToolkit::PostRedo(bool bSuccess)
{
	PostUndo(bSuccess);
}


/* FTextAssetEditorToolkit callbacks
 *****************************************************************************/

TSharedRef<SDockTab> FSpineRigDataEditorToolkit::SpawnTab_Viewport(const FSpawnTabArgs& Args, FName TabIdentifier)
{

	TSharedPtr<SWidget> TabWidget = SNullWidget::NullWidget;

	if (TabIdentifier == EditorTapIDs::ViewportID)
	{
		TabWidget = ViewportPtr;
	}

	return SNew(SDockTab)
		.TabRole(ETabRole::PanelTab)
		[
			TabWidget.ToSharedRef()
		];

	UE_LOG(LogTemp, Warning, TEXT("SpawnTab_Viewport Succesefully Called!"));
}

TSharedRef<SDockTab> FSpineRigDataEditorToolkit::SpawnTab_HierarchyPreview(const FSpawnTabArgs& Args , FName TabIdentifier)
{
	TSharedPtr<FSpineRigDataEditorToolkit> SpineRigDataEditorToolkitPtr = SharedThis(this);

	// Spawn the tab
	return SNew(SDockTab)
		.Icon(FEditorStyle::GetBrush("LevelEditor.Tabs.Modes"))
		.Label(LOCTEXT("Hierarchy Preview", "Hierarchy Preview"))
		[
			SSCSEditorPtr.ToSharedRef()
		];
	UE_LOG(LogTemp, Warning, TEXT("Called!"));
}

TSharedRef<SDockTab> FSpineRigDataEditorToolkit::SpawnTab_Details(const FSpawnTabArgs& Args, FName TabIdentifier)
{

	TSharedPtr<FSpineRigDataEditorToolkit> SpineRigDataEditorToolkitPtr = SharedThis(this);

	return SNew(SDockTab)
		.Icon(FEditorStyle::GetBrush("GenericEditor.Tabs.Properties"))
		.Label(LOCTEXT("GenericDetailsTitle", "Details"))
		.TabColorScale(GetTabColorScale())
		[
			DetailPtr.ToSharedRef()
		];
}



AActor* FSpineRigDataEditorToolkit::GetActorContext() const {
	AActor* ReturnVal = nullptr;
	if (ViewportPtr.IsValid()) {
		if (ViewportPtr.Get()->EditorSpineRigActor) {
			ReturnVal = Cast<AActor>(ViewportPtr.Get()->EditorSpineRigActor);
		}
	}
	return ReturnVal;
}

bool FSpineRigDataEditorToolkit::GetAllowComponentTreeEditing() const 
{
	return false;
}

void FSpineRigDataEditorToolkit::OnSelectionUpdate(const TArray<FSCSEditorTreeNodePtrType>& Arr) {
	TArray<UObject*> Objects;

	Objects.Empty();

	TSet<UActorComponent*> Comps;
	if (SSpineRigDataViewport* Viewport = ViewportPtr.Get()) {
		Comps = Viewport->EditorSpineRigActor->GetComponents();
	}

	if (Arr.Num()) {
		for (FSCSEditorTreeNodePtrType NodeShared : Arr) {
			if (FSCSEditorTreeNode* Node = NodeShared.Get()) {
				const UActorComponent* Obj = Node->GetObject<UActorComponent>();


				if (Comps.Contains(Obj)) {
					Objects.Add(*Comps.Find(Obj));
				}
				else {
					Objects.Empty();
					Objects.Add(GetEditingAsset());
					break;
				}
			}
		}
	}
	else {
		Objects.Add(GetEditingAsset());
	}

	DetailPtr.Get()->SetObjects(Objects);

	UpdateDetails();

	ViewportPtr->SpineRigDataEditorViewportClientptr.Get()->SelectObjects(Objects);
}

void FSpineRigDataEditorToolkit::OnItemDoubleClickedOnTreeView(const FSCSEditorTreeNodePtrType Type) {

}


void FSpineRigDataEditorToolkit::OnEditingRigDataPropertyChanged() {
	RefreshEditorData();
}

void FSpineRigDataEditorToolkit::RefreshEditorData() {

	ViewportPtr.Get()->UpdateScene();

	SSCSEditorPtr.Get()->RefreshSelectionDetails();
	SSCSEditorPtr.Get()->UpdateTree(true);
	SSCSEditorPtr.Get()->Invalidate(EInvalidateWidgetReason::All);

	UpdateDetails();

}

void FSpineRigDataEditorToolkit::UpdateDetails() {
	TArray<TWeakObjectPtr<UObject>> Arr = DetailPtr.Get()->GetSelectedObjects();

	for (TWeakObjectPtr<UObject> Test : Arr) {
		if (!Test.IsValid()) {
			Arr.Remove(Test);
		}
	}

	DetailPtr.Get()->SetObjects(Arr);
}

void FSpineRigDataEditorToolkit::OnEditModeSelectionChanged(TArray<USceneComponent*> SelectedComponentsArr) {
	SSCSEditorPtr.Get()->ClearSelection();

	TArray<TWeakObjectPtr<UObject>> ObjArr;
	for (USceneComponent* TestComp : SelectedComponentsArr) {
		ObjArr.Add(TestComp);


		if (FSCSEditorTreeNodePtrType Node = SSCSEditorPtr.Get()->GetNodeFromActorComponent(TestComp)) {
			SSCSEditorPtr.Get()->SelectNode(Node, true);
		}
	}


	DetailPtr.Get()->SetObjects(ObjArr);

	UpdateDetails();
}


void FSpineRigDataEditorToolkit::OnTryDeleteObjects(TArray<USceneComponent*> SelectedComponentsArr) {
	if (USpineRigData* Data =  GetEditingAsset()) {

		TArray<UEditorRigBoneDriverComponent*> ArrForRefresh;

		for (USceneComponent* TestComp : SelectedComponentsArr) {

			if (USceneComponent* ParentComp = TestComp->GetAttachParent()) {

				if (UEditorRigBoneDriverComponent* CastedParentComp = Cast<UEditorRigBoneDriverComponent>(ParentComp)) {

					CastedParentComp->DeleteRepresentingComponent(TestComp);

					if (!ArrForRefresh.Contains(CastedParentComp)) {
						ArrForRefresh.Add(CastedParentComp);
					}

				}
			}
		}


		for (UEditorRigBoneDriverComponent* TestComp : ArrForRefresh) {
			TestComp->CollectCollisionByRepresentingComponents();
		}

		SSCSEditorPtr.Get()->ClearSelection();

		RefreshEditorData();

	}
}
#undef LOCTEXT_NAMESPACE
