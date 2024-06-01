using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace MyProducerConsumer
{
    public partial class Form1 : Form
    {
        Random random = new Random();
        const int logMessagesLimit = 10;
        BlockingCollection<int> buffer = new BlockingCollection<int>(logMessagesLimit);

        bool stop = false;
        const int producerDelay = 1000;
        const int consumerDelay = 1500;

        public Form1()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            stop = false;
            Task.Run(() => Producer());
            Task.Run(() => Consumer());

        }

        private void stopButton_Click(object sender, EventArgs e)
        {
            stop = true;
        }

        private void Producer()
        {
            while (!stop)
            {
                int item = random.Next(100);
                buffer.Add(item);
                string logMessage = String.Format($"Produced: {item}");
                log(logMessage);
                updateStorage();
                updateStatus(logMessage);
                Thread.Sleep(producerDelay);
            }

        }
        private void log(string text)
        {
            if (logListBox.InvokeRequired)
                logListBox.Invoke(new Action<string>(log), text);
            else
            {
                logListBox.Items.Add(text);
            }
        }
        private void updateStorage()
        {
            if (storageListBox.InvokeRequired)
                storageListBox.Invoke(new Action(updateStorage));
            else
            {
                storageListBox.Items.Clear();
                foreach (var item in buffer)
                    storageListBox.Items.Add(item);
            }
        }
        private void updateStatus(string text)
        {
            if (lastLogMessage.InvokeRequired)
                lastLogMessage.Invoke(new Action<string>(updateStatus), text);
            else lastLogMessage.Text = text;
        }
        private void Consumer()
        {
            while (!stop)
            {
                int item = buffer.Take();
                string logMessage = String.Format($"Consumed: {item}");
                log(logMessage);
                updateStorage();
                updateStatus(logMessage);
                Thread.Sleep(consumerDelay);
            }
        }

        private void producerButton_Click(object sender, EventArgs e)
        {
            Task.Run(() => Producer());
        }

        private void consumerButton_Click(object sender, EventArgs e)
        {
            Task.Run(() => Consumer());

        }

        private void Clear_Click(object sender, EventArgs e)
        {
            stop = true;
            logListBox.Items.Clear();
            while(buffer.Count > 0) { buffer.Take(); }
            storageListBox.Items.Clear();
        }
    }
}
