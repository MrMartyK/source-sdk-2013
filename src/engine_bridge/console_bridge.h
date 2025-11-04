//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Bridge to engine console/cvar system
//
//=============================================================================

#ifndef CONSOLE_BRIDGE_H
#define CONSOLE_BRIDGE_H

#ifdef _WIN32
#pragma once
#endif

namespace S15 {

class CConsoleBridge {
public:
	CConsoleBridge();
	~CConsoleBridge();

	bool Init();
	void Shutdown();

	// Send console command to engine
	void ExecuteCommand(const char *pCommand);

	// Get cvar value
	const char* GetCvarString(const char *pCvarName);
	float GetCvarFloat(const char *pCvarName);
	int GetCvarInt(const char *pCvarName);

private:
	// TODO: Store engine interface pointers
};

} // namespace S15

#endif // CONSOLE_BRIDGE_H
