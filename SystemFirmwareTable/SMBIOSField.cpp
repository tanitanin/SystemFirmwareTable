#include "pch.h"
#include "SMBIOSField.h"
#include "SMBIOSField.g.cpp"

namespace winrt::SystemFirmwareTable::implementation
{
    uint8_t SMBIOSField::Offset()
    {
		return m_offset;
    }
	hstring SMBIOSField::Name()
	{
		return m_name;
	}
    hstring SMBIOSField::Type()
    {
		return m_type;
    }
    Windows::Foundation::IInspectable SMBIOSField::Value()
    {
		return m_value;
    }
    hstring SMBIOSField::Description()
    {
		return m_description;
    }

	void SMBIOSField::Offset(uint8_t value) {
		m_offset = value;
	}
	void SMBIOSField::Name(hstring value) {
		m_name = value;
	}
	void SMBIOSField::Type(hstring value) {
		m_type = value;
	}
	void SMBIOSField::Value(Windows::Foundation::IInspectable value) {
		m_value = value;
	}
	void SMBIOSField::Description(hstring value) {
		m_description = value;
	}

	SMBIOSField::SMBIOSField(byte offset, hstring name, hstring type, Windows::Foundation::IInspectable value, hstring description)
	: m_offset(offset), m_name(name), m_type(type), m_value(value), m_description(description) {

	}

}
