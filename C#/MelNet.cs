using System;
using System.Linq;
using System.Text;
using System.Net.Sockets;
using System.Net;

class MelNet
{
    UdpClient socket;
    IPEndPoint remoteHost;

    /// <summary>
    /// Default constructor. Use ports in the range of 49152 to 65535.
    /// </summary>
    public MelNet(int localPort, int remotePort, IPAddress remoteAddress, bool blocking = true)
    {
        try
        {
            socket = new UdpClient(localPort);
            socket.Client.Blocking = blocking;
            socket.Client.ReceiveTimeout = 10;
            socket.Client.SendTimeout = 10;
            remoteHost = new IPEndPoint(remoteAddress, remotePort);
        }
        catch
        {
            Console.WriteLine("Failed to open MelNet on local port " + localPort.ToString());
        }
    }

    /// <summary>
    /// Sends a vector of doubles to the remote host
    /// </summary>
    public void SendData(double[] data)
    {
        try
        {
            Byte[] bytes = new Byte[8 * data.Length];
            for (int i = 0; i < data.Length; ++i)
            {
                Byte[] temp = BitConverter.GetBytes(data[i]);
                for (int j = 0; j < 8; ++j)
                {
                    bytes[8 * i + j] = temp[j];
                }
            }
            socket.Send(bytes, bytes.Length, remoteHost);
        }
        catch
        {

        }
    }

    /// <summary>
    /// Receives a vector of doubles from the remote host
    /// </summary>
    public double[] ReceiveData()
    {
        try
        {
            Byte[] bytes = socket.Receive(ref remoteHost);
            int size = bytes.Length / 8;
            double[] data = new double[size];
            for (int i = 0; i < size; ++i)
                data[i] = BitConverter.ToDouble(bytes, 8 * i);
            return data;
        }
        catch
        {
            return new double[0];
        }
    }

    /// <summary>
    /// Sends a string message to the remote host
    /// </summary>
    public void SendMessage(string message)
    {
        try
        {
            Byte[] sizeBytes = BitConverter.GetBytes(message.Length);
            Array.Reverse(sizeBytes);
            Byte[] stringBytes = Encoding.ASCII.GetBytes(message);
            Byte[] bytes = new Byte[sizeBytes.Length + stringBytes.Length];
            Array.Copy(sizeBytes, bytes, sizeBytes.Length);
            Array.Copy(stringBytes, 0, bytes, sizeBytes.Length, stringBytes.Length);
            socket.Send(bytes, bytes.Length, remoteHost);
        }
        catch
        {

        }
    }

    /// <summary>
    /// Receives a string message from the remote host
    /// </summary>
    public string ReceiveMessage()
    {
        try
        {
            Byte[] bytes = socket.Receive(ref remoteHost);
            bytes = bytes.Skip(4).ToArray();
            return Encoding.ASCII.GetString(bytes);
        }
        catch
        {
            return "";
        }
    }

}
