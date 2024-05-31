namespace ProducerAndConsumer
{
    partial class Form1
    {
        /// <summary>
        /// Обязательная переменная конструктора.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Освободить все используемые ресурсы.
        /// </summary>
        /// <param name="disposing">истинно, если управляемый ресурс должен быть удален; иначе ложно.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Код, автоматически созданный конструктором форм Windows

        /// <summary>
        /// Требуемый метод для поддержки конструктора — не изменяйте 
        /// содержимое этого метода с помощью редактора кода.
        /// </summary>
        private void InitializeComponent()
        {
            this.bufferListBox = new System.Windows.Forms.ListBox();
            this.startButton = new System.Windows.Forms.Button();
            this.statusLabel = new System.Windows.Forms.Label();
            this.stopButton = new System.Windows.Forms.Button();
            this.producerTrackBar = new System.Windows.Forms.TrackBar();
            this.CustomerTrackBar2 = new System.Windows.Forms.TrackBar();
            this.ResultRichTextBox = new System.Windows.Forms.RichTextBox();
            ((System.ComponentModel.ISupportInitialize)(this.producerTrackBar)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.CustomerTrackBar2)).BeginInit();
            this.SuspendLayout();
            // 
            // bufferListBox
            // 
            this.bufferListBox.FormattingEnabled = true;
            this.bufferListBox.ItemHeight = 16;
            this.bufferListBox.Location = new System.Drawing.Point(143, 100);
            this.bufferListBox.Name = "bufferListBox";
            this.bufferListBox.Size = new System.Drawing.Size(120, 180);
            this.bufferListBox.TabIndex = 0;
            // 
            // startButton
            // 
            this.startButton.Location = new System.Drawing.Point(169, 342);
            this.startButton.Name = "startButton";
            this.startButton.Size = new System.Drawing.Size(75, 45);
            this.startButton.TabIndex = 1;
            this.startButton.Text = "Start";
            this.startButton.UseVisualStyleBackColor = true;
            this.startButton.Click += new System.EventHandler(this.startButton_Click);
            // 
            // statusLabel
            // 
            this.statusLabel.AutoSize = true;
            this.statusLabel.Location = new System.Drawing.Point(178, 61);
            this.statusLabel.Name = "statusLabel";
            this.statusLabel.Size = new System.Drawing.Size(44, 16);
            this.statusLabel.TabIndex = 2;
            this.statusLabel.Text = "label1";
            // 
            // stopButton
            // 
            this.stopButton.Location = new System.Drawing.Point(463, 353);
            this.stopButton.Name = "stopButton";
            this.stopButton.Size = new System.Drawing.Size(84, 34);
            this.stopButton.TabIndex = 3;
            this.stopButton.Text = "Stop";
            this.stopButton.UseVisualStyleBackColor = true;
            this.stopButton.Click += new System.EventHandler(this.button1_Click);
            // 
            // producerTrackBar
            // 
            this.producerTrackBar.LargeChange = 2;
            this.producerTrackBar.Location = new System.Drawing.Point(632, 41);
            this.producerTrackBar.Maximum = 4;
            this.producerTrackBar.Name = "producerTrackBar";
            this.producerTrackBar.Orientation = System.Windows.Forms.Orientation.Vertical;
            this.producerTrackBar.Size = new System.Drawing.Size(56, 104);
            this.producerTrackBar.TabIndex = 5;
            this.producerTrackBar.ValueChanged += new System.EventHandler(this.producerTrackBar_ValueChanged);
            // 
            // CustomerTrackBar2
            // 
            this.CustomerTrackBar2.LargeChange = 2;
            this.CustomerTrackBar2.Location = new System.Drawing.Point(723, 41);
            this.CustomerTrackBar2.Maximum = 4;
            this.CustomerTrackBar2.Name = "CustomerTrackBar2";
            this.CustomerTrackBar2.Orientation = System.Windows.Forms.Orientation.Vertical;
            this.CustomerTrackBar2.Size = new System.Drawing.Size(56, 104);
            this.CustomerTrackBar2.TabIndex = 6;
            this.CustomerTrackBar2.ValueChanged += new System.EventHandler(this.CustomerTrackBar2_ValueChanged);
            // 
            // ResultRichTextBox
            // 
            this.ResultRichTextBox.Location = new System.Drawing.Point(447, 100);
            this.ResultRichTextBox.Name = "ResultRichTextBox";
            this.ResultRichTextBox.Size = new System.Drawing.Size(138, 180);
            this.ResultRichTextBox.TabIndex = 7;
            this.ResultRichTextBox.Text = "";
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(800, 450);
            this.Controls.Add(this.ResultRichTextBox);
            this.Controls.Add(this.CustomerTrackBar2);
            this.Controls.Add(this.producerTrackBar);
            this.Controls.Add(this.stopButton);
            this.Controls.Add(this.statusLabel);
            this.Controls.Add(this.startButton);
            this.Controls.Add(this.bufferListBox);
            this.Name = "Form1";
            this.Text = "Form1";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.MainForm_FormClosing);
            this.Load += new System.EventHandler(this.MainForm_Load);
            ((System.ComponentModel.ISupportInitialize)(this.producerTrackBar)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.CustomerTrackBar2)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.ListBox bufferListBox;
        private System.Windows.Forms.Button startButton;
        private System.Windows.Forms.Label statusLabel;
        private System.Windows.Forms.Button stopButton;
        private System.Windows.Forms.TrackBar producerTrackBar;
        private System.Windows.Forms.TrackBar CustomerTrackBar2;
        private System.Windows.Forms.RichTextBox ResultRichTextBox;
    }
}

