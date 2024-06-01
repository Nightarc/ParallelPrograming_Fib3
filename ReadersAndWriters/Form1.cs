using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;
namespace ReadersAndWriters
{
    public partial class Form1 : Form
    {
        const int READER_COUNT = 4;
        const int WRITER_COUNT = 2;
        const int LIBRARY_CAPACITY = 3;
        const int SPAWN_DELAY = 100;
        const int OPERATION_DELAY = 1000;
        public double frequency = 0;
        public int readingSpeed = 1000;
        public int writingSpeed = 1000;
        public Semaphore write;
        public Semaphore mutex;
        public int readersCount = 0;
        public Boolean isClosing = false;
        Random random;
        WorkersContainer workers;
        public Form1()
        {
            InitializeComponent();
            random = new Random();
            write = new Semaphore(1, 1);
            mutex = new Semaphore(1, 1);
            workers = new WorkersContainer();
            //setupTables();
            setupTrackBarLabels();
            frequency = FrequencyTrackBar.Value;
        }

        const int cellHeight = 100;
        const int cellWidth = 100;

        void fillTable(TableLayoutPanel table, int count)
        {
            table.ColumnCount = 0;
            table.RowCount = 1;
            for (int i = 0; i < count; i++)
            {
                PictureBox pictureBox = new PictureBox(); 
                pictureBox.Size = new Size(cellWidth, cellHeight);
                pictureBox.BackColor = Color.Gray;
                table.ColumnCount++;
                table.Controls.Add(pictureBox, i, 0);
            }
        }

        void setupTrackBarLabels()
        {
            DelayMinValueLabel.Text = FrequencyTrackBar.Minimum.ToString();
            DelayMaxValueLabel.Text = FrequencyTrackBar.Maximum.ToString();
            ReadDelayMinValueLabel.Text = ReadingSpeedTrackBar.Minimum.ToString();
            ReadDelayMaxValueLabel.Text = ReadingSpeedTrackBar.Maximum.ToString();
            WriteDelayMinValueLabel.Text = WritingSpeedTrackBar.Minimum.ToString();
            WriteDelayMaxValueLabel.Text = WritingSpeedTrackBar.Maximum.ToString();

        }
        void setupTables()
        {
            fillTable(readerTable, READER_COUNT);
            fillTable(writerTable, WRITER_COUNT);           
        }

        void setupStreams()
        {
            workers.addReader(new Reader(this, reader1PictureBox, reader1DelayTrackBar, 1, readingSpeed));
            workers.addReader(new Reader(this, reader2PictureBox, reader2DelayTrackBar, 2, readingSpeed));
            workers.addReader(new Reader(this, reader3PictureBox, reader3DelayTrackBar, 3, readingSpeed));

            workers.addWriter(new Writer(this, writer1PictureBox, writer1DelayTrackBar, 1, writingSpeed));
            workers.addWriter(new Writer(this, writer2PictureBox, writer2DelayTrackBar, 2, writingSpeed));
            workers.addWriter(new Writer(this, writer3PictureBox, writer3DelayTrackBar, 3, writingSpeed));

            reader1DelayTrackBar.Enabled = true;
            reader2DelayTrackBar.Enabled = true;
            reader3DelayTrackBar.Enabled = true;

            writer1DelayTrackBar.Enabled = true;
            writer2DelayTrackBar.Enabled = true;
            writer3DelayTrackBar.Enabled = true;
        }
       
        private void mainCycleTimer_Tick(object sender, EventArgs e)
        {
            //mainCycle();
        }


        private void FrequencyTrackBar_Scroll(object sender, EventArgs e)
        {
            frequency = FrequencyTrackBar.Value;
            workers.updateWorkersDelay(frequency);

        }

        private void ReadingSpeedTrackBar_Scroll(object sender, EventArgs e)
        {
            readingSpeed = ReadingSpeedTrackBar.Value;
        }

        private void WritingSpeedTrackBar_Scroll(object sender, EventArgs e)
        {
            writingSpeed = WritingSpeedTrackBar.Value;
        }

        private void StartButton_Click(object sender, EventArgs e)
        {
            mainCycleTimer.Interval = FrequencyTrackBar.Value;
            readingSpeed = ReadingSpeedTrackBar.Value;
            writingSpeed = WritingSpeedTrackBar.Value;

            ((Button)sender).Enabled = false;
            setupStreams();

        }

        private void Form1_Load(object sender, EventArgs e)
        {

        }

        private void Form1_FormClosed(object sender, FormClosedEventArgs e)
        {
            isClosing = true;
        }

        private void readerTrackBarChange(object sender, EventArgs e)
        {
            TrackBar trackBar = sender as TrackBar;
            Reader reader = workers.GetReader(int.Parse(trackBar.Tag.ToString()));
            reader.sleepDelay = frequency * ((double)trackBar.Value / 1000);
        }

        private void WriterTrackBarChange(object sender, EventArgs e)
        {
            TrackBar trackBar = sender as TrackBar;
            Writer writer = workers.GetWriter(int.Parse(trackBar.Tag.ToString()));
            writer.sleepDelay = frequency * ((double)trackBar.Value / 1000);
        }
    }
}
