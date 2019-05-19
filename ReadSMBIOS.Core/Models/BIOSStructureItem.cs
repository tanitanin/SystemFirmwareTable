using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Text;

namespace ReadSMBIOS.Core.Models
{
    public class BIOSStructureItem
    {
        public string Name { get; }
        public byte Type { get; }
        public byte Length { get; }
        public ushort Handle { get; }
        public ObservableCollection<BIOSFieldItem> Items { get; } = new ObservableCollection<BIOSFieldItem>();

        public string Header { get => $"Type {Type} - {Name} ({Length} bytes)"; }

        public BIOSStructureItem(SystemFirmwareTable.SMBIOSStructure structure)
        {
            Name = structure.Name;
            Type = structure.Type;
            Length = structure.Length;
            Handle = structure.Handle;
            foreach (var value in structure.Values)
            {
                Items.Add(new BIOSFieldItem(value));
            }
        }
    }
}
