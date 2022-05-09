#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

#include "EditorModeRegistry.h"
#include "EditorModes.h"
#include "ViewportEditMode/RigEditMode.h"

#include "Containers/Array.h"
#include "ISettingsModule.h"
#include "ISettingsSection.h"
#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"
#include "Templates/SharedPointer.h"
#include "Toolkits/AssetEditorToolkit.h"

#include "IAssetTypeActions.h"
#include "IAssetTools.h"
#include "AssetToolsModule.h"

#include "Style/SpinePluginImplementEditorStyle.h"

#include "BoneRigComponent.h"
#include "Editing/BoneRigDetailsCustomization.h"
#include "EditorBoneRigComponent.h"
#include "Editing/EditorBoneRigDetailsCustomization.h"
#include "IKConstraintBoneDriverComponent.h"
#include "Editing/IKConstraintBoneDriverComponentDetailsCustomization.h"

#include "AdvancedComponents/SpineAdvancedDriverComponent.h"
#include "Editing/AdvancedDriverComponentDetailsCustomization.h"
#include "AdvancedComponents/SpineAdvancedFollowerComponent.h"
#include "Editing/AdvancedFollowerComponentDetailsCustomization.h"

#include "SpineRigBoneDriverComponent.h"
#include "Editing/RigBoneDriverComponentDetailsCustomization.h"
#include "Editing/SpineRigDataDetailsCustomization.h"
#include "Editing/EditorRigBoneDriverComponentDetailsCustomization.h"
#include "SpineRigDataEditor/EditorRigBoneDriverComponent.h"

#include "AssetTool/SpineRigDataEditorActions.h"
#include "AssetTool/BoneRigComponentSequenceActions.h"


#include "SpineRigDataEditor/SpineRigDataEditorCommands.h"

#include "Sequence/BoneRigComponentSequenceEditorActorBinding.h"

#include "ISequencerModule.h"


#define LOCTEXT_NAMESPACE "FSpinePluginImplementEditorModule"

