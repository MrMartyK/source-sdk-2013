//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: FileSystem bridge implementation
//
//=============================================================================

#include "filesystem_bridge.h"
#include "filesystem.h"
#include "tier0/icommandline.h"

namespace S15 {

CFileSystemBridge::CFileSystemBridge() : m_pFileSystem(nullptr) {
}

CFileSystemBridge::~CFileSystemBridge() {
	Shutdown();
}

bool CFileSystemBridge::Init() {
	// TODO: Use CreateInterface to get IFileSystem
	// For now, stub returns false (will implement when needed)
	return false;
}

void CFileSystemBridge::Shutdown() {
	m_pFileSystem = nullptr;
}

bool CFileSystemBridge::FileExists(const char *pFileName, const char *pPathID) {
	if (!m_pFileSystem)
		return false;

	return m_pFileSystem->FileExists(pFileName, pPathID);
}

int CFileSystemBridge::GetFileSize(const char *pFileName, const char *pPathID) {
	if (!m_pFileSystem)
		return -1;

	return m_pFileSystem->Size(pFileName, pPathID);
}

} // namespace S15
