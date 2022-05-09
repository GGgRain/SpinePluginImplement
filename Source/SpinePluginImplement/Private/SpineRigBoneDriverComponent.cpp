// Fill out your copyright notice in the Description page of Project Settings.


#include "SpineRigBoneDriverComponent.h"
#include "SpineSkeletonRendererComponent.h"
#include "SpineSkeletonComponent.h"
#include "DrawDebugHelpers.h"

#include "SpineAnimationHandlerComponent.h"


#include "Kismet/KismetMathLibrary.h"

USpineRigBoneDriverComponent::USpineRigBoneDriverComponent() {

	this->BodyInstance.PositionSolverIterationCount = 8;
	this->BodyInstance.VelocitySolverIterationCount = 4;

	this->BodyInstance.LinearDamping = 0.2;
	this->BodyInstance.AngularDamping = 0.2;

	this->BodyInstance.bLockXTranslation = false;
	this->BodyInstance.bLockYTranslation = false;
	this->BodyInstance.bLockZTranslation = false;

	this->BodyInstance.bLockXRotation = false;
	this->BodyInstance.bLockYRotation = false;
	this->BodyInstance.bLockZRotation = false;

	this->BodyInstance.MassScale = 1;

	this->SetMobility(EComponentMobility::Movable);

}

/*
bool USkeletalMeshComponent::MoveComponentImpl(const FVector& Delta, const FQuat& NewRotation, bool bSweep, FHitResult* OutHit , EMoveComponentFlags MoveFlags, ETeleportType Teleport)
{

	bool bSuccess = Super::MoveComponentImpl(Delta, NewRotation, bSweep, OutHit, MoveFlags, Teleport);
	if (bSuccess && Teleport != ETeleportType::None)
	{
		// If a skeletal mesh component recieves a teleport we should reset any other dynamic simulations
		ResetAnimInstanceDynamics(Teleport);

		OnSkelMeshPhysicsTeleported.Broadcast();
	}

	return bSuccess;
}

*/
void USpineRigBoneDriverComponent::UpdateDriveBoneWorldTransform(class USpineAnimationHandlerComponent* TargetComp, float MixValue) {
	if (this) {
		if (LastBoneTranslateData.bShouldApply) {
			if (TargetComp != nullptr) {



				if (spine::Bone* Bone = TargetComp->GetSkeletonAnimationComp()->GetSkeleton()->findBone(TCHAR_TO_UTF8(*BoneName))) {

					Bone->update();

					FTransform BeforeAdjustTransform = LastBoneTranslateData.GetWorldTransformFromArchieve(TargetComp);

					const FTransform ThisTransform = this->GetComponentTransform();

					FVector DisLoc = ThisTransform.GetLocation() - BeforeAdjustTransform.GetLocation();

					TargetComp->GetSkeletonAnimationComp()->SetBoneWorldPosition(BoneName, BeforeAdjustTransform.GetLocation() + DisLoc * MixValue);

					//if (BoneName == "LeftLowerLeg") {
						//DrawDebugBox(this->GetWorld(), BeforeAdjustTransform.GetLocation() + DisLoc * MixValue, FVector(1, 1, 1), FColor::Cyan, false, 0.1, 0, 1);
					//}

					if (Bone->getData().getTransformMode() != spine::TransformMode::TransformMode_NoRotationOrReflection &&
						Bone->getData().getTransformMode() != spine::TransformMode::TransformMode_OnlyTranslation)
					{
						FQuat DisRot = FQuat::FastLerp(BeforeAdjustTransform.GetRotation(), ThisTransform.GetRotation(), MixValue);

						FVector DownVec = DisRot.GetUpVector().GetSafeNormal();

						float Theta = FMath::FastAsin(-DownVec.X);

						Theta = ((DownVec.Z > 0) ? Theta : PI - Theta) + PI / 2.f;

						Theta = Theta * 180.f / PI;


						//Decide whether we should flip the bone or not

						bool bShouldFlip = (TargetComp->GetSkeletonAnimationComp()->GetScaleX() > 0) != (TargetComp->GetSkeletonAnimationComp()->GetScaleY() > 0);

						if (bShouldFlip) {
							Theta += 180;
						}


						//부모 본의 로테이션이 반영이 되지 않는 모드 -> 기존 로테이션을 유지시킴

						Bone->setRotation(Bone->worldToLocalRotation(Theta));

					}

					Bone->update();
				}
			}

		}
	}
}

