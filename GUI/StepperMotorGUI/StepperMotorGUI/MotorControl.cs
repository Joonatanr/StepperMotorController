﻿using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace StepperMotorGUI
{
    public partial class MotorControl : UserControl
    {
        private string m_motorName = " ";

        public delegate void SendMotorCommandHandler(string cmd);
        public SendMotorCommandHandler SendMotorCommand = null;

        public string MotorName
        {
            get
            {
                return m_motorName;
            }
            set
            {
                m_motorName = value;
                groupBox1.Text = "Motor : " + m_motorName;
            }
        }

        public MotorControl()
        {
            InitializeComponent();

            /* Initialize radio buttons */
            foreach(RadioButton btn in groupBox1.Controls.OfType<RadioButton>())
            {
                //We have to test this...
                btn.CheckedChanged += new EventHandler(radioButtons_CheckedChanged);
            }
        }

        private void sendCommand(string cmd)
        {
            SendMotorCommand?.Invoke(MotorName + cmd);
        }


        private void numericUpDownSpeed_ValueChanged(object sender, EventArgs e)
        {
            UInt16 val = (UInt16)numericUpDownSpeed.Value;
            sendCommand("R" + val);
        }


        /* We have to add debounce somehow... */
        private void radioButtons_CheckedChanged(object sender, EventArgs e)
        {
            RadioButton radioButton = sender as RadioButton;
            UInt32 microstep;

            RadioButton rb = sender as RadioButton;

            if (rb != null)
            {
                if (!rb.Checked)
                {
                    /* Prevent this event from firing twice. */
                    return;
                }
            }

            if (radioButtonFullStep.Checked)
            {
                microstep = 1u;
            }
            else if (radioButtonHalfStep.Checked)
            {
                microstep = 2u;
            }
            else if (radioButtonQuarterStep.Checked)
            {
                microstep = 4u;
            }
            else if (radioButtonOne8.Checked)
            {
                microstep = 8u;
            }
            else if (radioButtonOne16.Checked)
            {
                microstep = 16u;
            }
            else if (radioButtonOne32.Checked)
            {
                microstep = 32u;
            }
            else
            {
                return;
                //Should not happen.
            }

            string cmd = "M" + microstep;
            sendCommand(cmd);
        }



        /* TODO : Remove this.*/
        private void groupBox1_Enter(object sender, EventArgs e)
        {

        }
    }
}
