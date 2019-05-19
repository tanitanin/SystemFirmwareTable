#pragma once
#include "SMBIOSField.g.h"

namespace winrt::SystemFirmwareTable::implementation
{
    struct SMBIOSField : SMBIOSFieldT<SMBIOSField>
    {
        SMBIOSField() = default;
		SMBIOSField(byte offset, hstring name, hstring type, Windows::Foundation::IInspectable value, hstring description);

        uint8_t Offset();
		hstring Name();
        hstring Type();
        Windows::Foundation::IInspectable Value();
        hstring Description();

		void Offset(uint8_t value);
		void Name(hstring value);
		void Type(hstring value);
		void Value(Windows::Foundation::IInspectable value);
		void Description(hstring value);

	private:
		uint8_t m_offset;
		hstring m_name;
		hstring m_type;
		Windows::Foundation::IInspectable m_value;
		hstring m_description;
    };
}
