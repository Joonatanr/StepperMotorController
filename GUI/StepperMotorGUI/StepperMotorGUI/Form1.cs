using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO.Ports;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace StepperMotorGUI
{
    public partial class StepperMotorControlForm : Form
    {
        private SerialPort mySerialPort;

        public StepperMotorControlForm()
        {
            InitializeComponent();

            populateComPorts();

            motorControl1.SendMotorCommand = new MotorControl.SendMotorCommandHandler(sendComPortCommand);
        }

        private void populateComPorts()
        {
            string[] ports = SerialPort.GetPortNames();
            foreach (string port in ports)
            {
                comboBoxComPorts.Items.Add(port);
            }
        }

        private void buttonOpen_Click(object sender, EventArgs e)
        {
            string portName;

            if (comboBoxComPorts.SelectedItem == null)
            {
                MessageBox.Show("No COM port selected");
                return;
            }

            portName = comboBoxComPorts.SelectedItem.ToString();
            printLine("Opening port :" + portName);

            try
            {
                mySerialPort = new SerialPort(portName, 9600);
                //mySerialPort.NewLine = "\r";
                mySerialPort.Open();
                //mySerialPort.DataReceived += new SerialDataReceivedEventHandler(DataReceivedHandler);
            }
            catch (Exception ex)
            {
                printLine("Failed to open COM port : " + ex.Message);
            }

        }

        private void sendComPortCommand(string cmd)
        {
            if (mySerialPort == null)
            {
                printLine("COM port not opened");
                return;
            }

            if (!mySerialPort.IsOpen)
            {
                printLine("COM port not opened");
                return;
            }

            try
            {
                mySerialPort.WriteLine(cmd);
            }
            catch (Exception ex)
            {
                printLine("Failed to send command " + cmd + " Got exception : " + ex.Message);
                return;
            }

            printLine("Sent command : " + cmd);
        }

        private void DataReceivedHandler(
                    object sender,
                    SerialDataReceivedEventArgs e)
        {
            SerialPort sp = (SerialPort)sender;
            string indata = sp.ReadExisting();
            printLine("Data Received:");
            printLine("<" + indata + ">");
        }


        private void printLine(string line)
        {
            if (richTextBox1.InvokeRequired)
            {
                this.Invoke(new Action<string>(printLine), new object[] { line });
            }
            else
            {
                richTextBox1.AppendText(line + Environment.NewLine);
            }
        }

        private void buttonClose_Click(object sender, EventArgs e)
        {
            if(mySerialPort != null)
            {
                mySerialPort.Close();
            }
        }
    }
}
