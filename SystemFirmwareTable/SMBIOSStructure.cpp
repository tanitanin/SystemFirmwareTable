#include "pch.h"
#include "SMBIOSStructure.h"
#include "SMBIOSStructure.g.cpp"

#include "RawSMBIOSData.h"
#include "SMBIOSField.h"

namespace winrt::SystemFirmwareTable::implementation
{
    uint8_t SMBIOSStructure::Type()
    {
		return ((RawSMBIOSHeader *)m_begin)->Type;
    }
    uint8_t SMBIOSStructure::Length()
    {
		return ((RawSMBIOSHeader*)m_begin)->Length;
    }
    uint16_t SMBIOSStructure::Handle()
    {
		return ((RawSMBIOSHeader*)m_begin)->Handle;
    }

	uint8_t SMBIOSStructure::GetByte(uint8_t offset)
	{
		if (offset >= Length()) {
			throw winrt::hresult_out_of_bounds();
		}
		return *(uint8_t*)(&m_begin[offset]);
	}
	uint16_t SMBIOSStructure::GetWord(uint8_t offset)
	{
		if (offset + sizeof(uint16_t) - 1 >= Length()) {
			throw winrt::hresult_out_of_bounds();
		}
		return *(uint16_t *)(&m_begin[offset]);
	}
	uint32_t SMBIOSStructure::GetDword(uint8_t offset)
	{
		if (offset + sizeof(uint32_t) - 1 >= Length()) {
			throw winrt::hresult_out_of_bounds();
		}
		return *(uint32_t*)(&m_begin[offset]);
	}
	uint64_t SMBIOSStructure::GetQword(uint8_t offset)
	{
		if (offset + sizeof(uint64_t) - 1 >= Length()) {
			throw winrt::hresult_out_of_bounds();
		}
		return *(uint64_t*)(&m_begin[offset]);
	}
	hstring SMBIOSStructure::GetString(uint8_t offset)
	{
		auto index = GetByte(offset);
		if (index == 0) {
			return hstring();
		}
		if (index - 1 >= m_strings.size()) {
			throw winrt::hresult_out_of_bounds();
		}
		auto len = ::strlen(m_strings[index - 1]) + 1;
		std::unique_ptr<wchar_t[]> p_buf(new wchar_t[len]);
		size_t size;
		::mbstowcs_s(&size, p_buf.get(), len, m_strings[index - 1], len);
		hstring s(p_buf.get(), len);
		return s;
	}
	Windows::Foundation::Collections::IVectorView<uint8_t> SMBIOSStructure::GetBytes(uint8_t offset, uint8_t size)
	{
		if (offset + size - 1 >= Length()) {
			throw winrt::hresult_out_of_bounds();
		}
		winrt::array_view<byte> view(m_begin + offset, m_begin + offset + size);
		std::vector<byte> vec(view.begin(), view.end());

		return winrt::single_threaded_vector(std::move(vec)).GetView();
	}

	bool SMBIOSStructure::check_version(byte major, byte minor) {
		if (m_version_major > major) {
			return true;
		}
		if (m_version_major < major) {
			return false;
		}
		if (m_version_minor > minor) {
			return true;
		}
		if (m_version_minor < minor) {
			return false;
		}
		return true;
	}

	SystemFirmwareTable::SMBIOSField SMBIOSStructure::make_field(byte offset, hstring name, hstring type, Windows::Foundation::IInspectable value, hstring description) {
		auto ptr = winrt::make_self<SMBIOSField>();
		ptr->Offset(offset);
		ptr->Name(name);
		ptr->Type(type);
		ptr->Value(value);
		ptr->Description(description);
		return ptr.as<SystemFirmwareTable::SMBIOSField>();
	}

	SystemFirmwareTable::SMBIOSField SMBIOSStructure::make_field_byte(byte offset, hstring name, hstring description = L"") {
		hstring type = L"BYTE";
		auto value = GetByte(offset);
		return make_field(offset, name, type, winrt::box_value(value), description);
	}

