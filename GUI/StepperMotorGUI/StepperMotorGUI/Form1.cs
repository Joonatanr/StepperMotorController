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

        private List<MotorControl> StepperMotors = new List<MotorControl>();

        public StepperMotorControlForm()
        {
            InitializeComponent();

            populateComPorts();

            foreach(MotorControl m in groupBoxStepperMotors.Controls.OfType<MotorControl>())
            {
                /* We create a list so that we don't have to access each control separately all the time. */
                StepperMotors.Add(m);
            }

            /* Connect the command handlers for each motor control to the main COM port. */
            foreach(MotorControl m in StepperMotors)
            {
                m.SendMotorCommand = new MotorControl.SendMotorCommandHandler(sendComPortCommand);
            }

            updateStatus("Disconnected");
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
                mySerialPort.ReadTimeout = 1000;
                mySerialPort.WriteTimeout = 1000;

                mySerialPort.Open();
                updateMotorStatus();
                updateStatus("Connected");
                mySerialPort.DataReceived += new SerialDataReceivedEventHandler(DataReceivedHandler);
            }
            catch (Exception ex)
            {
                printLine("Failed to open COM port : " + ex.Message);
            }

        }

        private void updateMotorStatus()
        {
            /* Send a query string to get the current motor status. */
            sendComPortCommand("Q");
            try
            {
                /* Echo would break this.... */
                string res = mySerialPort.ReadLine();

                /* Lets try to resolve the echo issue... */
                if (res[0] == ':')
                {
                    /* Not sure if this will work, needs testing. */
                    res = mySerialPort.ReadLine();
                }

                foreach(MotorControl m in StepperMotors)
                {
                    m.HandleStatusString(res);
                }
                
            }
            catch (Exception ex)
            {
                printLine("Failed to get motor state : " + ex.Message);
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
                printLine("Tx: " + cmd);
            }
            catch (Exception ex)
            {
                printLine("Failed to send command " + cmd + " Got exception : " + ex.Message);
                return;
            }
        }

        private void DataReceivedHandler(
                    object sender,
                    SerialDataReceivedEventArgs e)
        {
            SerialPort sp = (SerialPort)sender;
            string indata = sp.ReadExisting();
            printLog("Rx:");
            printLog(indata);
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
            if (mySerialPort != null)
            {
                mySerialPort.Close();
                updateStatus("Disconnected");
            }
        }

        private void updateStatus(string stat)
        {
            toolStripStatusLabel1.Text = "Status : " + stat;
        }


        /* TODO : Remove this. */
        private void StepperMotorControlForm_Load(object sender, EventArgs e)
        {

        }
    }
}
