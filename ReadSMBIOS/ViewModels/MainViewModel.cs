using System;
using System.Collections.ObjectModel;
using System.Threading.Tasks;
using ReadSMBIOS.Core.Models;
using ReadSMBIOS.Helpers;


namespace ReadSMBIOS.ViewModels
{
    public class MainViewModel : Observable
    {
        public BIOSTable BIOSTable
        {
            get => this.table;
            set
            {
                Set(ref this.table, value);
            }
        }
        private BIOSTable table = null;

        public MainViewModel()
        {
        }

        public void Initialize()
        {
            table = BIOSTable.Current;
        }
    }
}
