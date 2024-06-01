using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace BackgroundProcessing
{
    public partial class BPForm : Form
    {
        const int limit = 10;
        BlockingCollection<int[]> arrays = new BlockingCollection<int[]>(limit);
        BlockingCollection<int> sums = new BlockingCollection<int>(limit);

        bool stop = false;

        static readonly object lockObj = new object(); // Объект блокировки для синхронизации доступа к файлу

        public BPForm()
        {
            InitializeComponent();
        }

        //Main method
        private void Start()
        {
            stop = false;
            using (StreamWriter sw = new StreamWriter("output.txt", false)) { }
            var task1 = Task.Run(() => Reader("input.txt", "output.txt"));
            var task2 = Task.Run(() => Processor("input.txt", "output.txt"));

        }

        //Button event handlers
        private void startButton_Click(object sender, EventArgs e)
        {
            Start();
        }

        //Thread methods

        /// <summary>
        /// Читает массив целых чисел из файла и записывает его в коллекцию
        /// </summary>
        private void Reader(string sourceFileName, string resultFileName)
        {
            using (StreamReader reader = new StreamReader(sourceFileName))
            {
                int numArrays = int.Parse(reader.ReadLine());
                for (int i = 0; i < numArrays - 1; i++)
                {
                    string line = reader.ReadLine();
                    // Чтение массива из файла
                    string[] numbers = line.Split(new char[] {' '}, StringSplitOptions.RemoveEmptyEntries);
                    int[] array = Array.ConvertAll(numbers, int.Parse);

                    //После прочтения массива запишем все результаты обработки другого потока в друго файл
                    lock (lockObj)
                    {
                        using (StreamWriter wr = File.AppendText(resultFileName))
                        {
                            foreach (int num in array) wr.Write(num + " ");
                            wr.WriteLine();
                            log($"Array was written in result file");
                        }
                    }
                    Thread.Sleep(100);
                }

                arrays.CompleteAdding();
            }
        }

        /// <summary>
        /// Вынимает массив целых чисел из коллекции, преобразует его в сумму и записывает в файл
        /// </summary>
        private void Processor(string sourceFileName, string resultFileName)
        {
            using (StreamReader reader = new StreamReader(sourceFileName))
            {
                int numArrays = int.Parse(reader.ReadLine());
                for (int i = 0; i < numArrays - 1; i++)
                {
                    string line = reader.ReadLine();
                    // Чтение массива из файла
                    string[] numbers = line.Split(new char[] { ' ' }, StringSplitOptions.RemoveEmptyEntries);
                    int[] array = Array.ConvertAll(numbers, int.Parse);

                    //После прочтения массива запишем все результаты обработки другого потока в друго файл
                    lock (lockObj)
                    {
                        using (StreamWriter wr = File.AppendText(resultFileName))
                        {
                            int arraySum = array.Sum();
                            wr.WriteLine(arraySum);
                            log($"Sum was written: {arraySum}");
                        }
                    }
                    Thread.Sleep(1500);
                }

                arrays.CompleteAdding();
            }

        }

        //Thread-safe ui updates
        private void log(string text)
        {
            if (logListBox.InvokeRequired)
                logListBox.Invoke(new Action<string>(log), text);
            else
            {
                logListBox.Items.Add(text);
            }
        }

        private void BackgroundProcessing_Load(object sender, EventArgs e)
        {

        }
    }


}
