//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Color grading and tonemapping utilities for Source 1.5
//          Engine-agnostic color operations (no tier dependencies)
//
//=============================================================================

#ifndef COLOR_GRADING_H
#define COLOR_GRADING_H

namespace S15 {

// Simple 3D vector for RGB color operations
struct Vector3 {
	float x, y, z;

	Vector3() : x(0.0f), y(0.0f), z(0.0f) {
	}
	Vector3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {
	}
};

/**
 * ACES Filmic Tonemap (Narkowicz 2015 approximation)
 *
 * Maps HDR color values (0 to infinity) to LDR range (0 to 1)
 * using the ACES (Academy Color Encoding System) filmic curve.
 *
 * This is a close approximation to the full ACES RRT/ODT transform
 * using a simple polynomial fit.
 *
 * Reference: "ACES Filmic Tone Mapping Curve" by Krzysztof Narkowicz
 *            https://knarkowicz.wordpress.com/2016/01/06/aces-filmic-tone-mapping-curve/
 *
 * @param x Input HDR color (linear RGB, 0 to infinity)
 * @return Output LDR color (sRGB-ready, 0 to 1)
 */
Vector3 ACESFilm(const Vector3& x);

} // namespace S15

#endif // COLOR_GRADING_H
