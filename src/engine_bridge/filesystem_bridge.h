//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Bridge to engine's IFileSystem via CreateInterface
//
//=============================================================================

#ifndef FILESYSTEM_BRIDGE_H
#define FILESYSTEM_BRIDGE_H

#ifdef _WIN32
#pragma once
#endif

class IFileSystem;

namespace S15 {

/**
 * Wrapper around engine's IFileSystem for tools/tests
 * Uses CreateInterface to get the filesystem instance
 */
class CFileSystemBridge {
public:
	CFileSystemBridge();
	~CFileSystemBridge();

	bool Init();
	void Shutdown();

	// Convenience methods (wrap IFileSystem)
	bool FileExists(const char *pFileName, const char *pPathID = nullptr);
	int GetFileSize(const char *pFileName, const char *pPathID = nullptr);

	// Direct access to interface (for advanced use)
	IFileSystem* GetInterface() { return m_pFileSystem; }

private:
	IFileSystem *m_pFileSystem;
};

} // namespace S15

#endif // FILESYSTEM_BRIDGE_H