void USpineRigBoneDriverComponent::UpdateFollowBoneWorldTransform(class USpineAnimationHandlerComponent* TargetComp, float MixValue) {
	if (this) {
		if (LastBoneTranslateData.bShouldApply) {
			if (TargetComp != nullptr) {

				if (BoneBodySetup != nullptr) { //만약 프리미티브 바디가 존재하는 경우, 컴포넌트의 로케이션을 따라가게끔 합니다.
					if (spine::Bone* Bone = TargetComp->GetSkeletonAnimationComp()->GetSkeleton()->findBone(TCHAR_TO_UTF8(*BoneName))) {

						if (spine::Bone* ParentBone = Bone->getParent()) {
							ParentBone->update();
						}

						Bone->update();
						
						const FTransform ThisTransform = this->GetComponentTransform();


						//DrawDebugBox(GetWorld(), ThisTransform.GetLocation(), FVector(0.3,2,0.3),FColor::Red,false, 0.05, 0 ,1 );

						//Location Adjust

						TargetComp->GetSkeletonAnimationComp()->SetBoneWorldPosition(BoneName, ThisTransform.GetLocation());
	
						const FTransform TF = TargetComp->GetSkeletonAnimationComp()->GetBoneWorldTransform(BoneName);

						//Rotation Adjust

						if (Bone->getData().getTransformMode() != spine::TransformMode::TransformMode_NoRotationOrReflection &&
							Bone->getData().getTransformMode() != spine::TransformMode::TransformMode_OnlyTranslation)
						{

							FVector DownVec = ThisTransform.GetRotation().GetUpVector().GetSafeNormal();

							float Theta = FMath::FastAsin(-DownVec.X);

							Theta = ((DownVec.Z > 0) ? Theta : PI - Theta) + PI / 2.f;

							Theta = Theta * 180.f / PI;


							//Decide whether we should flip the bone or not

							bool bShouldFlip = (TargetComp->GetSkeletonAnimationComp()->GetScaleX() > 0) != (TargetComp->GetSkeletonAnimationComp()->GetScaleY() > 0);

							if (bShouldFlip) {
								Theta += 180;
							}


							//부모 본의 로테이션이 반영이 되지 않는 모드 -> 기존 로테이션을 유지시킴

							Bone->setRotation(Bone->worldToLocalRotation(Theta));

						}
					

						Bone->update();


					}
				}
				else { 

					if (spine::Bone* Bone = TargetComp->GetSkeletonAnimationComp()->GetSkeleton()->findBone(TCHAR_TO_UTF8(*BoneName))) {

					
						if (spine::Bone* ParentBone = Bone->getParent()) {
							ParentBone->update();
						}

						Bone->update();

						//const FTransform TF = LastBoneTranslateData.GetWorldTransformFromArchieve(TargetComp);

						//TargetComp->GetSkeletonAnimationComp()->SetBoneWorldPosition(BoneName, TF.GetLocation());
						
						const FTransform TF = TargetComp->GetSkeletonAnimationComp()->GetBoneWorldTransform(BoneName);

						this->SetWorldLocationAndRotation(TF.GetLocation(), TF.GetRotation());

						//DrawDebugBox(GetWorld(), TF.GetLocation(), FVector(0.3, 2, 0.3), FColor::Green, false, 0.05, 0, 1);

					}
				}
				
			}

		}
	}
}

