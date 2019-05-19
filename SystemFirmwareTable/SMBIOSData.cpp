#include "pch.h"
#include "SMBIOSData.h"
#include "SMBIOSData.g.cpp"

#include "RawSMBIOSData.h"

#include <Windows.h>
#include <sysinfoapi.h>

#pragma comment(lib, "kernel32")

#include "SMBIOSStructure.h"

namespace winrt::SystemFirmwareTable::implementation
{
    uint8_t SMBIOSData::Used20CallingMethod()
    {
		return ((RawSMBIOSData*)m_buffer.data())->Used20CallingMethod;
    }
    uint8_t SMBIOSData::SMBIOSMajorVersion()
    {
		return ((RawSMBIOSData*)m_buffer.data())->SMBIOSMajorVersion;
    }
    uint8_t SMBIOSData::SMBIOSMinorVersion()
    {
		return ((RawSMBIOSData*)m_buffer.data())->SMBIOSMinorVersion;
    }
    uint8_t SMBIOSData::DmiRevision()
    {
		return ((RawSMBIOSData*)m_buffer.data())->DmiRevision;
    }
    int32_t SMBIOSData::Length()
    {
		return ((RawSMBIOSData*)m_buffer.data())->Length;
    }
    Windows::Foundation::Collections::IVectorView<SystemFirmwareTable::SMBIOSStructure> SMBIOSData::SMBIOSTableData()
    {
		std::vector<winrt::SystemFirmwareTable::SMBIOSStructure> vec;

		byte* start_ptr = ((RawSMBIOSData*)m_buffer.data())->SMBIOSStructureData;
		byte* end_ptr = start_ptr + ((RawSMBIOSData*)m_buffer.data())->Length;

		while (start_ptr < end_ptr) {
			byte length = ((RawSMBIOSHeader*)start_ptr)->Length;

			byte* str_st = start_ptr + length;
			byte* ptr = str_st + 1;
			while (!(*(ptr - 1) == 0x0 && *ptr == 0x0)) ptr++;

			size_t size = (ptr - start_ptr + 1);
			auto s = winrt::make<SMBIOSStructure>(SMBIOSMajorVersion(), SMBIOSMinorVersion(), start_ptr, start_ptr + size);
			vec.push_back(s);

			start_ptr += size;
		}
		return winrt::single_threaded_vector(std::move(vec)).GetView();
    }

	SMBIOSData::SMBIOSData() {
		size_t size = ::GetSystemFirmwareTable('RSMB', NULL, nullptr, 0);
		if (size == 0) {
			winrt::throw_last_error();
		}

		std::unique_ptr<byte[]> p_buffer(new byte[size]);

		size_t res = ::GetSystemFirmwareTable('RSMB', NULL, p_buffer.get(), size);
		if (res != size) {
			winrt::throw_last_error();
		}

		winrt::array_view<const byte> view(p_buffer.get(), p_buffer.get() + size);

		auto writer = winrt::Windows::Storage::Streams::DataWriter();
		writer.WriteBytes(view);
		m_buffer = writer.DetachBuffer();
		writer.Close();
	}

}
