using ReadSMBIOS.Core.Helpers;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Text;

namespace ReadSMBIOS.Core.Models
{
    public class BIOSTable
    {
        public static BIOSTable Current { get => Singleton<BIOSTable>.Instance; }

        public int Length { get; }
        public string Version { get; }
        public ObservableCollection<BIOSStructureItem> Items { get; } = new ObservableCollection<BIOSStructureItem>();

        public BIOSTable()
        {
            var data = SystemFirmwareTable.RSMB.Load();
            Length = data.Length;
            Version = $"{data.SMBIOSMajorVersion}.{data.SMBIOSMinorVersion}";
            foreach (var structure in data.SMBIOSTableData)
            {
                if (structure.Type == 0x0) Items.Add(new BIOSStructureItem(structure));
                else if (structure.Type == 0x1) Items.Add(new BIOSStructureItem(structure));
                else if (structure.Type == 0x3) Items.Add(new BIOSStructureItem(structure));
                else if (structure.Type == 0x4) Items.Add(new BIOSStructureItem(structure));
            }
        }
    }
}
