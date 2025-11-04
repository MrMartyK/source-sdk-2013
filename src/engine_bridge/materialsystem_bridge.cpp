//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: MaterialSystem bridge implementation
//
//=============================================================================

#include "materialsystem_bridge.h"

namespace S15 {

CMaterialSystemBridge::CMaterialSystemBridge() : m_pMaterialSystem(nullptr) {
}

CMaterialSystemBridge::~CMaterialSystemBridge() {
	Shutdown();
}

bool CMaterialSystemBridge::Init() {
	// TODO: CreateInterface("VMaterialSystem080")
	return false;
}

void CMaterialSystemBridge::Shutdown() {
	m_pMaterialSystem = nullptr;
}

IMaterial* CMaterialSystemBridge::FindMaterial(const char *pMaterialName, const char *pTextureGroupName) {
	if (!m_pMaterialSystem)
		return nullptr;

	// TODO: Call m_pMaterialSystem->FindMaterial()
	return nullptr;
}

void CMaterialSystemBridge::ReloadMaterial(const char *pMaterialName) {
	if (!m_pMaterialSystem)
		return;

	// TODO: Send reload command via IVEngineClient
}

} // namespace S15
