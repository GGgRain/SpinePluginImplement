// Fill out your copyright notice in the Description page of Project Settings.

#include "SpineAnimationHandlerComponent.h"

#include "BoneRigComponent.h"
#include "AdvancedComponents/SpineAdvancedDriverComponent.h"
#include "AdvancedComponents/SpineAdvancedFollowerComponent.h"
#include "IKConstraintBoneDriverComponent.h"
#include "Kismet/GameplayStatics.h"

USpineAnimationHandlerComponent::USpineAnimationHandlerComponent(const FObjectInitializer& ObjectInitializer) : USpineSkeletonRendererComponent(ObjectInitializer) {
	
	this->SetMobility(EComponentMobility::Movable);

	SkeletonAnimationComp = CreateDefaultSubobject<USpineSkeletonAnimationComponent>(TEXT("AnimationComponent"));

	//SkeletonAnimationComp->AnimationInterrupt.AddDynamic(this, &USpineAnimationHandlerComponent::Internal_PlayNextAnimation);
	//SkeletonAnimationComp->AnimationEnd.AddDynamic(this, &USpineAnimationHandlerComponent::Internal_PlayNextAnimation);
	//SkeletonAnimationComp->AnimationDispose.AddDynamic(this, &USpineAnimationHandlerComponent::Internal_PlayNextAnimation);
	SkeletonAnimationComp->AnimationComplete.AddDynamic(this, &USpineAnimationHandlerComponent::Internal_PlayNextAnimation);

	//Event Add
	SkeletonAnimationComp->BeforeApplyUpdateWorldTransform.AddDynamic(this, &USpineAnimationHandlerComponent::BeforeApplyUpdateWorldTransform);
	SkeletonAnimationComp->BeforeUpdateWorldTransform.AddDynamic(this, &USpineAnimationHandlerComponent::BeforeUpdateWorldTransform);
	SkeletonAnimationComp->AfterUpdateWorldTransform.AddDynamic(this, &USpineAnimationHandlerComponent::AfterUpdateWorldTransform);

	PrimaryComponentTick.AddPrerequisite(SkeletonAnimationComp, SkeletonAnimationComp->PrimaryComponentTick);
}


