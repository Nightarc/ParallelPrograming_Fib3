using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.Remoting.Messaging;
using System.Text;
using System.Threading.Tasks;

namespace ReadersAndWriters
{
    internal class WorkersContainer
    {
        List<Reader> _readers;
        List<Writer> _writers;

        public WorkersContainer()
        {
            _readers = new List<Reader>();
            _writers = new List<Writer>();
        }

        public Reader GetReader(int i) =>  _readers[i];
        public Writer GetWriter(int i) =>  _writers[i];

        public void addReader(Reader reader) => _readers.Add(reader);
        public void addWriter(Writer writer) => _writers.Add(writer);

        public void updateWorkersDelay(double delay)
        {
            for (int i = 0; i < _readers.Count; i++)
                _readers[i].sleepDelay = delay * ((double)_readers[i].sleepDelayTrackBar.Value / 1000);

            for (int i = 0; i < _writers.Count; i++)
                _writers[i].sleepDelay = delay * ((double)_writers[i].sleepDelayTrackBar.Value / 1000);
        }
    }
}
