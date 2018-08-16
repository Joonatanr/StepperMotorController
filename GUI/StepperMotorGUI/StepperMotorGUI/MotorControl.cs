using System;
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
        }

        private void numericUpDownSpeed_ValueChanged(object sender, EventArgs e)
        {
            if (SendMotorCommand != null)
            {
                UInt16 val = (UInt16)numericUpDownSpeed.Value;
                SendMotorCommand(MotorName + "R" + val);
            }
        }
    }
}
