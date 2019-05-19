#pragma once
#include "SMBIOSStructure.g.h"

namespace winrt::SystemFirmwareTable::implementation
{
    struct SMBIOSStructure : SMBIOSStructureT<SMBIOSStructure>
    {
	private:
        SMBIOSStructure();
	public:
		SMBIOSStructure(byte versionMajor, byte versionMinor, byte* begin, byte* end);

	public:
		hstring Name();
        uint8_t Type();
        uint8_t Length();
        uint16_t Handle();
        Windows::Foundation::Collections::IVectorView<SystemFirmwareTable::SMBIOSField> Values();

		uint8_t GetByte(uint8_t offset);
		uint16_t GetWord(uint8_t offset);
		uint32_t GetDword(uint8_t offset);
		uint64_t GetQword(uint8_t offset);
		hstring GetString(uint8_t offset);
		Windows::Foundation::Collections::IVectorView<uint8_t> GetBytes(uint8_t offset, uint8_t size);

	private:
		byte* m_begin;
		byte* m_end;
		std::vector<char*> m_strings;

		byte m_version_major;
		byte m_version_minor;
		bool check_version(byte major, byte minor);

		SystemFirmwareTable::SMBIOSField make_field(byte offset, hstring name, hstring type, Windows::Foundation::IInspectable value, hstring description);
		SystemFirmwareTable::SMBIOSField make_field_byte(byte offset, hstring name, hstring description);
		SystemFirmwareTable::SMBIOSField make_field_word(byte offset, hstring name, hstring description);
		SystemFirmwareTable::SMBIOSField make_field_dword(byte offset, hstring name, hstring description);
		SystemFirmwareTable::SMBIOSField make_field_qword(byte offset, hstring name, hstring description);
		SystemFirmwareTable::SMBIOSField make_field_string(byte offset, hstring name, hstring description);
		SystemFirmwareTable::SMBIOSField make_field_bytes(byte offset, byte size, hstring name, hstring description);
    };
}