class FSpinePluginImplementEditorModule : 
	public IHasMenuExtensibility, 
	public IHasToolBarExtensibility, 
	public IModuleInterface
{
public:

	//~ IHasMenuExtensibility interface

	virtual TSharedPtr<FExtensibilityManager> GetMenuExtensibilityManager() override
	{
		return MenuExtensibilityManager;
	}

public:

	//~ IHasToolBarExtensibility interface

	virtual TSharedPtr<FExtensibilityManager> GetToolBarExtensibilityManager() override
	{
		return ToolBarExtensibilityManager;
	}

public:

	//~ IModuleInterface interface

	virtual void StartupModule() override
	{
		FSpinePluginImplementEditorStyle::Initialize();

		FSpineRigDataEditorCommands::Register();

		RegisterAssetTools();
		RegisterMenuExtensions();

		RegisterClassLayout();
		RegisterEditorObjectBindings();

		FEditorModeRegistry::Get().RegisterMode<FRigEditMode>(
			FRigEditMode::EM_RigEdit,
			LOCTEXT("RigEditMode", "Rig Edit Mode"),
			FSlateIcon(),
			false);

	}

	virtual void ShutdownModule() override
	{

		FSpinePluginImplementEditorStyle::Shutdown();

		FSpineRigDataEditorCommands::Unregister();

		UnregisterAssetTools();
		UnregisterMenuExtensions();

		UnregisterClassLayout();
		UnregisterEditorObjectBindings();


		FEditorModeRegistry::Get().UnregisterMode(FRigEditMode::EM_RigEdit);
	}

	virtual bool SupportsDynamicReloading() override
	{
		return true;
	}

protected:

	/** Registers asset tool actions. */
	void RegisterAssetTools()
	{
		IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();

		RegisterAssetTypeAction(AssetTools, MakeShareable(new FSpineRigDataEditorActions(SpineImpCategoryBit, FSpinePluginImplementEditorStyle::Get().ToSharedRef())));
		RegisterAssetTypeAction(AssetTools, MakeShareable(new FBoneRigComponentSequenceActions(FSpinePluginImplementEditorStyle::Get().ToSharedRef())));
		//RegisterAssetTypeAction(AssetTools, MakeShareable(new FBoneRigEditorActions(Style.ToSharedRef())));
	}

	/**
	 * Registers a single asset type action.
	 *
	 * @param AssetTools The asset tools object to register with.
	 * @param Action The asset type action to register.
	 */
	void RegisterAssetTypeAction(IAssetTools& AssetTools, TSharedRef<IAssetTypeActions> Action)
	{
		AssetTools.RegisterAssetTypeActions(Action);
		RegisteredAssetTypeActions.Add(Action);
	}


	/** Unregisters asset tool actions. */
	void UnregisterAssetTools()
	{
		FAssetToolsModule* AssetToolsModule = FModuleManager::GetModulePtr<FAssetToolsModule>("AssetTools");

		if (AssetToolsModule != nullptr)
		{
			IAssetTools& AssetTools = AssetToolsModule->Get();

			for (auto Action : RegisteredAssetTypeActions)
			{
				AssetTools.UnregisterAssetTypeActions(Action);
			}
		}
	}

	void RegisterClassLayout()
	{
		FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
		PropertyModule.RegisterCustomClassLayout(FName(*UBoneRigComponent::StaticClass()->GetName()), FOnGetDetailCustomizationInstance::CreateStatic(&FBoneRigDetailsCustomization::MakeInstance));
		PropertyModule.RegisterCustomClassLayout(FName(*UEditorBoneRigComponent::StaticClass()->GetName()), FOnGetDetailCustomizationInstance::CreateStatic(&FEditorBoneRigDetailsCustomization::MakeInstance));
		PropertyModule.RegisterCustomClassLayout(FName(*UIKConstraintBoneDriverComponent::StaticClass()->GetName()), FOnGetDetailCustomizationInstance::CreateStatic(&FIKConstraintBoneDriverComponentDetailsCustomization::MakeInstance));
		PropertyModule.RegisterCustomClassLayout(FName(*USpineAdvancedDriverComponent::StaticClass()->GetName()), FOnGetDetailCustomizationInstance::CreateStatic(&FAdvancedDriverComponentDetailsCustomization::MakeInstance));
		PropertyModule.RegisterCustomClassLayout(FName(*USpineRigBoneDriverComponent::StaticClass()->GetName()), FOnGetDetailCustomizationInstance::CreateStatic(&FRigBoneDriverComponentDetailsCustomization::MakeInstance));
		PropertyModule.RegisterCustomClassLayout(FName(*USpineRigData::StaticClass()->GetName()), FOnGetDetailCustomizationInstance::CreateStatic(&FSpineRigDataDetailsCustomization::MakeInstance));
		PropertyModule.RegisterCustomClassLayout(FName(*UEditorRigBoneDriverComponent::StaticClass()->GetName()), FOnGetDetailCustomizationInstance::CreateStatic(&FEditorRigBoneDriverComponentDetailsCustomization::MakeInstance));

	}


	/** Unregisters asset tool actions. */
	void UnregisterClassLayout()
	{
		FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
		PropertyModule.UnregisterCustomClassLayout(FName(*UBoneRigComponent::StaticClass()->GetName()));
		PropertyModule.UnregisterCustomClassLayout(FName(*UEditorBoneRigComponent::StaticClass()->GetName()));
		PropertyModule.UnregisterCustomClassLayout(FName(*UIKConstraintBoneDriverComponent::StaticClass()->GetName()));
		PropertyModule.UnregisterCustomClassLayout(FName(*USpineAdvancedDriverComponent::StaticClass()->GetName()));
		PropertyModule.UnregisterCustomClassLayout(FName(*USpineRigBoneDriverComponent::StaticClass()->GetName()));
		PropertyModule.UnregisterCustomClassLayout(FName(*USpineRigData::StaticClass()->GetName()));
		PropertyModule.UnregisterCustomClassLayout(FName(*UEditorRigBoneDriverComponent::StaticClass()->GetName()));

	}

	/** Register sequencer editor object bindings */
	void RegisterEditorObjectBindings()
	{
		ISequencerModule& SequencerModule = FModuleManager::LoadModuleChecked<ISequencerModule>("Sequencer");
		ActorBindingDelegateHandle = SequencerModule.RegisterEditorObjectBinding(FOnCreateEditorObjectBinding::CreateStatic(&FSpinePluginImplementEditorModule::OnCreateActorBinding));
	}
	
	void UnregisterEditorObjectBindings()
	{
		ISequencerModule* SequencerModule = FModuleManager::GetModulePtr<ISequencerModule>("Sequencer");
		if (SequencerModule)
		{
			SequencerModule->UnRegisterEditorObjectBinding(ActorBindingDelegateHandle);
		}
	}

	static TSharedRef<ISequencerEditorObjectBinding> OnCreateActorBinding(TSharedRef<ISequencer> InSequencer)
	{
		return MakeShareable(new FBoneRigComponentSequenceEditorActorBinding(InSequencer));
	}

protected:

	/** Registers main menu and tool bar menu extensions. */
	void RegisterMenuExtensions()
	{
		MenuExtensibilityManager = MakeShareable(new FExtensibilityManager);
		ToolBarExtensibilityManager = MakeShareable(new FExtensibilityManager);
	}

	/** Unregisters main menu and tool bar menu extensions. */
	void UnregisterMenuExtensions()
	{
		MenuExtensibilityManager.Reset();
		ToolBarExtensibilityManager.Reset();
	}

private:

	/** Holds the menu extensibility manager. */
	TSharedPtr<FExtensibilityManager> MenuExtensibilityManager;

	/** The collection of registered asset type actions. */
	TArray<TSharedRef<IAssetTypeActions>> RegisteredAssetTypeActions;

	/** Holds the tool bar extensibility manager. */
	TSharedPtr<FExtensibilityManager> ToolBarExtensibilityManager;

	FDelegateHandle ActorBindingDelegateHandle;

	EAssetTypeCategories::Type SpineImpCategoryBit = EAssetTypeCategories::Misc;


};

IMPLEMENT_MODULE(FSpinePluginImplementEditorModule, SpinePluginImplementEditor)

#undef LOCTEXT_NAMESPACE