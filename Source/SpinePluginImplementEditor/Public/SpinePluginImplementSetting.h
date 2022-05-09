// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/DeveloperSettings.h"
#include "SpinePluginImplementSetting.generated.h"

/**
 * 
 */
UCLASS(config = Editor, defaultconfig)
class SPINEPLUGINIMPLEMENTEDITOR_API USpinePluginImplementSetting : public UDeveloperSettings
{
	GENERATED_BODY()

	USpinePluginImplementSetting(const FObjectInitializer& ObjectInitializer);

public:

	//Name of default collision profile for the Ik target bones for bonerig.
	UPROPERTY(config, EditAnywhere, Category = BoneRig_Collision)
		FName DefaultBoneRigIKBoneOverrideCollisionProfileName = "NoCollisionWithPhysicsUpdate";

public:

	static const USpinePluginImplementSetting* Get();
	
};