void USpineAnimationHandlerComponent::UpdateMesh(spine::Skeleton* Skeleton) {
	TArray<FVector> vertices;
	TArray<int32> indices;
	TArray<FVector> normals;
	TArray<FVector2D> uvs;
	TArray<FColor> colors;
	TArray<FVector> darkColors;

	int idx = 0;
	int meshSection = 0;
	UMaterialInstanceDynamic* lastMaterial = nullptr;

	ClearAllMeshSections();

	// Early out if skeleton is invisible
	if (Skeleton->getColor().a == 0) return;

	float depthOffset = 0;
	unsigned short quadIndices[] = { 0, 1, 2, 0, 2, 3 };

	for (size_t i = 0; i < Skeleton->getSlots().size(); ++i) {
		spine::Vector<float>* attachmentVertices = &worldVertices;
		unsigned short* attachmentIndices = nullptr;
		int numVertices;
		int numIndices;
		spine::AtlasRegion* attachmentAtlasRegion = nullptr;
		spine::Color attachmentColor;
		attachmentColor.set(1, 1, 1, 1);
		float* attachmentUvs = nullptr;

		spine::Slot* slot = Skeleton->getDrawOrder()[i];
		spine::Attachment* attachment = slot->getAttachment();

		if (slot->getColor().a == 0 || !slot->getBone().isActive()) {
			clipper.clipEnd(*slot);
			continue;
		}

		if (!attachment) {
			clipper.clipEnd(*slot);
			continue;
		}
		if (!attachment->getRTTI().isExactly(spine::RegionAttachment::rtti) && !attachment->getRTTI().isExactly(spine::MeshAttachment::rtti) && !attachment->getRTTI().isExactly(spine::ClippingAttachment::rtti)) {
			clipper.clipEnd(*slot);
			continue;
		}

		if (attachment->getRTTI().isExactly(spine::RegionAttachment::rtti)) {
			spine::RegionAttachment* regionAttachment = (spine::RegionAttachment*)attachment;

			// Early out if region is invisible
			if (regionAttachment->getColor().a == 0) {
				clipper.clipEnd(*slot);
				continue;
			}

			attachmentColor.set(regionAttachment->getColor());
			attachmentAtlasRegion = (spine::AtlasRegion*)regionAttachment->getRendererObject();
			regionAttachment->computeWorldVertices(slot->getBone(), *attachmentVertices, 0, 2);
			attachmentIndices = quadIndices;
			attachmentUvs = regionAttachment->getUVs().buffer();
			numVertices = 4;
			numIndices = 6;
		}
		else if (attachment->getRTTI().isExactly(spine::MeshAttachment::rtti)) {
			spine::MeshAttachment* mesh = (spine::MeshAttachment*)attachment;

			// Early out if region is invisible
			if (mesh->getColor().a == 0) {
				clipper.clipEnd(*slot);
				continue;
			}

			attachmentColor.set(mesh->getColor());
			attachmentAtlasRegion = (spine::AtlasRegion*)mesh->getRendererObject();
			mesh->computeWorldVertices(*slot, 0, mesh->getWorldVerticesLength(), *attachmentVertices, 0, 2);
			attachmentIndices = mesh->getTriangles().buffer();
			attachmentUvs = mesh->getUVs().buffer();
			numVertices = mesh->getWorldVerticesLength() >> 1;
			numIndices = mesh->getTriangles().size();
		}
		else /* clipping */ {
			spine::ClippingAttachment* clip = (spine::ClippingAttachment*)attachment;
			clipper.clipStart(*slot, clip);
			continue;
		}

		// if the user switches the atlas data while not having switched
		// to the correct skeleton data yet, we won't find any regions.
		// ignore regions for which we can't find a material
		UMaterialInstanceDynamic* material = nullptr;
		switch (slot->getData().getBlendMode()) {
		case spine::BlendMode_Normal:
			if (!pageToNormalBlendMaterial.Contains(attachmentAtlasRegion->page)) {
				clipper.clipEnd(*slot);
				continue;
			}
			material = pageToNormalBlendMaterial[attachmentAtlasRegion->page];
			break;
		case spine::BlendMode_Additive:
			if (!pageToAdditiveBlendMaterial.Contains(attachmentAtlasRegion->page)) {
				clipper.clipEnd(*slot);
				continue;
			}
			material = pageToAdditiveBlendMaterial[attachmentAtlasRegion->page];
			break;
		case spine::BlendMode_Multiply:
			if (!pageToMultiplyBlendMaterial.Contains(attachmentAtlasRegion->page)) {
				clipper.clipEnd(*slot);
				continue;
			}
			material = pageToMultiplyBlendMaterial[attachmentAtlasRegion->page];
			break;
		case spine::BlendMode_Screen:
			if (!pageToScreenBlendMaterial.Contains(attachmentAtlasRegion->page)) {
				clipper.clipEnd(*slot);
				continue;
			}
			material = pageToScreenBlendMaterial[attachmentAtlasRegion->page];
			break;
		default:
			if (!pageToNormalBlendMaterial.Contains(attachmentAtlasRegion->page)) {
				clipper.clipEnd(*slot);
				continue;
			}
			material = pageToNormalBlendMaterial[attachmentAtlasRegion->page];
		}

		if (clipper.isClipping()) {
			clipper.clipTriangles(attachmentVertices->buffer(), attachmentIndices, numIndices, attachmentUvs, 2);
			attachmentVertices = &clipper.getClippedVertices();
			numVertices = clipper.getClippedVertices().size() >> 1;
			attachmentIndices = clipper.getClippedTriangles().buffer();
			numIndices = clipper.getClippedTriangles().size();
			attachmentUvs = clipper.getClippedUVs().buffer();
			if (clipper.getClippedTriangles().size() == 0) {
				clipper.clipEnd(*slot);
				continue;
			}
		}

		if (lastMaterial != material) {
			Flush(meshSection, vertices, indices, normals, uvs, colors, darkColors, lastMaterial);
			lastMaterial = material;
			idx = 0;
		}

		SetMaterial(meshSection, material);

		uint8 r = static_cast<uint8>(Skeleton->getColor().r * slot->getColor().r * attachmentColor.r * 255);
		uint8 g = static_cast<uint8>(Skeleton->getColor().g * slot->getColor().g * attachmentColor.g * 255);
		uint8 b = static_cast<uint8>(Skeleton->getColor().b * slot->getColor().b * attachmentColor.b * 255);
		uint8 a = static_cast<uint8>(Skeleton->getColor().a * slot->getColor().a * attachmentColor.a * 255);

		float dr = slot->hasDarkColor() ? slot->getDarkColor().r : 0.0f;
		float dg = slot->hasDarkColor() ? slot->getDarkColor().g : 0.0f;
		float db = slot->hasDarkColor() ? slot->getDarkColor().b : 0.0f;

		float* verticesPtr = attachmentVertices->buffer();
		for (int j = 0; j < numVertices << 1; j += 2) {
			colors.Add(FColor(r, g, b, a));
			darkColors.Add(FVector(dr, dg, db));
			vertices.Add(FVector(verticesPtr[j], depthOffset, verticesPtr[j + 1]));
			uvs.Add(FVector2D(attachmentUvs[j], attachmentUvs[j + 1]));
		}

		int firstIndex = indices.Num();
		for (int j = 0; j < numIndices; j++) {
			indices.Add(idx + attachmentIndices[j]);
		}

		

		//노말 방향을 각 트라이앵글에 맞게 최적화 합니다. 가끔가다가 트라이앵글이 뒤로 접히는 경우가 있는데, 이런 경우를 막기 위한 추가 코드입니다.
		//false일시 기존 플러그인에서 처리했듯, 첫 트라이앵글의 버텍스들을 이용해 닷 프로젝트를 한 노말벡터를 이용합니다.

		//현재 로직은 그냥 노말 자체를 런타임으로 시작합니다.

		//기본 제공 로직이 문제가 많음. 
		//노말 갯수가 버텍스 개수랑 동일한데, 이거 걍 설계미스.
		if(bUseAdvancedNormalCalculation){


			int32 NumCCWTri = 0;
			int32 NumRCWTri = 0;

			normals.AddUninitialized(vertices.Num());

			int32 NumTri = indices.Num() / 3;

			FVector normal = FVector(0, -1, 0);

			for (int j = 0; j < NumTri; j++) {

				if (FVector::CrossProduct(
					vertices[indices[j * 3 + 2]] - vertices[indices[j * 3]],
					vertices[indices[j * 3 + 1]] - vertices[indices[j * 3]]).Y > 0.f)
				{
					NumCCWTri++;
				}
				else {
					NumRCWTri++;
				}
			}

			normal.Y = (NumCCWTri < NumRCWTri) ? -1 : 1;

			if (bShouldFlipNormal) {
				normal.Y *= -1;
			}

			for (int j = 0; j < normals.Num(); ++j) {
				normals[j] = normal;
			}

		}
		else {
			FVector normal = FVector(0, -1, 0);

			if (numVertices > 2 &&
				FVector::CrossProduct(
					vertices[indices[firstIndex + 2]] - vertices[indices[firstIndex]],
					vertices[indices[firstIndex + 1]] - vertices[indices[firstIndex]]).Y > 0.f) {
				normal.Y = 1;
			}

			for (int j = 0; j < numVertices; j++) {
				normals.Add(normal);
			}
		}

		
		idx += numVertices;
		depthOffset += this->DepthOffset;

		clipper.clipEnd(*slot);
	}

	Flush(meshSection, vertices, indices, normals, uvs, colors, darkColors, lastMaterial);
	clipper.clipEnd();
}

