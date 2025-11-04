//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Console bridge implementation
//
//=============================================================================

#include "console_bridge.h"

namespace S15 {

CConsoleBridge::CConsoleBridge() {
}

CConsoleBridge::~CConsoleBridge() {
	Shutdown();
}

bool CConsoleBridge::Init() {
	// TODO: Get IVEngineClient or cvar interface
	return false;
}

void CConsoleBridge::Shutdown() {
}

void CConsoleBridge::ExecuteCommand(const char *pCommand) {
	// TODO: engine->ClientCmd() or equivalent
}

const char* CConsoleBridge::GetCvarString(const char *pCvarName) {
	// TODO: cvar->FindVar()
	return "";
}

float CConsoleBridge::GetCvarFloat(const char *pCvarName) {
	return 0.0f;
}

int CConsoleBridge::GetCvarInt(const char *pCvarName) {
	return 0;
}

} // namespace S15
