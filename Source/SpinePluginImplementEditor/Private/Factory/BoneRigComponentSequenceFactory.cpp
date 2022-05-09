// Copyright Epic Games, Inc. All Rights Reserved.

#include "BoneRigComponentSequenceFactory.h"
#include "Sequencer/BoneRigComponentSequence.h"
#include "MovieScene.h"
#include "MovieSceneToolsProjectSettings.h"

#define LOCTEXT_NAMESPACE "MovieSceneFactory"

UBoneRigComponentSequenceFactory::UBoneRigComponentSequenceFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UBoneRigComponentSequence::StaticClass();
}

UObject* UBoneRigComponentSequenceFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	UBoneRigComponentSequence* NewSequence = NewObject<UBoneRigComponentSequence>(InParent, Name, Flags | RF_Transactional);
	NewSequence->Initialize();
	
	// Set up some sensible defaults
	const UMovieSceneToolsProjectSettings* ProjectSettings = GetDefault<UMovieSceneToolsProjectSettings>();

	FFrameRate TickResolution = NewSequence->GetMovieScene()->GetTickResolution();
	NewSequence->GetMovieScene()->SetPlaybackRange((ProjectSettings->DefaultStartTime*TickResolution).FloorToFrame(), (ProjectSettings->DefaultDuration*TickResolution).FloorToFrame().Value);

	return NewSequence;
}

bool UBoneRigComponentSequenceFactory::ShouldShowInNewMenu() const
{
	return true;
}

#undef LOCTEXT_NAMESPACE