//Movement Implementation

FDelegateHandle USpineAnimationHandlerComponent::RegisterOnTeleportDelegate(const FOnHandlerTeleported& Delegate)
{
	return OnHandlerTeleported.Add(Delegate);
}

void USpineAnimationHandlerComponent::UnregisterOnTeleportDelegate(const FDelegateHandle& DelegateHandle)
{
	OnHandlerTeleported.Remove(DelegateHandle);
}


bool USpineAnimationHandlerComponent::MoveComponentImpl(const FVector& Delta, const FQuat& NewRotation, bool bSweep, FHitResult* OutHit /*= nullptr*/, EMoveComponentFlags MoveFlags /*= MOVECOMP_NoFlags*/, ETeleportType Teleport /*= ETeleportType::None*/)
{

	bool bSuccess = Super::MoveComponentImpl(Delta, NewRotation, bSweep, OutHit, MoveFlags, Teleport);
	if (bSuccess && Teleport != ETeleportType::None)
	{
		OnHandlerTeleported.Broadcast();
	}

	return bSuccess;
}

void USpineAnimationHandlerComponent::FlipSkeleton(bool bIsForward) {
	float ScaleX = SkeletonAnimationComp->GetScaleX();

	if (bIsForward) {
		if (ScaleX < 0) { //플립을 해야하는가?
			SkeletonAnimationComp->SetScaleX(-ScaleX); 


			if (bShouldUseImmediateFlip) {
				SkeletonAnimationComp->InternalTick(0, false, false);
				this->UpdateRenderer(SkeletonAnimationComp);
			}
			else {
				SkeletonAnimationComp->TickComponent(0, ELevelTick::LEVELTICK_All, nullptr);
				this->TickComponent(0, ELevelTick::LEVELTICK_All, nullptr);
			}

			OnHandlerTeleported.Broadcast();
		}
	}
	else {
		if (ScaleX > 0) { //플립을 해야하는가?
			SkeletonAnimationComp->SetScaleX(-ScaleX);

			if (bShouldUseImmediateFlip) {
				SkeletonAnimationComp->InternalTick(0, false, false);
				this->UpdateRenderer(SkeletonAnimationComp);
			}
			else {
				SkeletonAnimationComp->TickComponent(0, ELevelTick::LEVELTICK_All, nullptr);
				this->TickComponent(0, ELevelTick::LEVELTICK_All, nullptr);
			}

			OnHandlerTeleported.Broadcast();
		}
	}
}

