#pragma once
#include "RSMB.g.h"

namespace winrt::SystemFirmwareTable::implementation
{
    struct RSMB : RSMBT<RSMB>
    {
	public:
		RSMB() = default;
        static SystemFirmwareTable::SMBIOSData Load();

    };
}
namespace winrt::SystemFirmwareTable::factory_implementation
{
    struct RSMB : RSMBT<RSMB, implementation::RSMB>
    {
    };
}