	SystemFirmwareTable::SMBIOSField SMBIOSStructure::make_field_word(byte offset, hstring name, hstring description = L"") {
		hstring type = L"WORD";
		auto value = GetWord(offset);
		return make_field(offset, name, type, winrt::box_value(value), description);
	}
	SystemFirmwareTable::SMBIOSField SMBIOSStructure::make_field_dword(byte offset, hstring name, hstring description = L"") {
		hstring type = L"DWORD";
		auto value = GetByte(offset);
		return make_field(offset, name, type, winrt::box_value(value), description);
	}
	SystemFirmwareTable::SMBIOSField SMBIOSStructure::make_field_qword(byte offset, hstring name, hstring description = L"") {
		hstring type = L"QWORD";
		auto value = GetByte(offset);
		return make_field(offset, name, type, winrt::box_value(value), description);
	}
	SystemFirmwareTable::SMBIOSField SMBIOSStructure::make_field_string(byte offset, hstring name, hstring description = L"") {
		hstring type = L"String";
		auto value = GetString(offset);
		return make_field(offset, name, type, winrt::box_value(value), description);
	}
	SystemFirmwareTable::SMBIOSField SMBIOSStructure::make_field_bytes(byte offset, byte size, hstring name, hstring description = L"") {
		hstring type = L"BYTE[]";
		auto value = GetBytes(offset, size);
		return make_field(offset, name, type, value, description);
	}

	hstring SMBIOSStructure::Name() {
		switch (Type())
		{
		case 0:   return L"BIOS Information";
		case 1:   return L"System Information";
		case 3:   return L"System Enclosure";
		case 4:   return L"Processor Information";
		case 7:   return L"Cache Information";
		case 9:   return L"System Slots";
		case 16:  return L"Physical Memory Array";
		case 17:  return L"Memory Device";
		case 19:  return L"Memory Array Mapped Address";
		case 32:  return L"System Boot Information";
		default:  return L"Unknown";
		}
	}

