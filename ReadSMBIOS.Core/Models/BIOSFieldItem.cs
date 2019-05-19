using System;
using System.Collections.Generic;
using System.Text;

namespace ReadSMBIOS.Core.Models
{
    public class BIOSFieldItem
    {
        public BIOSFieldItem(SystemFirmwareTable.SMBIOSField value)
        {
            Offset = $"0x{value.Offset:X2}";
            Name = value.Name;
            Type = value.Type;
            Value = value.Value;
        }

        public string Offset { get; set; }
        public string Name { get; set; }
        public string Type { get; set; }
        public object Value { get; set; }
    }
}
