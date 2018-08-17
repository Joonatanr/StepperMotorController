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
        private const bool debugEnabled = true;

        public StepperMotorControlForm()
        {
            InitializeComponent();

            populateComPorts();

            motorControl1.SendMotorCommand = new MotorControl.SendMotorCommandHandler(sendComPortCommand);
            motorControl2.SendMotorCommand = new MotorControl.SendMotorCommandHandler(sendComPortCommand);
            motorControl3.SendMotorCommand = new MotorControl.SendMotorCommandHandler(sendComPortCommand);
            motorControl4.SendMotorCommand = new MotorControl.SendMotorCommandHandler(sendComPortCommand);
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
            if (debugEnabled)
            {
                printLine("Command : <" + cmd + ">");
            }

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
            printLog("Data Received:");
            printLog("<" + indata + ">");
        }


        private void printLog(string data)
        {
            if (richTextBox1.InvokeRequired)
            {
                this.Invoke(new Action<string>(printLog), new object[] { data });
            }
            else
            {
                richTextBox1.AppendText(data);
            }
        }


        private void printLine(string line)
        {
            printLog(line + Environment.NewLine);
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