    Windows::Foundation::Collections::IVectorView<SystemFirmwareTable::SMBIOSField> SMBIOSStructure::Values()
    {
		std::vector<winrt::SystemFirmwareTable::SMBIOSField> vec;

		winrt::SystemFirmwareTable::SMBIOSField field{ nullptr };

		if (check_version(2, 0)) vec.push_back(make_field_byte(0x00, L"Type"));
		if (check_version(2, 0)) vec.push_back(make_field_byte(0x01, L"Length"));
		if (check_version(2, 0)) vec.push_back(make_field_byte(0x02, L"Handle"));

		if (Type() == 0) {
			if (check_version(2, 0)) vec.push_back(make_field_string(0x04, L"Vender"));
			if (check_version(2, 0)) vec.push_back(make_field_string(0x05, L"BIOS Version"));
			if (check_version(2, 0)) vec.push_back(make_field_word(0x06, L"BIOS Starting Address Segment"));
			if (check_version(2, 0)) vec.push_back(make_field_string(0x08, L"BIOS Release Date"));
			if (check_version(2, 0)) vec.push_back(make_field_byte(0x09, L"BIOS ROM Size"));
			if (check_version(2, 0)) vec.push_back(make_field_qword(0x0A, L"BIOS Characteristics"));
			if (check_version(2, 4)) vec.push_back(make_field_bytes(0x12, 2, L"BIOS Characteristics Extension Bytes"));
			if (check_version(2, 4)) vec.push_back(make_field_byte(0x14, L"System BIOS Major Release"));
			if (check_version(2, 4)) vec.push_back(make_field_byte(0x15, L"System BIOS Minor Release"));
			if (check_version(2, 4)) vec.push_back(make_field_byte(0x16, L"Embedded Controller Firmware Major Release"));
			if (check_version(2, 4)) vec.push_back(make_field_byte(0x17, L"Embedded Controller Firmware Minor Release"));
			if (check_version(3, 2)) vec.push_back(make_field_word(0x18, L"Extended BIOS ROM Size"));
		}
		else if (Type() == 1) {
			if (check_version(2, 0)) vec.push_back(make_field_string(0x04, L"Manufacturer"));
			if (check_version(2, 0)) vec.push_back(make_field_string(0x05, L"Product Name"));
			if (check_version(2, 0)) vec.push_back(make_field_string(0x06, L"Version"));
			if (check_version(2, 0)) vec.push_back(make_field_string(0x07, L"Serial Number"));
			if (check_version(2, 1)) vec.push_back(make_field_bytes(0x08, 16, L"UUID"));
			if (check_version(2, 1)) vec.push_back(make_field_byte(0x18, L"Wake-up Type"));
			if (check_version(2, 4)) vec.push_back(make_field_string(0x19, L"SKU Number"));
			if (check_version(2, 4)) vec.push_back(make_field_string(0x1A, L"Family"));
		}
		else if (Type() == 3) {
			if (check_version(2, 0)) vec.push_back(make_field_string(0x04, L"Manufacturer"));
			if (check_version(2, 0)) vec.push_back(make_field_byte(0x05, L"Type"));
			if (check_version(2, 0)) vec.push_back(make_field_string(0x06, L"Version"));
			if (check_version(2, 0)) vec.push_back(make_field_string(0x07, L"Serial Number"));
			if (check_version(2, 0)) vec.push_back(make_field_string(0x08, L"Asset Tag Number"));
			if (check_version(2, 1)) vec.push_back(make_field_byte(0x09, L"Boot-up State"));
			if (check_version(2, 1)) vec.push_back(make_field_byte(0x0A, L"Power Supply State"));
			if (check_version(2, 1)) vec.push_back(make_field_byte(0x0B, L"Thermal State"));
			if (check_version(2, 1)) vec.push_back(make_field_byte(0x0C, L"Security Status"));
			if (check_version(2, 3)) vec.push_back(make_field_word(0x0D, L"OEM-defined"));
			if (check_version(2, 3)) vec.push_back(make_field_byte(0x11, L"Height"));
			if (check_version(2, 3)) vec.push_back(make_field_byte(0x12, L"Number of Power Cords"));
			byte n = 0, m = 0;
			if (check_version(2, 3)) {
				vec.push_back(make_field_byte(0x13, L"Contained Element Count"));
				n = GetByte(0x13);
			}
			if (check_version(2, 3)) {
				vec.push_back(make_field_byte(0x14, L"Contained Element Record Length"));
				m = GetByte(0x14);
			}
			if (check_version(2, 3)) vec.push_back(make_field_bytes(0x15, n * m, L"Contained Element Record"));
			if (check_version(2, 7)) vec.push_back(make_field_byte(0x15 + n * m, L"SKU Number"));
		}
		else if (Type() == 4) {
			if (check_version(2, 0)) vec.push_back(make_field_string(0x04, L"Socket Designation"));
			if (check_version(2, 0)) vec.push_back(make_field_byte(0x05, L"Processor Type"));
			if (check_version(2, 0)) vec.push_back(make_field_byte(0x06, L"Processor Family"));
			if (check_version(2, 0)) vec.push_back(make_field_string(0x07, L"Processor Manufacturer"));
			if (check_version(2, 0)) vec.push_back(make_field_qword(0x08, L"Processor ID"));
			if (check_version(2, 0)) vec.push_back(make_field_string(0x10, L"Processor Version"));
			if (check_version(2, 0)) vec.push_back(make_field_byte(0x11, L"Voltage"));
			if (check_version(2, 0)) vec.push_back(make_field_word(0x12, L"External Clock"));
			if (check_version(2, 0)) vec.push_back(make_field_word(0x14, L"Max Speed"));
			if (check_version(2, 0)) vec.push_back(make_field_word(0x16, L"Current Speed"));
			if (check_version(2, 0)) vec.push_back(make_field_byte(0x18, L"Status"));
			if (check_version(2, 0)) vec.push_back(make_field_byte(0x19, L"Processor Upgrade"));
			if (check_version(2, 1)) vec.push_back(make_field_word(0x1A, L"L1 Cache Handle"));
			if (check_version(2, 1)) vec.push_back(make_field_word(0x1C, L"L2 Cache Handle"));
			if (check_version(2, 1)) vec.push_back(make_field_word(0x1E, L"L3 Cache Handle"));
			if (check_version(2, 3)) vec.push_back(make_field_string(0x20, L"Serial Number"));
			if (check_version(2, 3)) vec.push_back(make_field_string(0x21, L"Asset Tag"));
			if (check_version(2, 3)) vec.push_back(make_field_string(0x22, L"Part Number"));
			if (check_version(2, 5)) vec.push_back(make_field_byte(0x23, L"Core Count"));
			if (check_version(2, 5)) vec.push_back(make_field_byte(0x24, L"Core Enabled"));
			if (check_version(2, 5)) vec.push_back(make_field_byte(0x25, L"Thread Count"));
			if (check_version(2, 5)) vec.push_back(make_field_word(0x26, L"Processor Characteristics"));
			if (check_version(2, 6)) vec.push_back(make_field_word(0x28, L"Processor Family 2"));
			if (check_version(3, 0)) vec.push_back(make_field_word(0x2A, L"Core Count 2"));
			if (check_version(3, 0)) vec.push_back(make_field_word(0x2C, L"Core Enabled 2"));
			if (check_version(3, 0)) vec.push_back(make_field_word(0x2E, L"Thread Count 2"));
		}
		else if (Type() == 7) {
			if (check_version(2, 0)) vec.push_back(make_field_string(0x04, L"Socket Designation"));
			if (check_version(2, 0)) vec.push_back(make_field_word(0x05, L"Cache Configuration"));
			if (check_version(2, 0)) vec.push_back(make_field_word(0x07, L"Maximum Cache Size"));
			if (check_version(2, 0)) vec.push_back(make_field_word(0x09, L"Installed Size"));
			if (check_version(2, 0)) vec.push_back(make_field_word(0x0B, L"Supported SRAM Type"));
			if (check_version(2, 0)) vec.push_back(make_field_word(0x0D, L"Current SRAM Type"));
			if (check_version(2, 0)) vec.push_back(make_field_byte(0x0F, L"Cache Speed"));
			if (check_version(2, 1)) vec.push_back(make_field_byte(0x10, L"Error Correction Type"));
			if (check_version(2, 1)) vec.push_back(make_field_byte(0x11, L"System Cache Type"));
			if (check_version(2, 1)) vec.push_back(make_field_byte(0x12, L"Associativity"));
			if (check_version(3, 1)) vec.push_back(make_field_dword(0x13, L"Maximum Cache Size 2"));
			if (check_version(3, 1)) vec.push_back(make_field_dword(0x17, L"Installed Cache Size 2"));
		}
		else if (Type() == 9) {
			if (check_version(2, 0)) vec.push_back(make_field_string(0x04, L"Slot Designation"));
			if (check_version(2, 0)) vec.push_back(make_field_byte(0x05, L"Slot Type"));
			if (check_version(2, 0)) vec.push_back(make_field_byte(0x06, L"Slot Data Bus Width"));
			if (check_version(2, 0)) vec.push_back(make_field_byte(0x07, L"Current Usage"));
			if (check_version(2, 0)) vec.push_back(make_field_byte(0x08, L"Slot Length"));
			if (check_version(2, 0)) vec.push_back(make_field_word(0x09, L"Slot ID"));
			if (check_version(2, 0)) vec.push_back(make_field_byte(0x0B, L"Slot Characteristics 1"));
			if (check_version(2, 1)) vec.push_back(make_field_byte(0x0C, L"Slot Characteristics 2"));
			if (check_version(2, 6)) vec.push_back(make_field_word(0x0D, L"Segment Group Number (Base)"));
			if (check_version(2, 6)) vec.push_back(make_field_byte(0x0F, L"Bus Number (Base) "));
			if (check_version(2, 6)) vec.push_back(make_field_byte(0x10, L"Device/Function Number(Base)"));
			if (check_version(3, 2)) vec.push_back(make_field_byte(0x11, L"Data Bus Width (Base)"));
			byte n = 0;
			if (check_version(3, 2)) { 
				vec.push_back(make_field_byte(0x12, L"Peer (S/B/D/F/Width) grouping count(n)"));
				n = GetByte(0x12);
			}
			if (check_version(3, 2)) vec.push_back(make_field_bytes(0x13, 5 * n,  L"Peer (S/B/D/F/Width) groups"));
		}
		else if (Type() == 16) {
			if (check_version(2, 1)) vec.push_back(make_field_byte(0x04, L"Location"));
			if (check_version(2, 1)) vec.push_back(make_field_byte(0x05, L"Use"));
			if (check_version(2, 1)) vec.push_back(make_field_byte(0x06, L"Memory Error Correction"));
			if (check_version(2, 1)) vec.push_back(make_field_dword(0x07, L"Maximum Capacity"));
			if (check_version(2, 1)) vec.push_back(make_field_word(0x0B, L"Memory Error Information Handle"));
			if (check_version(2, 1)) vec.push_back(make_field_word(0x0D, L"Number of Memory Devices"));
			if (check_version(2, 7)) vec.push_back(make_field_qword(0x0F, L"Extended Maximum Capacity"));
		}
		else if (Type() == 17) {
			if (check_version(2, 1)) vec.push_back(make_field_word(0x04, L"Physical Memory Array Handle"));
			if (check_version(2, 1)) vec.push_back(make_field_word(0x06, L"Memory Error Information Handle"));
			if (check_version(2, 1)) vec.push_back(make_field_word(0x08, L"Total Width"));
			if (check_version(2, 1)) vec.push_back(make_field_word(0x0A, L"Data Width"));
			if (check_version(2, 1)) vec.push_back(make_field_word(0x0C, L"Size"));
			if (check_version(2, 1)) vec.push_back(make_field_byte(0x0E, L"Form Factor"));
			if (check_version(2, 1)) vec.push_back(make_field_byte(0x0F, L"Device Set"));
			if (check_version(2, 1)) vec.push_back(make_field_string(0x10, L"Device Locator"));
			if (check_version(2, 1)) vec.push_back(make_field_string(0x11, L"Bank Locator "));
			if (check_version(2, 1)) vec.push_back(make_field_byte(0x12, L"Memory Type"));
			if (check_version(2, 1)) vec.push_back(make_field_word(0x13, L"Type Detail"));
			if (check_version(2, 3)) vec.push_back(make_field_word(0x15, L"Speed"));
			if (check_version(2, 3)) vec.push_back(make_field_string(0x17, L"Manufacturer"));
			if (check_version(2, 3)) vec.push_back(make_field_string(0x18, L"Serial Number"));
			if (check_version(2, 3)) vec.push_back(make_field_string(0x19, L"Asset Tag"));
			if (check_version(2, 3)) vec.push_back(make_field_string(0x1A, L"Part Number"));
			if (check_version(2, 6)) vec.push_back(make_field_byte(0x1B, L"Attributes"));
			if (check_version(2, 7)) vec.push_back(make_field_dword(0x1C, L"Extended Size"));
			if (check_version(2, 7)) vec.push_back(make_field_word(0x20, L"Configured Memory Speed"));
			if (check_version(2, 8)) vec.push_back(make_field_word(0x22, L"Minimum voltage"));
			if (check_version(2, 8)) vec.push_back(make_field_word(0x24, L"Maximum voltage"));
			if (check_version(2, 8)) vec.push_back(make_field_word(0x26, L"Configured voltage"));
			if (check_version(3, 2)) vec.push_back(make_field_byte(0x28, L"Memory Technology"));
			if (check_version(3, 2)) vec.push_back(make_field_word(0x29, L"Memory Operating Mode Capability"));
			if (check_version(3, 2)) vec.push_back(make_field_string(0x2B, L"Firmware Version"));
			if (check_version(3, 2)) vec.push_back(make_field_word(0x2C, L"Module Manufacturer ID"));
			if (check_version(3, 2)) vec.push_back(make_field_word(0x2E, L"Module Product ID"));
			if (check_version(3, 2)) vec.push_back(make_field_word(0x30, L"Memory Subsystem Controller Manufacturer ID"));
			if (check_version(3, 2)) vec.push_back(make_field_word(0x32, L"Memory Subsystem Controller Product ID"));
			if (check_version(3, 2)) vec.push_back(make_field_qword(0x34, L"Non-volatile Size"));
			if (check_version(3, 2)) vec.push_back(make_field_qword(0x3C, L"Volatile Size"));
			if (check_version(3, 2)) vec.push_back(make_field_qword(0x44, L"Cache Size"));
			if (check_version(3, 2)) vec.push_back(make_field_qword(0x4C, L"Logical Size"));
		}
		else if (Type() == 19) {
			if (check_version(2, 1)) vec.push_back(make_field_dword(0x04, L"Starting Address"));
			if (check_version(2, 1)) vec.push_back(make_field_dword(0x08, L"Ending Address"));
			if (check_version(2, 1)) vec.push_back(make_field_word(0x0C, L"Memory Array Handle"));
			if (check_version(2, 1)) vec.push_back(make_field_byte(0x0E, L"Partition Width"));
			if (check_version(2, 7)) vec.push_back(make_field_qword(0x0F, L"Extended Starting Address"));
			if (check_version(2, 7)) vec.push_back(make_field_qword(0x17, L"Extended Ending Address"));
		}
		else if (Type() == 32) {

		}
		return winrt::single_threaded_vector(std::move(vec)).GetView();
	}

	SMBIOSStructure::SMBIOSStructure() {}

	SMBIOSStructure::SMBIOSStructure(byte versionMajor, byte versionMinor, byte* begin, byte* end)
		: m_begin(begin), m_end(end), m_strings(), m_version_major(versionMajor), m_version_minor(versionMinor)
	{
		byte length = *(begin + 1);

		byte * st = begin + length;
		while (st < end) {
			m_strings.push_back((char*)st);
			st += ::strnlen_s((char*)st, end - st) + 1;
		}
	}

}
