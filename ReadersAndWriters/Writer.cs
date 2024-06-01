using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace ReadersAndWriters
{
    internal class Writer
    {
        Thread thread;
        Form1 mainForm;
        PictureBox picture;
        public TrackBar sleepDelayTrackBar;
        public double sleepDelay;

        public Writer(Form1 mainForm, PictureBox picture, TrackBar sleepDelayTrackBar, int i, double sleepDelay)
        {
            this.mainForm = mainForm;
            this.picture = picture;
            this.sleepDelayTrackBar = sleepDelayTrackBar;
            this.sleepDelay = sleepDelay;
            thread = new Thread(Write);
            thread.Name = $"Писатель {i}";
            thread.Start();

        }

        void Write()
        {
            while (!mainForm.isClosing) { 
                picture.BackColor = System.Drawing.Color.Magenta;
                mainForm.write.WaitOne();

                picture.BackColor = System.Drawing.Color.Yellow;
                Thread.Sleep(mainForm.writingSpeed);

                picture.BackColor = System.Drawing.Color.Gray;
                mainForm.write.Release();

                Thread.Sleep(Convert.ToInt32(sleepDelay));
            }
        }
    }
}
