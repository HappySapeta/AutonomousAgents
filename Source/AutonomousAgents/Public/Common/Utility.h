
#pragma once

#include <CoreMinimal.h>

/**
 * 
 */
class AUTONOMOUSAGENTS_API Utility
{
public:
	static bool IsPointInFOV(const FVector& EyeLocation, const FVector& LookingDirection, const FVector& Point, float MinimumSearchRadius, float MaximumSearchRadius, float FOVHalfAngle)
	{
		const FVector& PointVector = Point - EyeLocation;
		const float Distance = PointVector.Length();

		const float DotProduct = PointVector.GetSafeNormal().Dot(LookingDirection.GetSafeNormal());
		const float Angle = FMath::Abs(FMath::RadiansToDegrees(FMath::Acos(DotProduct)));
	
		return Distance > MinimumSearchRadius && Distance < MaximumSearchRadius && Angle < FOVHalfAngle;
	}
};
