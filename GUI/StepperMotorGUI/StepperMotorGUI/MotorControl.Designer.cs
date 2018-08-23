namespace StepperMotorGUI
{
    partial class MotorControl
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

        #region Component Designer generated code

        /// <summary> 
        /// Required method for Designer support - do not modify 
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.numericUpDownSpeed = new System.Windows.Forms.NumericUpDown();
            this.label1 = new System.Windows.Forms.Label();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.radioButtonOne32 = new System.Windows.Forms.RadioButton();
            this.radioButtonOne16 = new System.Windows.Forms.RadioButton();
            this.radioButtonQuarterStep = new System.Windows.Forms.RadioButton();
            this.radioButtonOne8 = new System.Windows.Forms.RadioButton();
            this.radioButtonHalfStep = new System.Windows.Forms.RadioButton();
            this.radioButtonFullStep = new System.Windows.Forms.RadioButton();
            this.label2 = new System.Windows.Forms.Label();
            this.backgroundWorker1 = new System.ComponentModel.BackgroundWorker();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDownSpeed)).BeginInit();
            this.groupBox1.SuspendLayout();
            this.SuspendLayout();
            // 
            // numericUpDownSpeed
            // 
            this.numericUpDownSpeed.Increment = new decimal(new int[] {
            10,
            0,
            0,
            0});
            this.numericUpDownSpeed.Location = new System.Drawing.Point(172, 21);
            this.numericUpDownSpeed.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.numericUpDownSpeed.Maximum = new decimal(new int[] {
            240,
            0,
            0,
            0});
            this.numericUpDownSpeed.Name = "numericUpDownSpeed";
            this.numericUpDownSpeed.Size = new System.Drawing.Size(120, 22);
            this.numericUpDownSpeed.TabIndex = 0;
            this.numericUpDownSpeed.ValueChanged += new System.EventHandler(this.numericUpDownSpeed_ValueChanged);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(23, 22);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(131, 17);
            this.label1.TabIndex = 1;
            this.label1.Text = "Motor speed (RPM)";
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.radioButtonOne32);
            this.groupBox1.Controls.Add(this.radioButtonOne16);
            this.groupBox1.Controls.Add(this.radioButtonQuarterStep);
            this.groupBox1.Controls.Add(this.radioButtonOne8);
            this.groupBox1.Controls.Add(this.radioButtonHalfStep);
            this.groupBox1.Controls.Add(this.radioButtonFullStep);
            this.groupBox1.Controls.Add(this.label2);
            this.groupBox1.Controls.Add(this.numericUpDownSpeed);
            this.groupBox1.Controls.Add(this.label1);
            this.groupBox1.Location = new System.Drawing.Point(3, -1);
            this.groupBox1.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Padding = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.groupBox1.Size = new System.Drawing.Size(308, 193);
            this.groupBox1.TabIndex = 2;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "groupBox1";
            this.groupBox1.Enter += new System.EventHandler(this.groupBox1_Enter);
            // 
            // radioButtonOne32
            // 
            this.radioButtonOne32.AutoSize = true;
            this.radioButtonOne32.Checked = true;
            this.radioButtonOne32.Location = new System.Drawing.Point(172, 164);
            this.radioButtonOne32.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
            this.radioButtonOne32.Name = "radioButtonOne32";
            this.radioButtonOne32.Size = new System.Drawing.Size(88, 21);
            this.radioButtonOne32.TabIndex = 8;
            this.radioButtonOne32.TabStop = true;
            this.radioButtonOne32.Text = "1/32 step";
            this.radioButtonOne32.UseVisualStyleBackColor = true;
            // 
            // radioButtonOne16
            // 
            this.radioButtonOne16.AutoSize = true;
            this.radioButtonOne16.Location = new System.Drawing.Point(172, 142);
            this.radioButtonOne16.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
            this.radioButtonOne16.Name = "radioButtonOne16";
            this.radioButtonOne16.Size = new System.Drawing.Size(88, 21);
            this.radioButtonOne16.TabIndex = 7;
            this.radioButtonOne16.Text = "1/16 step";
            this.radioButtonOne16.UseVisualStyleBackColor = true;
            // 
            // radioButtonQuarterStep
            // 
            this.radioButtonQuarterStep.AutoSize = true;
            this.radioButtonQuarterStep.Location = new System.Drawing.Point(172, 97);
            this.radioButtonQuarterStep.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
            this.radioButtonQuarterStep.Name = "radioButtonQuarterStep";
            this.radioButtonQuarterStep.Size = new System.Drawing.Size(80, 21);
            this.radioButtonQuarterStep.TabIndex = 6;
            this.radioButtonQuarterStep.Text = "1/4 step";
            this.radioButtonQuarterStep.UseVisualStyleBackColor = true;
            // 
            // radioButtonOne8
            // 
            this.radioButtonOne8.AutoSize = true;
            this.radioButtonOne8.Location = new System.Drawing.Point(172, 119);
            this.radioButtonOne8.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
            this.radioButtonOne8.Name = "radioButtonOne8";
            this.radioButtonOne8.Size = new System.Drawing.Size(80, 21);
            this.radioButtonOne8.TabIndex = 5;
            this.radioButtonOne8.Text = "1/8 step";
            this.radioButtonOne8.UseVisualStyleBackColor = true;
            // 
            // radioButtonHalfStep
            // 
            this.radioButtonHalfStep.AutoSize = true;
            this.radioButtonHalfStep.Location = new System.Drawing.Point(172, 75);
            this.radioButtonHalfStep.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
            this.radioButtonHalfStep.Name = "radioButtonHalfStep";
            this.radioButtonHalfStep.Size = new System.Drawing.Size(80, 21);
            this.radioButtonHalfStep.TabIndex = 4;
            this.radioButtonHalfStep.Text = "1/2 step";
            this.radioButtonHalfStep.UseVisualStyleBackColor = true;
            // 
            // radioButtonFullStep
            // 
            this.radioButtonFullStep.AutoSize = true;
            this.radioButtonFullStep.Location = new System.Drawing.Point(172, 53);
            this.radioButtonFullStep.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
            this.radioButtonFullStep.Name = "radioButtonFullStep";
            this.radioButtonFullStep.Size = new System.Drawing.Size(84, 21);
            this.radioButtonFullStep.TabIndex = 3;
            this.radioButtonFullStep.Text = "Full Step";
            this.radioButtonFullStep.UseVisualStyleBackColor = true;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(23, 54);
            this.label2.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(96, 17);
            this.label2.TabIndex = 2;
            this.label2.Text = "Microstepping";
            // 
            // MotorControl
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.groupBox1);
            this.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.Name = "MotorControl";
            this.Size = new System.Drawing.Size(320, 197);
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDownSpeed)).EndInit();
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.NumericUpDown numericUpDownSpeed;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.ComponentModel.BackgroundWorker backgroundWorker1;
        private System.Windows.Forms.RadioButton radioButtonHalfStep;
        private System.Windows.Forms.RadioButton radioButtonFullStep;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.RadioButton radioButtonOne32;
        private System.Windows.Forms.RadioButton radioButtonOne16;
        private System.Windows.Forms.RadioButton radioButtonQuarterStep;
        private System.Windows.Forms.RadioButton radioButtonOne8;
    }
}
