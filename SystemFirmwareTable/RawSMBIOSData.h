#pragma once

#include <Windows.h>

struct RawSMBIOSData
{
	BYTE    Used20CallingMethod;
	BYTE    SMBIOSMajorVersion;
	BYTE    SMBIOSMinorVersion;
	BYTE    DmiRevision;
	DWORD   Length;
	BYTE    SMBIOSStructureData[];
};

struct RawSMBIOSHeader {
	BYTE	Type;
	BYTE	Length;
	DWORD	Handle;
};
