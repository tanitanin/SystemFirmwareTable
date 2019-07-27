using ReadSMBIOS.Core.Helpers;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
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
            foreach (var structure in data.SMBIOSTableData.OrderBy(x=>x.Type))
            {
                switch (structure.Type)
                {
                    case 0x0:
                    case 0x1:
                    case 0x3:
                    case 0x4:
                    case 0x7:
                        Items.Add(new BIOSStructureItem(structure));
                        break;
                }
            }
        }
    }
}
