// Copyright Epic Games, Inc. All Rights Reserved.

#include "Sequencer/BoneRigComponentSequence.h"
#include "UObject/Package.h"
#include "MovieScene.h"
#include "BoneRigComponent.h"
#include "Engine/Engine.h"

#define LOCTEXT_NAMESPACE "ControlRigSequence"

static TAutoConsoleVariable<int32> CVarControlRigDefaultEvaluationType(
	TEXT("ControlRigSequence.DefaultEvaluationType"),
	0,
	TEXT("0: Playback locked to playback frames\n1: Unlocked playback with sub frame interpolation"),
	ECVF_Default);

static TAutoConsoleVariable<FString> CVarControlRigDefaultTickResolution(
	TEXT("ControlRigSequence.DefaultTickResolution"),
	TEXT("24000fps"),
	TEXT("Specifies default a tick resolution for newly created control rig sequences. Examples: 30 fps, 120/1 (120 fps), 30000/1001 (29.97), 0.01s (10ms)."),
	ECVF_Default);

static TAutoConsoleVariable<FString> CVarControlRigDefaultDisplayRate(
	TEXT("ControlRigSequence.DefaultDisplayRate"),
	TEXT("30fps"),
	TEXT("Specifies default a display frame rate for newly created control rig sequences; also defines frame locked frame rate where sequences are set to be frame locked. Examples: 30 fps, 120/1 (120 fps), 30000/1001 (29.97), 0.01s (10ms)."),
	ECVF_Default); 

UBoneRigComponentSequence::UBoneRigComponentSequence(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, LastExportedFrameRate(30.0f)
{
	bParentContextsAreSignificant = false;
}

void UBoneRigComponentSequence::Initialize()
{
	MovieScene = NewObject<UMovieScene>(this, NAME_None, RF_Transactional);
	MovieScene->SetFlags(RF_Transactional);

	const bool bFrameLocked = CVarControlRigDefaultEvaluationType.GetValueOnGameThread() != 0;

	MovieScene->SetEvaluationType(bFrameLocked ? EMovieSceneEvaluationType::FrameLocked : EMovieSceneEvaluationType::WithSubFrames);

	FFrameRate TickResolution(60000, 1);
	TryParseString(TickResolution, *CVarControlRigDefaultTickResolution.GetValueOnGameThread());
	MovieScene->SetTickResolutionDirectly(TickResolution);

	FFrameRate DisplayRate(30, 1);
	TryParseString(DisplayRate, *CVarControlRigDefaultDisplayRate.GetValueOnGameThread());
	MovieScene->SetDisplayRate(DisplayRate);
}

/*
void UBoneRigComponentSequence::BindPossessableObject(const FGuid& ObjectId, UObject& PossessedObject, UObject* Context)
{
	UObject* PlaybackContext = Context ? Context->GetWorld() : nullptr;

	if (CanPossessObject(PossessedObject, PlaybackContext)) {
		Super::BindPossessableObject(ObjectId, PossessedObject, Context);
	}
}

bool UBoneRigComponentSequence::CanPossessObject(UObject& Object, UObject* InPlaybackContext) const
{
	bool bShouldPossess = false;

	bShouldPossess = Object.IsA<UActorComponent>() || Object.IsA<UAnimInstance>();

	if (!bShouldPossess) {

		if (Object.IsA<AActor>()) {

			if (Cast<AActor>(&Object)->GetComponentByClass(UBoneRigComponent::StaticClass()) != nullptr) {
				bShouldPossess = true;
			}
		}
	}

	return bShouldPossess;
}

UObject* UBoneRigComponentSequence::GetParentObject(UObject* Object) const
{
	return nullptr;
}

void UBoneRigComponentSequence::UnbindPossessableObjects(const FGuid& ObjectId)
{
	//юс╫ц
	Super::UnbindPossessableObjects(ObjectId);
}

UObject* UBoneRigComponentSequence::MakeSpawnableTemplateFromInstance(UObject& InSourceObject, FName ObjectName)
{
	UObject* NewInstance = NewObject<UObject>(MovieScene, InSourceObject.GetClass(), ObjectName);

	UEngine::CopyPropertiesForUnrelatedObjects(&InSourceObject, NewInstance);

	return NewInstance;
}

bool UBoneRigComponentSequence::CanAnimateObject(UObject& InObject) const
{
	return InObject.IsA<UBoneRigComponent>() || InObject.IsA<USpineRigBoneDriverComponent>();
}

*/

#undef LOCTEXT_NAMESPACE
