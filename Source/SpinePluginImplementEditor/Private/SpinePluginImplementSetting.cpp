// Fill out your copyright notice in the Description page of Project Settings.


#include "SpinePluginImplementSetting.h"

USpinePluginImplementSetting::USpinePluginImplementSetting(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

const USpinePluginImplementSetting* USpinePluginImplementSetting::Get()
{
    return GetDefault<USpinePluginImplementSetting>();
}