void USpineRigBoneDriverComponent::SyncBoneToAnimation(class USpineAnimationHandlerComponent* TargetComp) {
	if (spine::Bone* Bone = TargetComp->GetSkeletonAnimationComp()->GetSkeleton()->findBone(TCHAR_TO_UTF8(*BoneName))) {

		if (BoneBodySetup == nullptr) {
			const FTransform TF = TargetComp->GetSkeletonAnimationComp()->GetBoneWorldTransform(BoneName);

			this->SetWorldLocationAndRotation(TF.GetLocation(), TF.GetRotation());

			TargetComp->GetSkeletonAnimationComp()->SetBoneWorldPosition(BoneName, TF.GetLocation());
		}
	}
}

void USpineRigBoneDriverComponent::SyncAnimationToBone(class USpineAnimationHandlerComponent* TargetComp) {
	if (spine::Bone* Bone = TargetComp->GetSkeletonAnimationComp()->GetSkeleton()->findBone(TCHAR_TO_UTF8(*BoneName))) {

		if (BoneBodySetup == nullptr) {

			const FTransform ThisTransform = this->GetComponentTransform();

			//Location Adjust

			TargetComp->GetSkeletonAnimationComp()->SetBoneWorldPosition(BoneName, ThisTransform.GetLocation());

			const FTransform TF = TargetComp->GetSkeletonAnimationComp()->GetBoneWorldTransform(BoneName);

			//Rotation Adjust

			if (Bone->getData().getTransformMode() != spine::TransformMode::TransformMode_NoRotationOrReflection &&
				Bone->getData().getTransformMode() != spine::TransformMode::TransformMode_OnlyTranslation)
			{

				FVector DownVec = ThisTransform.GetRotation().GetUpVector().GetSafeNormal();

				float Theta = FMath::FastAsin(-DownVec.X);

				Theta = ((DownVec.Z > 0) ? Theta : PI - Theta) + PI / 2.f;

				Theta = Theta * 180.f / PI;


				//Decide whether we should flip the bone or not

				bool bShouldFlip = (TargetComp->GetSkeletonAnimationComp()->GetScaleX() > 0) != (TargetComp->GetSkeletonAnimationComp()->GetScaleY() > 0);

				if (bShouldFlip) {
					Theta += 180;
				}


				//부모 본의 로테이션이 반영이 되지 않는 모드 -> 기존 로테이션을 유지시킴

				Bone->setRotation(Bone->worldToLocalRotation(Theta));

			}

		}
	}
}


void USpineRigBoneDriverComponent::UpdateFollowBoneWorldTransformFinalApply(class USpineAnimationHandlerComponent* TargetComp, float PhysicalWeight) {
	if (spine::Bone* Bone = TargetComp->GetSkeletonAnimationComp()->GetSkeleton()->findBone(TCHAR_TO_UTF8(*BoneName))) {


		if (spine::Bone* ParentBone = Bone->getParent()) {
			ParentBone->update();
		}
		Bone->update();



		const FTransform TF = TargetComp->GetSkeletonAnimationComp()->GetBoneWorldTransform(BoneName);

		this->SetWorldLocationAndRotation(TF.GetLocation(), TF.GetRotation());

	}
}

//델리케이트와 파이프라인을 수정해야함.

void USpineRigBoneDriverComponent::CollectBoneTranslateData(class USpineSkeletonComponent* TargetComp) {
	if (this) {
		if (TargetComp != nullptr) {
			if (spine::Skeleton* Skel = TargetComp->GetSkeleton()) {
				if (spine::Bone* TargetBone = Skel->findBone(TCHAR_TO_UTF8(*BoneName))) {
					LastBoneTranslateData.CopyFromBone(TargetBone);
				}
			}
		}
	}
}

