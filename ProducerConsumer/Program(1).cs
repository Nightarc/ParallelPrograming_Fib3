using System;
using System.IO;
using System.Threading;

class Program
{
    static readonly object lockObj = new object(); // Объект блокировки для синхронизации доступа к файлу

    static void Main(string[] args)
    {
        // Путь к исходному файлу с массивами
        string sourceFilePath = "input.txt";
        // Путь к файлу, куда будут записываться суммы массивов
        string resultFilePath = "output.txt";

        // Создание потоков для чтения и обработки массивов
        Thread readerThread = new Thread(() => ReadArraysFromFile(sourceFilePath, resultFilePath));
        Thread processorThread = new Thread(() => ProcessArraysFromFile(sourceFilePath, resultFilePath));

        // Запуск потоков
        readerThread.Start();
        processorThread.Start();

        // Ожидание завершения обоих потоков
        readerThread.Join();
        processorThread.Join();

        Console.WriteLine("Готово!");
    }

    static void ReadArraysFromFile(string sourceFilePath, string resultFilePath)
    {
        using (StreamReader reader = new StreamReader(sourceFilePath))
        {
            string line;
            while ((line = reader.ReadLine()) != null)
            {
                // Чтение массива из файла
                string[] numbers = line.Split(' ');
                int[] array = Array.ConvertAll(numbers, int.Parse);

                lock (lockObj)
                {
                    using (StreamWriter writer = File.AppendText(resultFilePath))
                    {
                        foreach (int num in array)
                        {
                            writer.Write(num + " ");
                        }
                        writer.WriteLine();
                    }
                }

                Thread.Sleep(50);
            }
        }
    }

    static void ProcessArraysFromFile(string sourceFilePath, string resultFilePath)
    {
        using (StreamReader reader = new StreamReader(sourceFilePath))
        {
            Thread.Sleep(100);

            // Пропускаем первую строку с количеством массивов
            reader.ReadLine();

            string line;
            while ((line = reader.ReadLine()) != null)
            {
                string[] numbers = line.Split(' ');
                int[] array = Array.ConvertAll(numbers, int.Parse);

                int sum = 0;
                foreach (int num in array)
                {
                    sum += num;
                }

                lock (lockObj)
                {
                    using (StreamWriter writer = File.AppendText(resultFilePath))
                    {
                        writer.WriteLine(sum);
                    }
                }

                Thread.Sleep(500);
            }
        }
    }
}
