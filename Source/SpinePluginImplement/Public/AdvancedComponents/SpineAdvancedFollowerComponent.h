// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "SpineAdvancedFollowerComponent.generated.h"

class USpineAnimationHandlerComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPINEPLUGINIMPLEMENT_API USpineAdvancedFollowerComponent : public USceneComponent
{
	GENERATED_BODY()

public:

	USpineAdvancedFollowerComponent();

public:

	virtual void DestroyComponent(bool bPromoteChildren = false) override;

public:

	void AdjustLocation(class USpineSkeletonComponent* TargetComp);

private:

	UPROPERTY(VisibleAnywhere)
		USpineAnimationHandlerComponent* TargetComponent = nullptr;

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString BoneName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool UsePosition = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool UseRotation = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool UseScale = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bShouldUpdate = true;

public:

	UFUNCTION(BlueprintCallable)
		void SetTargetComponent(USpineAnimationHandlerComponent* InTargetComponent);

	UFUNCTION(BlueprintCallable)
		USpineAnimationHandlerComponent* GetTargetComponent() { return TargetComponent; }


public:

#if WITH_EDITORONLY_DATA

	//에디터상에서 부모 핸들러와의 연결을 위해서 사용하는 타겟 지정 변수입니다.
	UPROPERTY(EditAnywhere, Category = "Setting")
		FName TargetComponentName = NAME_None;
#endif

};
