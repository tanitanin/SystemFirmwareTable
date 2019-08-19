using System;
using System.Collections.Generic;
using System.Text;
using System.Linq;

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
            if (Value is IEnumerable<byte> bytes)
            {
                Value = bytes.Aggregate("", (a, b) => a + $" {b:X2}").Trim();
            }
        }

        public string Offset { get; set; }
        public string Name { get; set; }
        public string Type { get; set; }
        public object Value { get; set; }
    }
}
