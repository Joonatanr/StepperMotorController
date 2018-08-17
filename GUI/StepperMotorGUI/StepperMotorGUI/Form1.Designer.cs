namespace StepperMotorGUI
{
    partial class StepperMotorControlForm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.buttonClose = new System.Windows.Forms.Button();
            this.buttonOpen = new System.Windows.Forms.Button();
            this.label1 = new System.Windows.Forms.Label();
            this.comboBoxComPorts = new System.Windows.Forms.ComboBox();
            this.richTextBox1 = new System.Windows.Forms.RichTextBox();
            this.motorControl4 = new StepperMotorGUI.MotorControl();
            this.motorControl3 = new StepperMotorGUI.MotorControl();
            this.motorControl2 = new StepperMotorGUI.MotorControl();
            this.motorControl1 = new StepperMotorGUI.MotorControl();
            this.groupBoxStepperMotors = new System.Windows.Forms.GroupBox();
            this.groupBox1.SuspendLayout();
            this.groupBoxStepperMotors.SuspendLayout();
            this.SuspendLayout();
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.buttonClose);
            this.groupBox1.Controls.Add(this.buttonOpen);
            this.groupBox1.Controls.Add(this.label1);
            this.groupBox1.Controls.Add(this.comboBoxComPorts);
            this.groupBox1.Location = new System.Drawing.Point(9, 18);
            this.groupBox1.Margin = new System.Windows.Forms.Padding(2);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Padding = new System.Windows.Forms.Padding(2);
            this.groupBox1.Size = new System.Drawing.Size(522, 81);
            this.groupBox1.TabIndex = 1;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "COM port";
            // 
            // buttonClose
            // 
            this.buttonClose.Location = new System.Drawing.Point(92, 43);
            this.buttonClose.Margin = new System.Windows.Forms.Padding(2);
            this.buttonClose.Name = "buttonClose";
            this.buttonClose.Size = new System.Drawing.Size(72, 25);
            this.buttonClose.TabIndex = 3;
            this.buttonClose.Text = "Close";
            this.buttonClose.UseVisualStyleBackColor = true;
            this.buttonClose.Click += new System.EventHandler(this.buttonClose_Click);
            // 
            // buttonOpen
            // 
            this.buttonOpen.Location = new System.Drawing.Point(17, 43);
            this.buttonOpen.Margin = new System.Windows.Forms.Padding(2);
            this.buttonOpen.Name = "buttonOpen";
            this.buttonOpen.Size = new System.Drawing.Size(70, 25);
            this.buttonOpen.TabIndex = 2;
            this.buttonOpen.Text = "Open";
            this.buttonOpen.UseVisualStyleBackColor = true;
            this.buttonOpen.Click += new System.EventHandler(this.buttonOpen_Click);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(15, 20);
            this.label1.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(29, 13);
            this.label1.TabIndex = 1;
            this.label1.Text = "Port:";
            // 
            // comboBoxComPorts
            // 
            this.comboBoxComPorts.FormattingEnabled = true;
            this.comboBoxComPorts.Location = new System.Drawing.Point(73, 19);
            this.comboBoxComPorts.Margin = new System.Windows.Forms.Padding(2);
            this.comboBoxComPorts.Name = "comboBoxComPorts";
            this.comboBoxComPorts.Size = new System.Drawing.Size(92, 21);
            this.comboBoxComPorts.TabIndex = 0;
            // 
            // richTextBox1
            // 
            this.richTextBox1.Location = new System.Drawing.Point(9, 470);
            this.richTextBox1.Margin = new System.Windows.Forms.Padding(2);
            this.richTextBox1.Name = "richTextBox1";
            this.richTextBox1.Size = new System.Drawing.Size(522, 147);
            this.richTextBox1.TabIndex = 2;
            this.richTextBox1.Text = "";
            // 
            // motorControl4
            // 
            this.motorControl4.Location = new System.Drawing.Point(260, 189);
            this.motorControl4.Margin = new System.Windows.Forms.Padding(2);
            this.motorControl4.MotorName = "S3";
            this.motorControl4.Name = "motorControl4";
            this.motorControl4.Size = new System.Drawing.Size(250, 167);
            this.motorControl4.TabIndex = 5;
            // 
            // motorControl3
            // 
            this.motorControl3.Location = new System.Drawing.Point(8, 189);
            this.motorControl3.Margin = new System.Windows.Forms.Padding(2);
            this.motorControl3.MotorName = "S2";
            this.motorControl3.Name = "motorControl3";
            this.motorControl3.Size = new System.Drawing.Size(250, 167);
            this.motorControl3.TabIndex = 4;
            // 
            // motorControl2
            // 
            this.motorControl2.Location = new System.Drawing.Point(260, 18);
            this.motorControl2.Margin = new System.Windows.Forms.Padding(2);
            this.motorControl2.MotorName = "S1";
            this.motorControl2.Name = "motorControl2";
            this.motorControl2.Size = new System.Drawing.Size(250, 167);
            this.motorControl2.TabIndex = 3;
            // 
            // motorControl1
            // 
            this.motorControl1.Location = new System.Drawing.Point(8, 18);
            this.motorControl1.Margin = new System.Windows.Forms.Padding(2);
            this.motorControl1.MotorName = "S0";
            this.motorControl1.Name = "motorControl1";
            this.motorControl1.Size = new System.Drawing.Size(250, 167);
            this.motorControl1.TabIndex = 0;
            // 
            // groupBoxStepperMotors
            // 
            this.groupBoxStepperMotors.Controls.Add(this.motorControl1);
            this.groupBoxStepperMotors.Controls.Add(this.motorControl4);
            this.groupBoxStepperMotors.Controls.Add(this.motorControl2);
            this.groupBoxStepperMotors.Controls.Add(this.motorControl3);
            this.groupBoxStepperMotors.Location = new System.Drawing.Point(9, 104);
            this.groupBoxStepperMotors.Name = "groupBoxStepperMotors";
            this.groupBoxStepperMotors.Size = new System.Drawing.Size(522, 361);
            this.groupBoxStepperMotors.TabIndex = 6;
            this.groupBoxStepperMotors.TabStop = false;
            this.groupBoxStepperMotors.Text = "Stepper Motors";
            // 
            // StepperMotorControlForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(546, 628);
            this.Controls.Add(this.groupBoxStepperMotors);
            this.Controls.Add(this.richTextBox1);
            this.Controls.Add(this.groupBox1);
            this.Margin = new System.Windows.Forms.Padding(2);
            this.Name = "StepperMotorControlForm";
            this.Text = "Motor Controller";
            this.Load += new System.EventHandler(this.StepperMotorControlForm_Load);
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.groupBoxStepperMotors.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private MotorControl motorControl1;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.Button buttonClose;
        private System.Windows.Forms.Button buttonOpen;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.ComboBox comboBoxComPorts;
        private System.Windows.Forms.RichTextBox richTextBox1;
        private MotorControl motorControl2;
        private MotorControl motorControl3;
        private MotorControl motorControl4;
        private System.Windows.Forms.GroupBox groupBoxStepperMotors;
    }
}

