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

namespace ProducerAndConsumer
{
    public partial class Form1 : Form
    {
        private BlockingCollection<int> buffer = new BlockingCollection<int>(boundedCapacity: 5);
        private CancellationTokenSource cancellationTokenSource = new CancellationTokenSource();
        private CancellationToken cancellationToken;

        private Task producerTask;
        private Task consumerTask;
        private int producerDelay = 1000;
        private int consumerDelay = 2500;
        private bool stopThreads = false;
        public Form1()
        {
            InitializeComponent();
            cancellationToken = cancellationTokenSource.Token;
        }
        private void MainForm_Load(object sender, EventArgs e)
        {
            producerTask = Task.Run(() => Producer(), cancellationToken);
            consumerTask = Task.Run(() => Consumer(), cancellationToken);
            producerTrackBar.Value = 2;
            CustomerTrackBar2.Value = 1;
        }

        private void Producer()
        {
            while(!stopThreads)
            {
                try
                {
                    for (int i = 1; i <= 100; i++)
                    {
                        buffer.Add(i);
                        string prod = $"Produced: {i}";
                        UpdateStatus(prod);
                        ResultRichTextBox.Invoke((MethodInvoker)delegate {
                            ResultRichTextBox.Text += prod + '\n';
                        });
                        UpdateBuffer();
                        Thread.Sleep(producerDelay);
                    }
                }
                finally
                {
                    buffer.CompleteAdding();
                }
            }
        }

        private void Consumer()
        {
            while(!stopThreads)
            { 
            try
            {
                foreach (var item in buffer.GetConsumingEnumerable(cancellationToken))
                {
                    string consum = $"Consumed: {item}";
                    UpdateStatus(consum);
                    ResultRichTextBox.Invoke((MethodInvoker)delegate {
                        ResultRichTextBox.Text += consum + '\n';
                    });
                    UpdateBuffer();
                    Thread.Sleep(consumerDelay);
                }
            }
            catch (OperationCanceledException)
            {
                // Ignore if the operation is canceled
            }
            }
        }

        private void UpdateStatus(string status)
        {
            if (InvokeRequired)
            {
                Invoke(new Action<string>(UpdateStatus), status);
            }
            else
            {
                statusLabel.Text = status;
            }
        }

        private void UpdateBuffer()
        {
            if (InvokeRequired)
            {
                Invoke(new Action(UpdateBuffer));
            }
            else
            {
                bufferListBox.Items.Clear();
                foreach (var item in buffer)
                {
                    bufferListBox.Items.Add(item);
                }
            }
        }

        private void startButton_Click(object sender, EventArgs e)
        {
            startButton.Enabled = false;
            stopThreads = false;
            // Start producer and consumer tasks
            producerTask = Task.Run(() => Producer(), cancellationToken);
            consumerTask = Task.Run(() => Consumer(), cancellationToken);
        }

        private void MainForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            // Cancel the tasks and wait for their completion
            cancellationTokenSource.Cancel();
            Task.WaitAll(producerTask, consumerTask);
        }
        private int GetSleepIntervalThread1FromTrackBarValue(int trackBarValue)
        {
            switch (trackBarValue)
            {
                case 0:
                    return 5000;
                case 1:
                    return 4000;
                case 2:
                    return 3000;
                case 3:
                    return 1500;
                case 4:
                    return 500;
                default:
                    return 1000;
            }
        }
        private int GetSleepIntervalThread2FromTrackBarValue(int trackBarValue)
        {
            switch (trackBarValue)
            {
                case 0:
                    return 5000;
                case 1:
                    return 4000;
                case 2:
                    return 3000;
                case 3:
                    return 2000;
                case 4:
                    return 1000;
                default:
                    return 3000;
            }
        }
        private void producerTrackBar_ValueChanged(object sender, EventArgs e)
        {
            this.producerDelay = GetSleepIntervalThread1FromTrackBarValue(producerTrackBar.Value);
        }

        private void CustomerTrackBar2_ValueChanged(object sender, EventArgs e)
        {
            this.consumerDelay = GetSleepIntervalThread2FromTrackBarValue(CustomerTrackBar2.Value);
        }

        private void button1_Click(object sender, EventArgs e)
        {
            startButton.Enabled = true;
            stopThreads = true;
        }
    }
}
