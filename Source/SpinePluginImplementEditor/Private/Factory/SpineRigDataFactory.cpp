// Fill out your copyright notice in the Description page of Project Settings.


#include "Factory/SpineRigDataFactory.h"
#include "SpineRigData.h"

USpineRigDataFactory::USpineRigDataFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = USpineRigData::StaticClass();
}

UObject* USpineRigDataFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	USpineRigData* TrimTool = NewObject<USpineRigData>(InParent, Class, Name, Flags | RF_Transactional);

	return TrimTool;
}

bool USpineRigDataFactory::ShouldShowInNewMenu() const {
	return true;
}