void USpineRigBoneDriverComponent::ResetBoneTranslateData(class USpineSkeletonComponent* TargetComp) {
	if (this) {
		if (TargetComp != nullptr) {
			if (spine::Skeleton* Skel = TargetComp->GetSkeleton()) {
				if (spine::Bone* TargetBone = Skel->findBone(TCHAR_TO_UTF8(*BoneName))) {
					LastBoneTranslateData.PasteToBone(TargetBone);
				}
			}
		}
	}
}


#if WITH_EDITOR 


void USpineRigBoneDriverComponent::ChangeBoxColorByHierarchyIndex(const int Index ,const int MaxIndex) {
	if (this != nullptr) {
		FColor CompColor = GetColorForBoneHierarchy(Index, MaxIndex);
		this->BoneColor = CompColor;
	}
}

const FColor USpineRigBoneDriverComponent::GetColorForBoneHierarchy(const int index, const int MaxIndex) {

	/*
	TArray<FColor> BoneHierarchy(
		{
			FColor::White,
			FColor::Black,
			FColor::Red,
			FColor::Green,
			FColor::Blue,
			FColor::Yellow,
			FColor::Cyan,
			FColor::Magenta,
			FColor::Orange,
			FColor::Purple,
			FColor::Turquoise,
			FColor::Silver,
			FColor::Emerald,
		}
	);
	*/

	//return UKismetMathLibrary::HSVToRGB(255 * (float(index) / float(MaxIndex)), 128 , 128).ToFColor(true);

	//return BoneHierarchy[index % (BoneHierarchy.Num() - 1)];

	return FColor(255 * (1 - float(index) / float(MaxIndex)), 127 + 128 * (float(index) / float(MaxIndex)) , 255 * (float(index) / float(MaxIndex)));
}

#endif



