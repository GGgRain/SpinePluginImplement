// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Components/PrimitiveComponent.h"
#include "SpineAdvancedDriverComponent.generated.h"

class USpineAnimationHandlerComponent;


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SPINEPLUGINIMPLEMENT_API USpineAdvancedDriverComponent : public UPrimitiveComponent {
	GENERATED_BODY()
public:

	USpineAdvancedDriverComponent();

private:

	UPROPERTY(VisibleAnywhere)
		USpineAnimationHandlerComponent* TargetComponent = nullptr;

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString BoneName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool UseFixedRotation = true;


private:

	UPROPERTY(VisibleAnywhere)
		bool bShouldUpdate = false;

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		FTransform SavedTF;

public:

	void StartUpdateBone();
	
	virtual void UpdateBone(class USpineSkeletonComponent* TargetComp);

	void StopUpdateBone();

public:

	virtual void DestroyComponent(bool bPromoteChildren = false) override;

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
