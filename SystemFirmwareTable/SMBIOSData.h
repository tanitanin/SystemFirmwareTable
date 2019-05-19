#pragma once
#include "SMBIOSData.g.h"

#include <winrt/Windows.Storage.Streams.h>

namespace winrt::SystemFirmwareTable::implementation
{
	struct SMBIOSData : SMBIOSDataT<SMBIOSData>
	{
	private:

	public:
		SMBIOSData();
		~SMBIOSData() {}

		uint8_t Used20CallingMethod();
		uint8_t SMBIOSMajorVersion();
		uint8_t SMBIOSMinorVersion();
		uint8_t DmiRevision();
		int32_t Length();
		Windows::Foundation::Collections::IVectorView<SystemFirmwareTable::SMBIOSStructure> SMBIOSTableData();

	private:
		//std::unique_ptr<byte[]> m_buffer;
		winrt::Windows::Storage::Streams::IBuffer m_buffer{ nullptr };
	};
}
