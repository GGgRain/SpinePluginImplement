// Copyright Epic Games, Inc. All Rights Reserved.

#include "BoneRigComponentSequenceEditorBlueprintLibrary.h"

#include "ISequencer.h"
#include "IKeyArea.h"
#include "LevelSequence.h"

#include "Modules/ModuleManager.h"
#include "LevelEditor.h"
#include "Subsystems/AssetEditorSubsystem.h"

#include "MovieSceneCommonHelpers.h"
#include "MovieSceneSection.h"
#include "Channels/MovieSceneChannelProxy.h"

namespace
{
	static TWeakPtr<ISequencer> CurrentSequencer;
}

bool UBoneRigComponentSequenceEditorBlueprintLibrary::OpenLevelSequence(ULevelSequence* LevelSequence)
{
	if (LevelSequence)
	{
		return GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->OpenEditorForAsset(LevelSequence);
	}

	return false;
}

ULevelSequence* UBoneRigComponentSequenceEditorBlueprintLibrary::GetCurrentLevelSequence()
{
	if (CurrentSequencer.IsValid())
	{
		return Cast<ULevelSequence>(CurrentSequencer.Pin()->GetRootMovieSceneSequence());
	}
	return nullptr;
}

void UBoneRigComponentSequenceEditorBlueprintLibrary::CloseLevelSequence()
{
	if (CurrentSequencer.IsValid())
	{
		GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->CloseAllEditorsForAsset(CurrentSequencer.Pin()->GetRootMovieSceneSequence());
	}
}

void UBoneRigComponentSequenceEditorBlueprintLibrary::Play()
{
	const bool bTogglePlay = false;
	if (CurrentSequencer.IsValid())
	{
		CurrentSequencer.Pin()->OnPlay(bTogglePlay);
	}
}

void UBoneRigComponentSequenceEditorBlueprintLibrary::Pause()
{
	if (CurrentSequencer.IsValid())
	{
		CurrentSequencer.Pin()->Pause();
	}
}

void UBoneRigComponentSequenceEditorBlueprintLibrary::SetCurrentTime(int32 NewFrame)
{
	if (CurrentSequencer.IsValid())
	{
		FFrameRate DisplayRate = CurrentSequencer.Pin()->GetFocusedDisplayRate();
		FFrameRate TickResolution = CurrentSequencer.Pin()->GetFocusedTickResolution();

		CurrentSequencer.Pin()->SetGlobalTime(ConvertFrameTime(NewFrame, DisplayRate, TickResolution));
	}
}

int32 UBoneRigComponentSequenceEditorBlueprintLibrary::GetCurrentTime()
{
	if (CurrentSequencer.IsValid())
	{
		FFrameRate DisplayRate = CurrentSequencer.Pin()->GetFocusedDisplayRate();
		FFrameRate TickResolution = CurrentSequencer.Pin()->GetFocusedTickResolution();

		return ConvertFrameTime(CurrentSequencer.Pin()->GetGlobalTime().Time, TickResolution, DisplayRate).FloorToFrame().Value;
	}
	return 0;
}

bool UBoneRigComponentSequenceEditorBlueprintLibrary::IsPlaying()
{
	if (CurrentSequencer.IsValid())
	{
		return CurrentSequencer.Pin()->GetPlaybackStatus() == EMovieScenePlayerStatus::Playing;
	}
	return false;
}

TArray<UMovieSceneTrack*> UBoneRigComponentSequenceEditorBlueprintLibrary::GetSelectedTracks()
{
	TArray<UMovieSceneTrack*> OutSelectedTracks;
	if (CurrentSequencer.IsValid())
	{
		CurrentSequencer.Pin()->GetSelectedTracks(OutSelectedTracks);
	}
	return OutSelectedTracks;
}

TArray<UMovieSceneSection*> UBoneRigComponentSequenceEditorBlueprintLibrary::GetSelectedSections()
{
	TArray<UMovieSceneSection*> OutSelectedSections;
	if (CurrentSequencer.IsValid())
	{
		CurrentSequencer.Pin()->GetSelectedSections(OutSelectedSections);
	}
	return OutSelectedSections;
}

TArray<FBoneRigComponentSequencerChannelProxy> UBoneRigComponentSequenceEditorBlueprintLibrary::GetSelectedChannels()
{
	TArray<FBoneRigComponentSequencerChannelProxy> OutSelectedChannels;
	if (CurrentSequencer.IsValid())
	{
		TArray<const IKeyArea*> SelectedKeyAreas;

		CurrentSequencer.Pin()->GetSelectedKeyAreas(SelectedKeyAreas);

		for (const IKeyArea* KeyArea : SelectedKeyAreas)
		{
			if (KeyArea)
			{
				FBoneRigComponentSequencerChannelProxy ChannelProxy(KeyArea->GetName(), KeyArea->GetOwningSection());
				OutSelectedChannels.Add(ChannelProxy);
			}
		}
	}
	return OutSelectedChannels;
}

TArray<UMovieSceneFolder*> UBoneRigComponentSequenceEditorBlueprintLibrary::GetSelectedFolders()
{
	TArray<UMovieSceneFolder*> OutSelectedFolders;
	if (CurrentSequencer.IsValid())
	{
		CurrentSequencer.Pin()->GetSelectedFolders(OutSelectedFolders);
	}
	return OutSelectedFolders;
}

TArray<FGuid> UBoneRigComponentSequenceEditorBlueprintLibrary::GetSelectedObjects()
{
	TArray<FGuid> OutSelectedGuids;
	if (CurrentSequencer.IsValid())
	{
		CurrentSequencer.Pin()->GetSelectedObjects(OutSelectedGuids);
	}
	return OutSelectedGuids;
}

