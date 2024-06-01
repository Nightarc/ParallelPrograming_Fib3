using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace ReadersAndWriters
{

    internal class Reader
    {
        Thread thread;
        Form1 mainForm;
        PictureBox picture;
        public TrackBar sleepDelayTrackBar;
        public double sleepDelay;

        public Reader(Form1 mainForm, PictureBox picture, TrackBar sleepDelayTrackBar, int i, double sleepDelay)
        {
            this.mainForm = mainForm;
            this.picture = picture;
            this.sleepDelayTrackBar = sleepDelayTrackBar;
            this.sleepDelay = sleepDelay;

            thread = new Thread(Read_S);
            thread.Name = $"Читатель {i}";
            thread.Start();

        }

        void Read_S()
        {
            while (!mainForm.isClosing)
            {
                picture.BackColor = System.Drawing.Color.Magenta;
                mainForm.mutex.WaitOne();
                mainForm.readersCount++;
                if (mainForm.readersCount == 1) mainForm.write.WaitOne();
                mainForm.mutex.Release();

                picture.BackColor = System.Drawing.Color.Yellow;
                Thread.Sleep(mainForm.readingSpeed);

                mainForm.mutex.WaitOne();
                mainForm.readersCount--;
                if (mainForm.readersCount == 0) mainForm.write.Release();
                mainForm.mutex.Release();
                picture.BackColor = System.Drawing.Color.Gray;

                Thread.Sleep(Convert.ToInt32(sleepDelay));
            }
        }
    }
}
