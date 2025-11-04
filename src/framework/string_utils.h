//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: String utilities for Source 1.5 framework
//
//=============================================================================

#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#ifdef _WIN32
#pragma once
#endif

/**
 * Framework string utilities - engine-agnostic helpers
 * Use these instead of tier1 string functions when building
 * tools or modules that should not depend on engine libs
 */

namespace S15 {

/**
 * Safe string copy with null termination guarantee
 * @param pDest Destination buffer
 * @param pSrc Source string
 * @param nDestSize Size of destination buffer in bytes
 * @return Number of characters copied (excluding null terminator)
 */
int StringCopy(char *pDest, const char *pSrc, int nDestSize);

/**
 * Case-insensitive string comparison
 * @return 0 if equal, <0 if s1 < s2, >0 if s1 > s2
 */
int StringCompareI(const char *s1, const char *s2);

/**
 * Check if string ends with suffix (case-sensitive)
 */
bool StringEndsWith(const char *str, const char *suffix);

/**
 * Check if string ends with suffix (case-insensitive)
 */
bool StringEndsWithI(const char *str, const char *suffix);

/**
 * Simple path manipulation - get file extension
 * @return Pointer to extension (without dot) or empty string if none
 */
const char* GetFileExtension(const char *pPath);

/**
 * Get filename from path (strips directory)
 * @return Pointer to filename portion of path
 */
const char* GetFilename(const char *pPath);

} // namespace S15

#endif // STRING_UTILS_H
