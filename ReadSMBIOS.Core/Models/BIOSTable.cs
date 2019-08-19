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
                    case 0:
                    case 1:
                    case 3:
                    case 4:
                    case 7:
                    case 9:
                    case 16:
                    case 17:
                    case 19:
                    case 32:
                        Items.Add(new BIOSStructureItem(structure));
                        break;
                }
            }
        }
    }
}
