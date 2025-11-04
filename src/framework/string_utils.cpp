//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: String utilities implementation
//
//=============================================================================

#include "string_utils.h"
#include <ctype.h>
#include <string.h>

namespace S15 {

int StringCopy(char* pDest, const char* pSrc, int nDestSize) {
	if (!pDest || !pSrc || nDestSize <= 0)
		return 0;

	int i = 0;
	while (i < nDestSize - 1 && pSrc[i] != '\0') {
		pDest[i] = pSrc[i];
		i++;
	}
	pDest[i] = '\0';
	return i;
}

int StringCompareI(const char* s1, const char* s2) {
	if (!s1 || !s2)
		return (s1 == s2) ? 0 : (s1 ? 1 : -1);

	while (*s1 && *s2) {
		int c1 = tolower((unsigned char)*s1);
		int c2 = tolower((unsigned char)*s2);
		if (c1 != c2)
			return c1 - c2;
		s1++;
		s2++;
	}
	return tolower((unsigned char)*s1) - tolower((unsigned char)*s2);
}

bool StringEndsWith(const char* str, const char* suffix) {
	if (!str || !suffix)
		return false;

	size_t strLen = strlen(str);
	size_t suffixLen = strlen(suffix);

	if (suffixLen > strLen)
		return false;

	return strcmp(str + strLen - suffixLen, suffix) == 0;
}

bool StringEndsWithI(const char* str, const char* suffix) {
	if (!str || !suffix)
		return false;

	size_t strLen = strlen(str);
	size_t suffixLen = strlen(suffix);

	if (suffixLen > strLen)
		return false;

	return StringCompareI(str + strLen - suffixLen, suffix) == 0;
}

const char* GetFileExtension(const char* pPath) {
	if (!pPath)
		return "";

	const char* pDot = strrchr(pPath, '.');
	const char* pSlash = strrchr(pPath, '/');
	const char* pBackslash = strrchr(pPath, '\\');

	// Find the last slash (either forward or back)
	const char* pLastSlash = pSlash > pBackslash ? pSlash : pBackslash;

	// Extension must come after the last slash
	if (pDot && (!pLastSlash || pDot > pLastSlash))
		return pDot + 1;

	return "";
}

const char* GetFilename(const char* pPath) {
	if (!pPath)
		return "";

	const char* pSlash = strrchr(pPath, '/');
	const char* pBackslash = strrchr(pPath, '\\');

	const char* pLastSlash = pSlash > pBackslash ? pSlash : pBackslash;

	if (pLastSlash)
		return pLastSlash + 1;

	return pPath;
}

} // namespace S15
