//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Bridge to IMaterialSystem for tools/tests
//
//=============================================================================

#ifndef MATERIALSYSTEM_BRIDGE_H
#define MATERIALSYSTEM_BRIDGE_H

#ifdef _WIN32
#pragma once
#endif

class IMaterialSystem;
class IMaterial;

namespace S15 {

class CMaterialSystemBridge {
public:
	CMaterialSystemBridge();
	~CMaterialSystemBridge();

	bool Init();
	void Shutdown();

	// Convenience methods
	IMaterial* FindMaterial(const char *pMaterialName, const char *pTextureGroupName);
	void ReloadMaterial(const char *pMaterialName);

	IMaterialSystem* GetInterface() { return m_pMaterialSystem; }

private:
	IMaterialSystem *m_pMaterialSystem;
};

} // namespace S15

#endif // MATERIALSYSTEM_BRIDGE_H