void USpineAnimationHandlerComponent::OnNotifyDriverBoneEndedUpdate(USpineAdvancedDriverComponent* Target) {
	if (!Target) {
		return;
	}

	if (!DriverArr.Contains(Target)) {
		return;
	}

	for(UBoneRigComponent* Comp : BoneRigArr) {

		if (Comp->RigBoneDrivers.Contains(FName(Target->BoneName))) {
			Comp->RigBoneDrivers[FName(Target->BoneName)]->LastBoneTranslateData.bShouldApply = false;
		}

	}
}

void USpineAnimationHandlerComponent::FlipSkeletonTowardVector(FVector Vec) {
	FlipSkeleton(Vec.X > 0);
}

void USpineAnimationHandlerComponent::BeforeApplyUpdateWorldTransform(class USpineSkeletonComponent* TargetComp) {
	if (this) {

		for (UBoneRigComponent* TestComp : BoneRigArr) {
			if (TestComp) {
				TestComp->ResetAfterUpdateBones(TargetComp);
			}
		}

		for (UBoneRigComponent* TestComp : BoneRigArr) {
			if (TestComp) {
				TestComp->ResetRootMotion();
			}
		}
	}
}

void USpineAnimationHandlerComponent::BeforeUpdateWorldTransform(class USpineSkeletonComponent* TargetComp) {
	if (this) {

		for (USpineAdvancedDriverComponent* TestComp : DriverArr) { // IK 본은 이때 업데이트를 해줘야 적용됨.
			if (TestComp) {
				TestComp->UpdateBone(TargetComp);
			}
		}

		for (UBoneRigComponent* TestComp : BoneRigArr) {
			if (TestComp) {
				TestComp->UpdateRootMotion();
			}
		}

		
	}
}

void USpineAnimationHandlerComponent::AfterUpdateWorldTransform(class USpineSkeletonComponent* TargetComp) {
	if (this) {

		SkeletonAnimationComp->GetSkeleton()->setX(RenderingOffset.X);
		SkeletonAnimationComp->GetSkeleton()->setY(RenderingOffset.Z);

		for (UBoneRigComponent* TestComp : BoneRigArr) {
			if (TestComp) {
				TestComp->CollectAllBoneTF(TargetComp);
			}
		}


		for (UBoneRigComponent* TestComp : BoneRigArr) {
			if (TestComp) {
				TestComp->UpdateBones(TargetComp);
			}
		}

		for (UIKConstraintBoneDriverComponent* TestComp : IKDriverArr) { // IK 본은 이때 업데이트를 해줘야 적용됨.
			if (TestComp) {
				TestComp->AdjustTargetIKBone(TargetComp);
			}
		}

		for (USpineAdvancedFollowerComponent* TestComp : FollowerArr) {
			if (TestComp) {
				TestComp->AdjustLocation(TargetComp);
			}
		}
	}
}