FPrimitiveSceneProxy* USpineRigBoneDriverComponent::CreateSceneProxy() {

	float length = 10;

	if (TargetSkeleton != nullptr) {
		if (spine::Skeleton* Skel = TargetSkeleton->GetSkeleton()) {
			if (spine::Bone* TargetBone = Skel->findBone(TCHAR_TO_UTF8(*BoneName))) {
				length = TargetBone->getData().getLength();
			}
		}
	}

	/** Represents a UBoxComponent to the scene manager. */
	class FBoneBaseSceneProxy final : public FPrimitiveSceneProxy
	{
	public:
		SIZE_T GetTypeHash() const override
		{
			static size_t UniquePointer;
			return reinterpret_cast<size_t>(&UniquePointer);
		}


		FBoneBaseSceneProxy(USpineRigBoneDriverComponent* InComponent, float Inlength)
			: FPrimitiveSceneProxy(InComponent)
			, bDrawOnlyIfSelected(true)
			, BoxExtents(InComponent->GetCollisionShape().GetExtent())
			, BoxColor(InComponent->BoneColor)
			, ArrowLength(Inlength)
			, LineThickness(3)
		{
			bWillEverBeLit = false;
		}


		virtual void GetDynamicMeshElements(const TArray<const FSceneView*>& Views, const FSceneViewFamily& ViewFamily, uint32 VisibilityMap, FMeshElementCollector& Collector) const override
		{
			QUICK_SCOPE_CYCLE_COUNTER(STAT_BoxSceneProxy_GetDynamicMeshElements);

			const FMatrix& LocalToWorld = GetLocalToWorld();
			FTransform Tf = FTransform(LocalToWorld);
			Tf.SetLocation(Tf.GetLocation() + FVector(0, 7, 0));
			Tf.SetRotation(Tf.GetRotation() * FRotator(90, 0, 0).Quaternion());
			//Tf.SetScale3D();

			for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++)
			{
				if (VisibilityMap & (1 << ViewIndex))
				{
					const FSceneView* View = Views[ViewIndex];

					//const FLinearColor DrawColor = GetViewSelectionColor(BoxColor, *View, IsSelected(), IsHovered(), false, IsIndividuallySelected());

					FPrimitiveDrawInterface* PDI = Collector.GetPDI(ViewIndex);

					if (ArrowLength < 3) {
						DrawOrientedWireBox(PDI, LocalToWorld.GetOrigin(), LocalToWorld.GetScaledAxis(EAxis::X), LocalToWorld.GetScaledAxis(EAxis::Y), LocalToWorld.GetScaledAxis(EAxis::Z), FVector(1.5,5,1.5) * 2, BoxColor, SDPG_World, LineThickness);
					}
					else {
						DrawDirectionalArrow(PDI, Tf.ToMatrixWithScale(), BoxColor, ArrowLength, 2, 0, LineThickness);
					}
					//PDI->DrawSprite(LocalToWorld.GetOrigin(), 1, 20, ArrowTex->Resource, BoxColor, 0, 0, 0, 0, 0);
					//Draw(PDI, LocalToWorld + FTransform(FRotator(90,0,0),FVector::ZeroVector,FVector::ZeroVector).ToMatrixNoScale(), BoxColor, 20, 12, 30, 1);
					
					//DrawOrientedWireBox(PDI, LocalToWorld.GetOrigin(), LocalToWorld.GetScaledAxis(EAxis::X), LocalToWorld.GetScaledAxis(EAxis::Y), LocalToWorld.GetScaledAxis(EAxis::Z), FVector(1,5,1) * 2, BoxColor, SDPG_World, LineThickness);

				}
			}
		}

		virtual FPrimitiveViewRelevance GetViewRelevance(const FSceneView* View) const override
		{
			const bool bProxyVisible = !bDrawOnlyIfSelected || IsSelected();

			// Should we draw this because collision drawing is enabled, and we have collision
			const bool bShowForCollision = View->Family->EngineShowFlags.Collision && IsCollisionEnabled();

			FPrimitiveViewRelevance Result;
			Result.bDrawRelevance = (IsShown(View) && bProxyVisible) || bShowForCollision;
			Result.bDynamicRelevance = true;
			Result.bShadowRelevance = IsShadowCast(View);
			Result.bEditorPrimitiveRelevance = UseEditorCompositing(View);
			return Result;
		}
		virtual uint32 GetMemoryFootprint(void) const override { return(sizeof(*this) + GetAllocatedSize()); }
		uint32 GetAllocatedSize(void) const { return(FPrimitiveSceneProxy::GetAllocatedSize()); }

	private:
		const uint32	bDrawOnlyIfSelected : 1;
		const FVector	BoxExtents;
		const FColor	BoxColor;
		const float ArrowLength;
		const float LineThickness;
	};

	return new FBoneBaseSceneProxy(this, length);
}

class UBodySetup* USpineRigBoneDriverComponent::GetBodySetup() {
	return BoneBodySetup;
}

void USpineRigBoneDriverComponent::CreateBodySetupIfNeeded() {
	if (this) {
		if (BoneBodySetup == nullptr || BoneBodySetup->IsPendingKill())
		{
			BoneBodySetup = NewObject<UBodySetup>(this, NAME_None, RF_Transient);
			if (GUObjectArray.IsDisregardForGC(this))
			{
				BoneBodySetup->AddToRoot();
			}

			// If this component is in GC cluster, make sure we add the body setup to it to
			BoneBodySetup->AddToCluster(this);
			// if we got created outside of game thread, but got added to a cluster, 
			// we no longer need the Async flag
			if (BoneBodySetup->HasAnyInternalFlags(EInternalObjectFlags::Async) && GUObjectClusters.GetObjectCluster(BoneBodySetup))
			{
				BoneBodySetup->ClearInternalFlags(EInternalObjectFlags::Async);
			}

			BoneBodySetup->CollisionTraceFlag = CTF_UseSimpleAsComplex;
		}
	}
}

