//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Color grading and tonemapping utilities implementation
//
//=============================================================================

#include "color_grading.h"
#include <algorithm> // For std::min, std::max

namespace S15 {

// Helper: Saturate float to [0, 1] range
static inline float Saturate(float value) {
	return std::min(1.0f, std::max(0.0f, value));
}

// Helper: Clamp negative values to zero
static inline float ClampNegative(float value) {
	return std::max(0.0f, value);
}

Vector3 ACESFilm(const Vector3& x) {
	// Narkowicz 2015 ACES approximation coefficients
	const float a = 2.51f;
	const float b = 0.03f;
	const float c = 2.43f;
	const float d = 0.59f;
	const float e = 0.14f;

	// Clamp negative inputs to zero (HDR should never be negative)
	float cx = ClampNegative(x.x);
	float cy = ClampNegative(x.y);
	float cz = ClampNegative(x.z);

	// Apply ACES tonemap per-channel
	// Formula: (x * (a*x + b)) / (x * (c*x + d) + e)
	Vector3 result;
	result.x = Saturate((cx * (a * cx + b)) / (cx * (c * cx + d) + e));
	result.y = Saturate((cy * (a * cy + b)) / (cy * (c * cy + d) + e));
	result.z = Saturate((cz * (a * cz + b)) / (cz * (c * cz + d) + e));

	return result;
}

} // namespace S15
