namespace MyProducerConsumer
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
            this.logListBox = new System.Windows.Forms.ListBox();
            this.button1 = new System.Windows.Forms.Button();
            this.stopButton = new System.Windows.Forms.Button();
            this.storageListBox = new System.Windows.Forms.ListBox();
            this.producerButton = new System.Windows.Forms.Button();
            this.consumerButton = new System.Windows.Forms.Button();
            this.Clear = new System.Windows.Forms.Button();
            this.lastLogMessage = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // logListBox
            // 
            this.logListBox.FormattingEnabled = true;
            this.logListBox.Location = new System.Drawing.Point(12, 12);
            this.logListBox.Name = "logListBox";
            this.logListBox.Size = new System.Drawing.Size(193, 303);
            this.logListBox.TabIndex = 0;
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point(175, 346);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(75, 23);
            this.button1.TabIndex = 1;
            this.button1.Text = "Start";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.button1_Click);
            // 
            // stopButton
            // 
            this.stopButton.Location = new System.Drawing.Point(256, 346);
            this.stopButton.Name = "stopButton";
            this.stopButton.Size = new System.Drawing.Size(75, 23);
            this.stopButton.TabIndex = 2;
            this.stopButton.Text = "Stop";
            this.stopButton.UseVisualStyleBackColor = true;
            this.stopButton.Click += new System.EventHandler(this.stopButton_Click);
            // 
            // storageListBox
            // 
            this.storageListBox.FormattingEnabled = true;
            this.storageListBox.Location = new System.Drawing.Point(211, 12);
            this.storageListBox.Name = "storageListBox";
            this.storageListBox.Size = new System.Drawing.Size(193, 303);
            this.storageListBox.TabIndex = 3;
            // 
            // producerButton
            // 
            this.producerButton.Location = new System.Drawing.Point(12, 346);
            this.producerButton.Name = "producerButton";
            this.producerButton.Size = new System.Drawing.Size(75, 23);
            this.producerButton.TabIndex = 4;
            this.producerButton.Text = "Producer";
            this.producerButton.UseVisualStyleBackColor = true;
            this.producerButton.Click += new System.EventHandler(this.producerButton_Click);
            // 
            // consumerButton
            // 
            this.consumerButton.Location = new System.Drawing.Point(93, 346);
            this.consumerButton.Name = "consumerButton";
            this.consumerButton.Size = new System.Drawing.Size(75, 23);
            this.consumerButton.TabIndex = 5;
            this.consumerButton.Text = "Consumer";
            this.consumerButton.UseVisualStyleBackColor = true;
            this.consumerButton.Click += new System.EventHandler(this.consumerButton_Click);
            // 
            // Clear
            // 
            this.Clear.Location = new System.Drawing.Point(354, 346);
            this.Clear.Name = "Clear";
            this.Clear.Size = new System.Drawing.Size(75, 23);
            this.Clear.TabIndex = 6;
            this.Clear.Text = "Clear";
            this.Clear.UseVisualStyleBackColor = true;
            this.Clear.Click += new System.EventHandler(this.Clear_Click);
            // 
            // lastLogMessage
            // 
            this.lastLogMessage.AutoSize = true;
            this.lastLogMessage.Location = new System.Drawing.Point(12, 318);
            this.lastLogMessage.Name = "lastLogMessage";
            this.lastLogMessage.Size = new System.Drawing.Size(35, 13);
            this.lastLogMessage.TabIndex = 7;
            this.lastLogMessage.Text = "label1";
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(441, 381);
            this.Controls.Add(this.lastLogMessage);
            this.Controls.Add(this.Clear);
            this.Controls.Add(this.consumerButton);
            this.Controls.Add(this.producerButton);
            this.Controls.Add(this.storageListBox);
            this.Controls.Add(this.stopButton);
            this.Controls.Add(this.button1);
            this.Controls.Add(this.logListBox);
            this.Name = "Form1";
            this.Text = "ProducerConsumer";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.ListBox logListBox;
        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.Button stopButton;
        private System.Windows.Forms.ListBox storageListBox;
        private System.Windows.Forms.Button producerButton;
        private System.Windows.Forms.Button consumerButton;
        private System.Windows.Forms.Button Clear;
        private System.Windows.Forms.Label lastLogMessage;
    }
}