void USpineRigBoneDriverComponent::RebuildBodyCollisionWithBodyInstanceShapes() {
	if (BoneBodySetup != nullptr) {

		//Update bodyinstance and shapes
		BodyInstance.InitBody(
			BoneBodySetup,
			this->GetComponentTransform(),
			this,
			GetWorld()->GetPhysicsScene()
		);


		BoneBodySetup->InvalidatePhysicsData();

		BoneBodySetup->bCreatedPhysicsMeshes = false;
		BoneBodySetup->bNeverNeedsCookedCollisionData = false;
		
		BoneBodySetup->CreatePhysicsMeshes();

	}
}

void USpineRigBoneDriverComponent::SetupPhysicActorHandle(FPhysicsActorHandle& Handle) {
	BodyInstance.ActorHandle.SyncActor = Handle.SyncActor;
}





void USpineRigBoneDriverComponent::AddImpulse(FVector Impulse, FName InBoneName, bool bVelChange)
{
	if (FBodyInstance* BI = GetBodyInstance(InBoneName))
	{
		Super::AddImpulse(Impulse * BI->PhysicsBlendWeight, InBoneName, bVelChange);
	}
}

void USpineRigBoneDriverComponent::AddAngularImpulseInRadians(FVector Impulse, FName InBoneName, bool bVelChange)
{
	if (FBodyInstance* BI = GetBodyInstance(InBoneName))
	{
		Super::AddAngularImpulseInRadians(Impulse * BI->PhysicsBlendWeight, InBoneName, bVelChange);
	}
}

void USpineRigBoneDriverComponent::AddImpulseAtLocation(FVector Impulse, FVector Location, FName InBoneName)
{
	if (FBodyInstance* BI = GetBodyInstance(InBoneName))
	{
		Super::AddImpulseAtLocation(Impulse * BI->PhysicsBlendWeight, Location, InBoneName);
	}
}

void USpineRigBoneDriverComponent::AddRadialImpulse(FVector Origin, float Radius, float Strength, ERadialImpulseFalloff Falloff, bool bVelChange)
{
	if (FBodyInstance* BI = GetBodyInstance())
	{
		Super::AddRadialImpulse(Origin, Radius, Strength * BI->PhysicsBlendWeight, Falloff, bVelChange);
	}
}


void USpineRigBoneDriverComponent::AddForce(FVector Force, FName InBoneName, bool bAccelChange)
{
	if (FBodyInstance* BI = GetBodyInstance(InBoneName))
	{
		Super::AddForce(Force * BI->PhysicsBlendWeight, InBoneName, bAccelChange);
	}
}

void USpineRigBoneDriverComponent::AddForceAtLocation(FVector Force, FVector Location, FName InBoneName)
{
	if (FBodyInstance* BI = GetBodyInstance(InBoneName))
	{
		Super::AddForceAtLocation(Force * BI->PhysicsBlendWeight, Location, InBoneName);
	}
}

void USpineRigBoneDriverComponent::AddForceAtLocationLocal(FVector Force, FVector Location, FName InBoneName)
{
	if (FBodyInstance* BI = GetBodyInstance(InBoneName))
	{
		Super::AddForceAtLocationLocal(Force * BI->PhysicsBlendWeight, Location, InBoneName);
	}
}

void USpineRigBoneDriverComponent::AddRadialForce(FVector Origin, float Radius, float Strength, ERadialImpulseFalloff Falloff, bool bAccelChange)
{

	if (FBodyInstance* BI = GetBodyInstance())
	{
		Super::AddRadialForce(Origin, Radius, Strength * BI->PhysicsBlendWeight, Falloff, bAccelChange);
	}
}

void USpineRigBoneDriverComponent::AddTorqueInRadians(FVector Torque, FName InBoneName, bool bAccelChange)
{
	if (FBodyInstance* BI = GetBodyInstance(InBoneName))
	{
		Super::AddTorqueInRadians(Torque * BI->PhysicsBlendWeight, InBoneName,  bAccelChange);
	}
}

