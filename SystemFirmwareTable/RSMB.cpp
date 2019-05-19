#include "pch.h"
#include "RSMB.h"
#include "RSMB.g.cpp"

#include "SMBIOSData.h"

namespace winrt::SystemFirmwareTable::implementation
{
	SystemFirmwareTable::SMBIOSData RSMB::Load()
    {
		winrt::SystemFirmwareTable::SMBIOSData data{ nullptr };
		
		auto d = winrt::make<SMBIOSData>();
		return d.as<SystemFirmwareTable::SMBIOSData>();
    }
}