void UBoneRigComponentSequenceEditorBlueprintLibrary::SelectTracks(const TArray<UMovieSceneTrack*>& Tracks)
{
	if (CurrentSequencer.IsValid())
	{
		for (UMovieSceneTrack* Track : Tracks)
		{
			CurrentSequencer.Pin()->SelectTrack(Track);
		}
	}
}

void UBoneRigComponentSequenceEditorBlueprintLibrary::SelectSections(const TArray<UMovieSceneSection*>& Sections)
{
	if (CurrentSequencer.IsValid())
	{
		for (UMovieSceneSection* Section : Sections)
		{
			CurrentSequencer.Pin()->SelectSection(Section);
		}
	}
}

void UBoneRigComponentSequenceEditorBlueprintLibrary::SelectChannels(const TArray<FBoneRigComponentSequencerChannelProxy>& Channels)
{
	if (CurrentSequencer.IsValid())
	{
		for (FBoneRigComponentSequencerChannelProxy ChannelProxy : Channels)
		{
			UMovieSceneSection* Section = ChannelProxy.Section;
			if (Section)
			{
				TArray<FName> ChannelNames;
				ChannelNames.Add(ChannelProxy.ChannelName);
				CurrentSequencer.Pin()->SelectByChannels(Section, ChannelNames, false, true);
			}
		}
	}
}

void UBoneRigComponentSequenceEditorBlueprintLibrary::SelectFolders(const TArray<UMovieSceneFolder*>& Folders)
{
	if (CurrentSequencer.IsValid())
	{
		for (UMovieSceneFolder* Folder : Folders)
		{
			CurrentSequencer.Pin()->SelectFolder(Folder);
		}
	}
}

void UBoneRigComponentSequenceEditorBlueprintLibrary::SelectObjects(TArray<FGuid> ObjectBindings)
{
	if (CurrentSequencer.IsValid())
	{
		for (FGuid ObjectBinding : ObjectBindings)
		{
			CurrentSequencer.Pin()->SelectObject(ObjectBinding);
		}
	}
}

void UBoneRigComponentSequenceEditorBlueprintLibrary::EmptySelection()
{
	if (CurrentSequencer.IsValid())
	{
		CurrentSequencer.Pin()->EmptySelection();
	}
}

void UBoneRigComponentSequenceEditorBlueprintLibrary::SetSequencer(TSharedRef<ISequencer> InSequencer)
{
	CurrentSequencer = TWeakPtr<ISequencer>(InSequencer);
}

void UBoneRigComponentSequenceEditorBlueprintLibrary::RefreshCurrentLevelSequence()
{
	if (CurrentSequencer.IsValid())
	{
		CurrentSequencer.Pin()->NotifyMovieSceneDataChanged(EMovieSceneDataChangeType::Unknown);
	}
}
	
TArray<UObject*> UBoneRigComponentSequenceEditorBlueprintLibrary::GetBoundObjects(FMovieSceneObjectBindingID ObjectBinding)
{
	TArray<UObject*> BoundObjects;
	if (CurrentSequencer.IsValid())
	{
		for (TWeakObjectPtr<> WeakObject : CurrentSequencer.Pin()->FindBoundObjects(ObjectBinding.GetGuid(), ObjectBinding.GetRelativeSequenceID()))
		{
			if (WeakObject.IsValid())
			{
				BoundObjects.Add(WeakObject.Get());
			}
		}

	}
	return BoundObjects;
}


bool UBoneRigComponentSequenceEditorBlueprintLibrary::IsLevelSequenceLocked()
{
	if (CurrentSequencer.IsValid())
	{
		TSharedPtr<ISequencer> Sequencer = CurrentSequencer.Pin();
		UMovieSceneSequence* FocusedMovieSceneSequence = Sequencer->GetFocusedMovieSceneSequence();
		if (FocusedMovieSceneSequence) 
		{
			if (FocusedMovieSceneSequence->GetMovieScene()->IsReadOnly()) 
			{
				return true;
			}
			else
			{
				TArray<UMovieScene*> DescendantMovieScenes;
				MovieSceneHelpers::GetDescendantMovieScenes(Sequencer->GetFocusedMovieSceneSequence(), DescendantMovieScenes);

				for (UMovieScene* DescendantMovieScene : DescendantMovieScenes)
				{
					if (DescendantMovieScene && DescendantMovieScene->IsReadOnly())
					{
						return true;
					}
				}
			}
		}
	}
	return false;
}

void UBoneRigComponentSequenceEditorBlueprintLibrary::SetLockLevelSequence(bool bLock)
{
	if (CurrentSequencer.IsValid())
	{
		TSharedPtr<ISequencer> Sequencer = CurrentSequencer.Pin();

		if (Sequencer->GetFocusedMovieSceneSequence())
		{
			UMovieScene* MovieScene = Sequencer->GetFocusedMovieSceneSequence()->GetMovieScene();

			if (bLock != MovieScene->IsReadOnly()) 
			{
				MovieScene->Modify();
				MovieScene->SetReadOnly(bLock);
			}

			TArray<UMovieScene*> DescendantMovieScenes;
			MovieSceneHelpers::GetDescendantMovieScenes(Sequencer->GetFocusedMovieSceneSequence(), DescendantMovieScenes);

			for (UMovieScene* DescendantMovieScene : DescendantMovieScenes)
			{
				if (DescendantMovieScene && bLock != DescendantMovieScene->IsReadOnly())
				{
					DescendantMovieScene->Modify();
					DescendantMovieScene->SetReadOnly(bLock);
				}
			}

			Sequencer->NotifyMovieSceneDataChanged(EMovieSceneDataChangeType::Unknown);
		}
	}
}

