// 2012 - 2019 Soverance Studios
// https://soverance.com

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

// http://www.apache.org/licenses/LICENSE-2.0

// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "PhysicsEngine/ConstraintInstance.h"
#include "CommonLibrary.generated.h"

UCLASS()
class ENDLESSREACHHD_API UCommonLibrary : public UObject
{
	GENERATED_BODY()

public:

	UCommonLibrary();

	// Spawn an Actor into a specified level
	UFUNCTION(BlueprintCallable, Category = Default, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
	static AActor* SpawnActorIntoLevel(UObject* WorldContextObject, TSubclassOf<AActor> ActorClass, FName Level = NAME_None, FVector Location = FVector::ZeroVector, FRotator Rotation = FRotator::ZeroRotator, bool SpawnEvenIfColliding = true);
	
	//////////////////////////////////////////////////
	// SetAngularLimits for Physics Constraints
	//
	// THE VARIABLES FOR CONSTRAINT PROPERTIES HAVE BEEN DEPRECATED WITHOUT DOCUMENTATION!
	//
	/////////////////////////////////////////////////
	static FORCEINLINE void SetAngularLimits(
		FConstraintInstance& Constraint,
		const uint8 Swing1Limit, const uint8 Swing2Limit, const uint8 TwistLimit,
		const float Swing1LimitAngle,
		const float Swing2LimitAngle,
		const float TwistLimitAngle,

		bool SoftSwingLimit = true, bool SoftTwistLimit = true,
		const float SwingStiff = 0, const float SwingDamp = 0,
		const float TwistStiff = 0, const float TwistDamp = 0
	)
	{
		switch (Swing1Limit)
		{
			case 0: Constraint.SetAngularSwing1Motion(EAngularConstraintMotion::ACM_Free); break;
			case 1: Constraint.SetAngularSwing1Motion( EAngularConstraintMotion::ACM_Limited); break;
			case 2: Constraint.SetAngularSwing1Motion(EAngularConstraintMotion::ACM_Locked); break;
		}
		switch (Swing2Limit)
		{
			case 0: Constraint.SetAngularSwing2Motion(EAngularConstraintMotion::ACM_Free); break;
			case 1: Constraint.SetAngularSwing2Motion(EAngularConstraintMotion::ACM_Limited); break;
			case 2: Constraint.SetAngularSwing2Motion(EAngularConstraintMotion::ACM_Locked); break;
		}
		switch (TwistLimit)
		{
			case 0: Constraint.SetAngularTwistMotion(EAngularConstraintMotion::ACM_Free); break;
			case 1: Constraint.SetAngularTwistMotion(EAngularConstraintMotion::ACM_Limited); break;
			case 2: Constraint.SetAngularTwistMotion(EAngularConstraintMotion::ACM_Locked); break;
		}

		// Soft Limit?
		if (SoftSwingLimit) Constraint.ProfileInstance.LinearLimit.bSoftConstraint = 1;
		else Constraint.ProfileInstance.LinearLimit.bSoftConstraint = 0;

		if (SoftTwistLimit) Constraint.ProfileInstance.TwistLimit.bSoftConstraint = 1;
		else Constraint.ProfileInstance.TwistLimit.bSoftConstraint = 0;

		// Limit Angles
		Constraint.SetAngularSwing1Limit(EAngularConstraintMotion::ACM_Free, Swing1LimitAngle);
		Constraint.SetAngularSwing2Limit(EAngularConstraintMotion::ACM_Free, Swing2LimitAngle);
		Constraint.SetAngularTwistLimit(EAngularConstraintMotion::ACM_Free, TwistLimitAngle);

		Constraint.ProfileInstance.LinearLimit.Stiffness = SwingStiff;
		Constraint.ProfileInstance.LinearLimit.Damping = SwingDamp;
		Constraint.ProfileInstance.TwistLimit.Stiffness = TwistStiff;
		Constraint.ProfileInstance.TwistLimit.Damping = TwistDamp;
	}

	//////////////////////////////////////////////////
	// SetLinearLimits for Physics Constraints
	//
	// THE VARIABLES FOR CONSTRAINT PROPERTIES HAVE BEEN DEPRECATED WITHOUT DOCUMENTATION!
	//
	/////////////////////////////////////////////////
	static FORCEINLINE void SetLinearLimits(
		FConstraintInstance& Constraint,
		bool bDisableCollision,
		const uint8 XLimit, const uint8 YLimit, const uint8 ZLimit,
		const float Size,
		bool SoftLimit = true,
		const float SoftStiffness = 0,
		const float SoftDampening = 0
	)
	{
		// Collision
		Constraint.ProfileInstance.bDisableCollision = bDisableCollision;

		switch (XLimit)
		{
			case 0: Constraint.SetLinearXMotion(ELinearConstraintMotion::LCM_Free); break;
			case 1: Constraint.SetLinearXMotion(ELinearConstraintMotion::LCM_Limited); break;
			case 2: Constraint.SetLinearXMotion(ELinearConstraintMotion::LCM_Locked); break;
		}
		switch (YLimit)
		{
			case 0: Constraint.SetLinearYMotion(ELinearConstraintMotion::LCM_Free); break;
			case 1: Constraint.SetLinearYMotion(ELinearConstraintMotion::LCM_Limited); break;
			case 2: Constraint.SetLinearYMotion(ELinearConstraintMotion::LCM_Locked); break;
		}
		switch (ZLimit)
		{
			case 0: Constraint.SetLinearZMotion(ELinearConstraintMotion::LCM_Free); break;
			case 1: Constraint.SetLinearZMotion(ELinearConstraintMotion::LCM_Limited); break;
			case 2: Constraint.SetLinearZMotion(ELinearConstraintMotion::LCM_Locked); break;
		}

		Constraint.SetLinearLimitSize(Size);

		// Soft Limit?
		if (SoftLimit) Constraint.ProfileInstance.LinearLimit.bSoftConstraint = 1;
		else Constraint.ProfileInstance.LinearLimit.bSoftConstraint = 0;

		Constraint.ProfileInstance.LinearLimit.Stiffness = SoftStiffness;
		Constraint.ProfileInstance.LinearLimit.Damping = SoftDampening;
	}

	// Configure Skeletal Meshes
	static FORCEINLINE void SetupSKComponentsWithCollision(USkeletalMeshComponent* Comp)
	{
		if (!Comp) return;

		Comp->bOwnerNoSee = false;
		Comp->bCastDynamicShadow = true;
		Comp->CastShadow = true;
		Comp->BodyInstance.SetObjectType(ECC_WorldDynamic);
		Comp->BodyInstance.SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Comp->BodyInstance.SetResponseToAllChannels(ECR_Ignore);
		Comp->SetVisibility(false);
		Comp->SetHiddenInGame(false);
		Comp->bReceivesDecals = false;
	}

	// Configure Static Meshes
	static FORCEINLINE void SetupSMComponentsWithCollision(UStaticMeshComponent* Comp)
	{
		if (!Comp) return;

		Comp->bOwnerNoSee = false;
		Comp->bCastDynamicShadow = true;
		Comp->CastShadow = true;
		Comp->BodyInstance.SetObjectType(ECC_WorldDynamic);
		Comp->BodyInstance.SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Comp->BodyInstance.SetResponseToAllChannels(ECR_Ignore);
		Comp->SetHiddenInGame(false);
		Comp->bReceivesDecals = false;
	}

	// Get Float as Text with Precision
	static FORCEINLINE FText GetFloatAsTextWithPrecision(float TheFloat, int32 Precision, bool IncludeLeadingZero = true)
	{
		FNumberFormattingOptions NumberFormat;
		NumberFormat.MinimumIntegralDigits = (IncludeLeadingZero) ? 1 : 0;
		NumberFormat.MaximumIntegralDigits = 10000;
		NumberFormat.MinimumFractionalDigits = Precision;
		NumberFormat.MaximumFractionalDigits = Precision;
		return FText::AsNumber(TheFloat, &NumberFormat);
	}

	// Get Float As String With Precision
	static FORCEINLINE FString GetFloatAsStringWithPrecision(float TheFloat, int32 Precision, bool IncludeLeadingZero = true)
	{
		FNumberFormattingOptions NumberFormat;					//Text.h
		NumberFormat.MinimumIntegralDigits = (IncludeLeadingZero) ? 1 : 0;
		NumberFormat.MaximumIntegralDigits = 10000;
		NumberFormat.MinimumFractionalDigits = Precision;
		NumberFormat.MaximumFractionalDigits = Precision;
		return FText::AsNumber(TheFloat, &NumberFormat).ToString();
	}
};
