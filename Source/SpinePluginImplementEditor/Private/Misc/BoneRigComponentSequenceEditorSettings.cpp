// Copyright Epic Games, Inc. All Rights Reserved.

#include "Misc/BoneRigComponentSequenceEditorSettings.h"

UBoneRigComponentSequenceEditorSettings::UBoneRigComponentSequenceEditorSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bAutoBindToSimulate = true;
	bAutoBindToPIE      = true;
}

UBoneRigComponentSequenceMasterSequenceSettings::UBoneRigComponentSequenceMasterSequenceSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, MasterSequenceName(TEXT("Sequence"))
	, MasterSequenceSuffix(TEXT("Master"))
	, MasterSequenceNumShots(5)
{
	MasterSequenceBasePath.Path = TEXT("/Game/Cinematics/Sequences");
}
