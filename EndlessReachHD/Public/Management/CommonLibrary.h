// Â© 2014 - 2017 Soverance Studios
// http://www.soverance.com

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
	
	// SetAngularLimits for Physics Constraints
	// THE PROPERTIES FOR CONSTRAINTS HAVE BEEN DEPRECATED WITHOUT DOCUMENTATION!
	static FORCEINLINE void SetAngularLimits(
		FConstraintInstance& Constraint,
		const uint8 S1Lim, const uint8 S2Lim, const uint8 TLim,
		const float Swing1LimitAngle,
		const float Swing2LimitAngle,
		const float TwistLimitAngle,

		bool SoftSwingLimit = true, bool SoftTwistLimit = true,
		const float SwingStiff = 0, const float SwingDamp = 0,
		const float TwistStiff = 0, const float TwistDamp = 0
	)
	{
		switch (S1Lim)
		{
		case 0: Constraint.AngularSwing1Motion_DEPRECATED = EAngularConstraintMotion::ACM_Free; break;
		case 1: Constraint.AngularSwing1Motion_DEPRECATED = EAngularConstraintMotion::ACM_Limited; break;
		case 2: Constraint.AngularSwing1Motion_DEPRECATED = EAngularConstraintMotion::ACM_Locked; break;
		}
		switch (S2Lim)
		{
		case 0: Constraint.AngularSwing2Motion_DEPRECATED = EAngularConstraintMotion::ACM_Free; break;
		case 1: Constraint.AngularSwing2Motion_DEPRECATED = EAngularConstraintMotion::ACM_Limited; break;
		case 2: Constraint.AngularSwing2Motion_DEPRECATED = EAngularConstraintMotion::ACM_Locked; break;
		}
		switch (TLim)
		{
		case 0: Constraint.AngularTwistMotion_DEPRECATED = EAngularConstraintMotion::ACM_Free; break;
		case 1: Constraint.AngularTwistMotion_DEPRECATED = EAngularConstraintMotion::ACM_Limited; break;
		case 2: Constraint.AngularTwistMotion_DEPRECATED = EAngularConstraintMotion::ACM_Locked; break;
		}
		//~~~~~~~~~~

		//Soft Lmit?
		if (SoftSwingLimit) Constraint.bSwingLimitSoft_DEPRECATED = 1;
		else Constraint.bSwingLimitSoft_DEPRECATED = 0;

		if (SoftTwistLimit) Constraint.bTwistLimitSoft_DEPRECATED = 1;
		else Constraint.bTwistLimitSoft_DEPRECATED = 0;

		//Limit Angles
		Constraint.Swing1LimitAngle_DEPRECATED = Swing1LimitAngle;
		Constraint.Swing2LimitAngle_DEPRECATED = Swing2LimitAngle;
		Constraint.TwistLimitAngle_DEPRECATED = TwistLimitAngle;

		Constraint.SwingLimitStiffness_DEPRECATED = SwingStiff;
		Constraint.SwingLimitDamping_DEPRECATED = SwingDamp;
		Constraint.TwistLimitStiffness_DEPRECATED = TwistStiff;
		Constraint.TwistLimitDamping_DEPRECATED = TwistDamp;
	}

	// SetLinearLimits for Physics Constraints
	// THE PROPERTIES FOR CONSTRAINTS HAVE BEEN DEPRECATED WITHOUT DOCUMENTATION!
	static FORCEINLINE void SetLinearLimits(
		FConstraintInstance& Constraint,
		bool bDisableCollision,
		const uint8 XLim, const uint8 YLim, const uint8 ZLim,
		const float Size,
		bool SoftLimit = true,
		const float SoftStiffness = 0,
		const float SoftDampening = 0
	)
	{
		//Collision
		Constraint.bDisableCollision_DEPRECATED = bDisableCollision;

		switch (XLim)
		{
		case 0: Constraint.LinearXMotion_DEPRECATED = ELinearConstraintMotion::LCM_Free; break;
		case 1: Constraint.LinearXMotion_DEPRECATED = ELinearConstraintMotion::LCM_Limited; break;
		case 2: Constraint.LinearXMotion_DEPRECATED = ELinearConstraintMotion::LCM_Locked; break;
		}
		switch (YLim)
		{
		case 0: Constraint.LinearYMotion_DEPRECATED = ELinearConstraintMotion::LCM_Free; break;
		case 1: Constraint.LinearYMotion_DEPRECATED = ELinearConstraintMotion::LCM_Limited; break;
		case 2: Constraint.LinearYMotion_DEPRECATED = ELinearConstraintMotion::LCM_Locked; break;
		}
		switch (ZLim)
		{
		case 0: Constraint.LinearZMotion_DEPRECATED = ELinearConstraintMotion::LCM_Free; break;
		case 1: Constraint.LinearZMotion_DEPRECATED = ELinearConstraintMotion::LCM_Limited; break;
		case 2: Constraint.LinearZMotion_DEPRECATED = ELinearConstraintMotion::LCM_Locked; break;
		}
		//~~~~~~~~~~

		Constraint.LinearLimitSize_DEPRECATED = Size;

		if (SoftLimit) Constraint.bLinearLimitSoft_DEPRECATED = 1;
		else Constraint.bLinearLimitSoft_DEPRECATED = 0;

		Constraint.LinearLimitStiffness_DEPRECATED = SoftStiffness;
		Constraint.LinearLimitDamping_DEPRECATED = SoftDampening;
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
