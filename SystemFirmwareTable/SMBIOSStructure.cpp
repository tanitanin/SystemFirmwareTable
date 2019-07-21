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

		}
		else if (Type() == 4) {

		}
		else if (Type() == 7) {

		}
		else if (Type() == 9) {

		}
		else if (Type() == 16) {

		}
		else if (Type() == 17) {

		}
		else if (Type() == 19) {

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
